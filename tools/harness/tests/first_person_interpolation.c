#include "harness.h"
#include <math.h>

#include "config.inc"

/* One local player's first-person weapon of two nodes, drawn from a camera at
the origin looking along x (render_interpolation.c). Each tick the game poses
the nodes in the world from the camera; each frame between ticks the pose is
blended from the last tick's. */
#define WEAPON 0x10002L
#define OTHER_WEAPON 0x10003L
#define GRAPH 0x20004L
#define OTHER_GRAPH 0x20005L
#define NODES 2
#define PI 3.14159265f

struct render_camera { real_point3d position; real_vector3d forward, up; };
static void cross_product3d(real_vector3d const *a, real_vector3d const *b, real_vector3d *result)
{
	result->i = a->j * b->k - a->k * b->j;
	result->j = a->k * b->i - a->i * b->k;
	result->k = a->i * b->j - a->j * b->i;
}
static void set_real_point3d(real_point3d *point, real x, real y, real z) { point->x = x; point->y = y; point->z = z; }
static long interpolation_tick;
static boolean interpolation_rendering = TRUE;
static real interpolation_fraction;

#include "under_test.inc"

enum {_object_type_biped, _object_type_vehicle, MAXIMUM_FILENAME_LENGTH=255, MAXIMUM_NODES_PER_ANIMATION=2};
#define MODELS_GROUP_TAG 0x6d6f6465L
#define MIN(a, b) ((a) < (b) ? (a) : (b))
struct tag_block { long count; void *address; };
#define TAG_BLOCK_GET_ELEMENT(block, index, type) (&((type *)(block)->address)[index])
#define csstrcmp strcmp
struct unit_datum { long definition_index; struct {short type;} object; };
struct model_node { char name[32]; };
struct animation_graph_node { char name[32]; };
struct model { struct tag_block nodes; };
struct animation_graph { struct tag_block nodes; };
static struct model_node stock_nodes[]={{"left hand"}}, alternate_nodes[]={{"right hand"}};
static struct animation_graph_node graph_nodes[]={{"left hand"},{"right hand"}};
static struct animation_graph hand_graph={{2,graph_nodes}};
static struct model stock_hands={{1,stock_nodes}}, alternate_hands={{1,alternate_nodes}};
static boolean alternate_present=TRUE;
static struct model *model_definition_get(long index) { return index==1 ? &stock_hands : &alternate_hands; }
static struct animation_graph *animation_graph_definition_get(long graph) { return &hand_graph; }
static char const *tag_get_name(long definition) { return definition==1 ? "characters\\elite\\elite" : NULL; }
static long tag_loaded(long group, char const *name)
{
	CHECK(group==MODELS_GROUP_TAG && !strcmp(name,"characters\\elite\\elite_fp"),"hands lookup used an unrelated tag");
	return alternate_present ? 2 : NONE;
}
#include "hands.inc"

static struct render_camera camera = { { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } };
static real_matrix4x3 nodes[NODES];

/* a node so far along x, turned so far about z */
static void pose(short node, real x, real angle)
{
	real_matrix4x3 *matrix = &nodes[node];

	matrix->scale = 1.0f;
	matrix->forward.i = (real)cos(angle);
	matrix->forward.j = (real)sin(angle);
	matrix->forward.k = 0.0f;
	matrix->left.i = -matrix->forward.j;
	matrix->left.j = matrix->forward.i;
	matrix->left.k = 0.0f;
	matrix->up.i = 0.0f;
	matrix->up.j = 0.0f;
	matrix->up.k = 1.0f;
	set_real_point3d(&matrix->position, x, 0.0f, 0.0f);
}

/* a frame drawn a fraction into a tick, both nodes so far along x */
static void frame(long tick, real fraction, long weapon, long graph, real x)
{
	interpolation_tick = tick;
	interpolation_fraction = fraction;
	pose(0, x, 0.0f);
	pose(1, x, 0.0f);
	render_interpolation_first_person(0, weapon, graph, nodes, NODES, &camera);
}

static void expect(real x)
{
	CHECK(fabs(nodes[0].position.x - x) < 1e-4f && fabs(nodes[1].position.x - x) < 1e-4f,
		"drawn at %f and %f, not %f", nodes[0].position.x, nodes[1].position.x, x);
}

