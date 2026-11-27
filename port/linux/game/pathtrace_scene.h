#ifndef FORERUNNER_PATHTRACE_SCENE_H
#define FORERUNNER_PATHTRACE_SCENE_H

struct render_camera;
struct structure_bsp;
struct structure_surface;

/* Borrowed for a single visit. Upload or copy before returning to the engine. */
struct pathtrace_material
{
	const struct structure_surface *surfaces;
	const unsigned char *vertices;
	const unsigned char *lightmap_vertices;
	long vertex_count, surface_count;
	long shader, lightmap_group;
	short permutation, lightmap_bitmap;
};

struct pathtrace_vertex
{
	float position[3], normal[3], texcoord[2], lightmap_texcoord[2];
};

typedef int (*pathtrace_material_sink)(void *context, const struct pathtrace_material *material);

/* Discard pending uploads on failure. A visit can stop after earlier materials
 * have been delivered, for example when a later section exceeds the budget. */
int pathtrace_scene_visit(const struct structure_bsp *structure, pathtrace_material_sink sink, void *context);
void pathtrace_scene_read_vertex(const struct pathtrace_material *material, long index, struct pathtrace_vertex *vertex);

/* Private, one-shot capture from an actual render view and its loaded BSP. */
void pathtrace_scene_capture(const struct structure_bsp *structure, short bsp,
	const struct render_camera *camera);

#endif