int main(int argc, char **argv)
{
	char const *case_name = argc > 1 ? argv[1] : "";

	CASE("authored hands")
	{
		struct unit_datum body={1,{_object_type_biped}};
		short mapping[2]={NONE,NONE};
		CHECK(first_person_weapon_select_hands(&body,GRAPH,1,mapping)==2 && mapping[0]==1,
			"authored body did not use the held weapon's matching hand node");
		strcpy(alternate_nodes[0].name,"unmatched");
		CHECK(first_person_weapon_select_hands(&body,GRAPH,1,mapping)==1 && mapping[0]==0,
			"incompatible arms did not rebuild the stock remap");
		alternate_hands.nodes.count=3;
		CHECK(first_person_weapon_select_hands(&body,GRAPH,1,mapping)==1,"oversized arms did not fall back");
		alternate_hands.nodes.count=0;
		CHECK(first_person_weapon_select_hands(&body,GRAPH,1,mapping)==1,"empty arms did not fall back");
		alternate_present=FALSE;
		CHECK(first_person_weapon_select_hands(&body,GRAPH,NONE,mapping)==NONE,"absent arms produced a model");
		return 0;
	}

	frame(10, 1.0f, WEAPON, GRAPH, 0.0f);
	/* a melee swings the weapon further in a tick than any limit that would
	tell it from another weapon: it is blended all the same */
	CASE("melee")
	{
		frame(11, 0.25f, WEAPON, GRAPH, 0.8f);
		expect(0.2f);
		frame(11, 0.75f, WEAPON, GRAPH, 0.8f);
		expect(0.6f);
		/* (from the tick's pose, not the last frame drawn) */
		frame(12, 0.5f, WEAPON, GRAPH, 1.6f);
		expect(1.2f);
	}
	/* another weapon of as many nodes: drawn as it is, then blended */
	CASE("weapon")
	{
		frame(11, 0.5f, OTHER_WEAPON, GRAPH, 0.1f);
		expect(0.1f);
		frame(12, 0.5f, OTHER_WEAPON, GRAPH, 0.2f);
		expect(0.15f);
	}
	CASE("graph")
	{
		frame(11, 0.5f, WEAPON, OTHER_GRAPH, 0.1f);
		expect(0.1f);
	}
	/* a weapon changed between two frames of a tick */
	CASE("within a tick")
	{
		frame(11, 0.25f, WEAPON, GRAPH, 0.4f);
		expect(0.1f);
		frame(11, 0.75f, OTHER_WEAPON, GRAPH, 0.2f);
		expect(0.2f);
		frame(12, 0.5f, OTHER_WEAPON, GRAPH, 0.4f);
		expect(0.3f);
	}
	/* the view away from first person for a while */
	CASE("gap")
	{
		frame(14, 0.5f, WEAPON, GRAPH, 0.4f);
		expect(0.4f);
	}
	CASE("not a number")
	{
		interpolation_tick = 11;
		interpolation_fraction = 0.5f;
		pose(0, 0.4f, 0.0f);
		pose(1, 0.4f, 0.0f);
		nodes[1].position.y = NAN;
		render_interpolation_first_person(0, WEAPON, GRAPH, nodes, NODES, &camera);
		CHECK(nodes[0].position.x == 0.4f, "a pose with a node not a number blended");
	}
	/* a node set back by half a turn snaps; the other still blends */
	CASE("half turn")
	{
		interpolation_tick = 11;
		interpolation_fraction = 0.5f;
		pose(0, 0.4f, 0.0f);
		pose(1, 0.0f, PI);
		render_interpolation_first_person(0, WEAPON, GRAPH, nodes, NODES, &camera);
		CHECK(fabs(nodes[0].position.x - 0.2f) < 1e-4f, "the other node not blended");
		CHECK(nodes[1].forward.i < -0.999f, "half a turn blended: forward (%f, %f)",
			nodes[1].forward.i, nodes[1].forward.j);
	}
	/* a large turn short of half a turn is blended */
	CASE("large turn")
	{
		interpolation_tick = 11;
		interpolation_fraction = 0.5f;
		pose(0, 0.0f, 0.0f);
		pose(1, 0.0f, 170.0f * PI / 180.0f);
		render_interpolation_first_person(0, WEAPON, GRAPH, nodes, NODES, &camera);
		CHECK(fabs(atan2(nodes[1].forward.j, nodes[1].forward.i) - 85.0f * PI / 180.0f) < 1e-3f,
			"170 degrees not blended halfway: forward (%f, %f)", nodes[1].forward.i, nodes[1].forward.j);
	}
	return 0;
}
