#include "harness.h"
#include <math.h>
#include <stddef.h>

#define HALO_PORT_MAXIMUM_NETWORK_PLAYERS 128
#define MAXIMUM_OBJECTS_PER_MAP 8192
#define MAXIMUM_MASS_POINTS_PER_PHYSICS 32
#define NUMBEROF(a) (sizeof(a)/sizeof(*(a)))
#define VALID_INDEX(index, count) (index>=0 && index<count)
#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))
#define PIN(x,a,b) MIN(MAX(x,a),b)
#define csmemcpy memcpy
#define csmemset memset
#define csmemcmp memcmp
#define SET_FLAG(value,bit,on) ((on)?((value)|=FLAG(bit)):((value)&=~FLAG(bit)))
#define _game_connection_network_server 2
enum { _object_dead_bit, _object_connected_to_map_bit, _object_outside_of_map_bit, _object_at_rest_bit, _object_on_ground_bit, _object_forge_phased_bit };
#include "unit_flags.inc"
enum { _object_type_scenery, _object_type_weapon, _object_type_equipment, _object_type_vehicle, _object_type_biped };
#define _object_mask_scenery FLAG(_object_type_scenery)
#define _object_mask_weapon FLAG(_object_type_weapon)
#define _object_mask_equipment FLAG(_object_type_equipment)
#define _object_mask_vehicle FLAG(_object_type_vehicle)
#define _new_object_never_automatically_delete_bit 0
enum { _collision_test_structure_bit, _collision_test_objects_bit, _collision_test_front_facing_surfaces_bit,
       _collision_test_back_facing_surfaces_bit };
#define _collision_test_objects_sight_blocking_flags 0
#define _object_mask_all 0xffffffffUL
#define _object_mask_projectile 4
struct scenario_object_datum {int unused;};
struct player_datum {long unit_index;};
struct location {long leaf_index; short cluster_index;};
struct test_object {
 long definition_index;
 struct {long parent_object_index,first_child_object_index,next_object_index,owner_object_index,owner_player_index; unsigned long flags,damage_flags; real_point3d position,bounding_sphere_center;
 real_vector3d forward,up,translational_velocity,angular_velocity; float bounding_sphere_radius; short variant_number;
 float base_change_colors[12];short name_index,type,owner_team_index;real scale;byte region_permutations[16];} object;
 struct {unsigned long flags; long driver_object_index,player_index;} unit;
 struct {byte airborne_ticks;} vehicle;
 struct {unsigned long flags;} item;
};
#define vehicle_datum test_object
#define unit_datum test_object
#define object_datum test_object
#define item_datum test_object
enum { _item_attached_to_unit_bit, _item_belongs_to_player_bit };
enum { _object_header_being_deleted_bit };
struct object_header_datum { unsigned long flags; };
struct object_placement_data {long definition_index; short variant_number; real_point3d position;
 real_vector3d forward,up; float change_colors[12];unsigned long flags;short owner_team_index;};
struct unit_definition {byte pad[0x2f4];short type;struct {struct {long index;} physics;float bounding_radius;} object;};
struct mass_point_definition {real_point3d position;float radius;};
struct tag_block {long count;void *address;};
struct physics_definition {struct tag_block mass_points;};
struct collision_model_instance {int unused;};
enum { _collision_result_structure, _collision_result_object };
#define _collision_test_for_line_of_sight_flags 0
#define _collision_test_environment_flags 0
#define _collision_test_objects_weapons_bit 8
#define _collision_test_objects_equipment_bit 9
#define _collision_test_objects_projectiles_bit 10
#define _collision_test_forge_phased_objects_bit 11
#define _collision_test_objects_all_types_flags 0xffffUL
struct collision_result {
 short type;
 long object_index;
 real_point3d point;
 struct { real_vector3d n; } plane;
};
static struct collision_result forge_hit;
static boolean forge_probe, forge_hit_found, forge_floor_contact;
struct network_game {int players[128];};
static struct network_game game;
static struct player_datum player;
static struct test_object objects[1024];
static long identifiers[1024];
static struct unit_definition definitions[8];
static struct mass_point_definition mass_point;
static struct physics_definition physics;
static struct scenario_object_datum sources[4];
static struct {long actual_count;} headers,*object_header_data=&headers;
static short global_structure_bsp_index;
static int enabled,connection,coop,percent,multiplier,blocked,sphere_blocked,floor_present,allocation_failed,nearby;
static long now,new_count,last_copy,placement_overlap,placement_phase_overlap=NONE;
static boolean collision_model_instance_new(struct collision_model_instance *p,long index) {(void)p;(void)index;return TRUE;}
static boolean collision_model_test_sphere(struct collision_model_instance const *p,real_point3d const *c,float r) {(void)p;(void)c;(void)r;return blocked;}

static char const *mode="per_player";
static char const *config_string(char const *name) {(void)name;return enabled?mode:"none";}
static long config_integer(char const *name) {return !strcmp(name,"network.coop_enemies")?percent:multiplier;}
static int game_connection(void) {return connection;}
static boolean network_coop_active(void) {return coop;}
static long game_time_get(void) {return now;}
static struct network_game *network_game_get_game(void) {return &game;}
static int network_player_is_valid(int const *p) {return *p;}
static struct test_object *get(long index) {int n=index&65535;return index!=NONE&&n<1024&&identifiers[n]==index?&objects[n]:NULL;}
#define vehicle_try_and_get get
#define vehicle_datum_try_and_get get
#define unit_try_and_get get
#define object_get get
static struct player_datum *player_try_and_get(long index) {return index==0?&player:NULL;}
static struct unit_definition *vehicle_definition_get(long index) {return &definitions[index];}
static struct physics_definition *physics_definition_get(long index) {(void)index;return &physics;}
#define TAG_BLOCK_GET_ELEMENT(b,i,t) (&((t *)(b)->address)[i])
static void object_placement_data_new(struct object_placement_data *p,long def,long owner) {
 (void)owner;memset(p,0,sizeof(*p));p->definition_index=def;
}
static boolean collision_test_vector(unsigned long f,real_point3d const *p,real_vector3d const *v,long ignored,struct collision_result *hit) {
 (void)f;(void)p;(void)v;(void)ignored;
 if (forge_probe) {
  *hit=forge_hit;
  return forge_hit_found;
 }
 hit->plane.n.k=1;return floor_present;
}
boolean collision_test_sphere(real_point3d const *p,float r,long ignored) {
 (void)ignored;
 return sphere_blocked || (forge_floor_contact && p->z - r < forge_hit.point.z);
}
static boolean clone_destination_outside;
static void scenario_location_from_point(struct location *l,real_point3d const *p)
{ (void)p;l->leaf_index=0;l->cluster_index=clone_destination_outside ? NONE : 0; }
static boolean game_in_progress(void) { return TRUE; }
#define valid_real isfinite
static boolean valid_real_point3d(real_point3d const *p) { return isfinite(p->x)&&isfinite(p->y)&&isfinite(p->z); }
static short objects_in_sphere(int f,unsigned long m,struct location const *l,real_point3d const *p,float r,long *indices,int max) {
 short count=0;
 (void)f;(void)m;(void)l;(void)p;(void)r;
 if(placement_overlap!=NONE&&count<max)indices[count++]=placement_overlap;
 if(placement_phase_overlap!=NONE&&count<max)indices[count++]=placement_phase_overlap;
 if((blocked||nearby)&&count<max)indices[count++]=identifiers[9];
 return count;
}
static void cross_product3d(real_vector3d const *a,real_vector3d const *b,real_vector3d *c) {
 c->i=a->j*b->k-a->k*b->j;c->j=a->k*b->i-a->i*b->k;c->k=a->i*b->j-a->j*b->i;
}
static long make(int slot,int definition) {
 struct test_object *v=&objects[slot];memset(v,0,sizeof(*v));
 identifiers[slot]=0x10000+slot;v->definition_index=definition;v->object.parent_object_index=NONE;
 v->object.first_child_object_index=NONE;v->object.next_object_index=NONE;v->object.owner_object_index=NONE;
 v->object.owner_player_index=NONE;v->object.name_index=NONE;
 v->object.flags=FLAG(_object_connected_to_map_bit);v->object.forward.i=1;v->object.up.k=1;
 v->unit.driver_object_index=NONE;v->unit.player_index=NONE;v->object.bounding_sphere_radius=.7f;return identifiers[slot];
}
static long object_new(struct object_placement_data const *p) {
 long index;
 if(allocation_failed)return NONE;
 index=make(20+new_count++,p->definition_index);last_copy=index;
 objects[index&65535].object.position=p->position;
 objects[index&65535].object.position.z+=definitions[p->definition_index].object.bounding_radius*.5f;
 return index;
}

static boolean terminal_command_running=TRUE,clone_refused,clone_xbox,clone_quota=TRUE,clone_objective;
static struct object_placement_data clone_placement;
#define object_try_and_get get
static struct object_header_datum clone_header;
static struct object_header_datum *object_header_try_and_get(long index) {
 return get(index) ? &clone_header : NULL;
}
static struct item_datum *item_try_and_get(long index) {
 struct test_object *object=get(index);
 return object && (object->object.type==_object_type_weapon || object->object.type==_object_type_equipment)
  ? object : NULL;
}
static void object_delete(long index) { identifiers[index&65535]=0; }
static boolean network_game_distributed_client(void){return connection==1;}
static boolean network_game_host_cheats_refused(void){return clone_refused;}
static void *global_network_game_server_get(void){return connection==2?&game:NULL;}
static boolean cheats_console_objects_xbox(void){return clone_xbox;}
static boolean cheats_console_object_allowed(void){return clone_quota;}
static boolean cheats_console_object_made(long index){return index!=NONE;}
static boolean weapon_is_flag(long index){return clone_objective;}
static void console_warning(char const *text, ...) { (void)text; }
static char const *tag_get_name(long index) { (void)index; return "fixture object"; }
static void objects_scripting_set_scale(long index,real scale,short ticks){get(index)->object.scale=scale;}
static long forge_viewer;
static long player_control_get_unit_index(short slot) { (void)slot; return forge_viewer; }
static void unit_get_camera_position(long index, real_point3d *p) { *p=get(index)->object.position; }
static void unit_get_aiming_vector(long index, real_vector3d *v) { *v=get(index)->object.forward; }
static void scale_vector3d(real_vector3d const *v, real scale, real_vector3d *out) {
 out->i=v->i*scale; out->j=v->j*scale; out->k=v->k*scale;
}
static void vector_from_points3d(real_point3d const *a, real_point3d const *b, real_vector3d *out) {
 out->i=b->x-a->x; out->j=b->y-a->y; out->k=b->z-a->z;
}
static void point_from_line3d(real_point3d const *p, real_vector3d const *v, real t, real_point3d *out) {
 out->x=p->x+v->i*t; out->y=p->y+v->j*t; out->z=p->z+v->k*t;
}
typedef struct { real_point3d position; real_vector3d forward, left, up; } real_matrix4x3;
struct physics_instance {
 struct physics_definition const *physics;
 real_matrix4x3 world_matrix;
};
static boolean physics_instance_new(struct physics_instance *instance, long index) {
 instance->physics=&physics;
 instance->world_matrix=(real_matrix4x3){0};
 instance->world_matrix.position=get(index)->object.position;
 return TRUE;
}
static short physics_mass_point_count(struct physics_definition const *definition) {
 return (short)definition->mass_points.count;
}
struct collision_feature_list { int unused; };
struct collision_plane { long object_index; };
static boolean collision_get_features_in_sphere(unsigned long flags, real_point3d const *center,
 real enclosing, real height, real radius, long ignore, struct collision_feature_list *features) {
 (void)flags; (void)center; (void)enclosing; (void)height; (void)radius; (void)features;
 return nearby || (placement_overlap!=NONE&&placement_overlap!=ignore);
}
static boolean collision_features_test_point(struct collision_feature_list const *features,
 real_point3d const *center, struct collision_plane *collision) {
 (void)features; (void)center;
 collision->object_index=identifiers[9];
 return TRUE;
}
#define matrix4x3_transform_point(matrix, point, out) ( \
 (out)->x=(matrix)->position.x+(point)->x, \
 (out)->y=(matrix)->position.y+(point)->y, \
 (out)->z=(matrix)->position.z+(point)->z )
static long forge_activated=NONE;
static real_vector3d zero_vector;
static real_vector3d const *global_zero_vector3d=&zero_vector;
static void object_set_position(long index,real_point3d const *p,real_vector3d const *f,real_vector3d const *u);
static void object_activate(long index) { forge_activated=index; }

/* Keep the existing allocation adapter, recording what native creation receives. */
static long clone_object_new(struct object_placement_data const *p){clone_placement=*p;return object_new(p);}
#define object_new clone_object_new
#include "clone.inc"
#undef object_new

#include "under_test.inc"

#include "rider_types.inc"
static struct {
 struct coop_riding_vehicle vehicles[MAXIMUM_RIDING_VEHICLES];short vehicle_count;
 struct coop_kept_rider riders[MAXIMUM_KEPT_RIDERS];short rider_count;
} coop_enemies;
struct squad_definition {short default_state,initial_state;};
struct encounter_definition {struct tag_block squads;};
static struct squad_definition rider_squads[2];
static struct encounter_definition rider_encounters[4];
static struct {struct tag_block ai_encounters;} rider_scenario;
static long rider_attempts,rider_successes,rider_statuses,rider_variants[4];
static short rider_offsets[4];
#define global_scenario_get() (&rider_scenario)
#define object_try_and_get_and_verify_type(index,mask) get(index)
#define _object_mask_unit 1
static boolean coop_enemies_host(void) {return connection==2&&coop;}
static void firefight_update(void) {}
static void coop_enemies_rider_place(struct coop_riding_vehicle const *vehicle,short number,real_point3d *p,real *facing) {
 *p=get(vehicle->vehicle_index)->object.position;p->x+=number;*facing=0;
}
static long actor_place(long variant,long encounter,long squad,struct actor_starting_location *location,boolean major,short initial) {
 rider_attempts++;
 CHECK(encounter==0x10003&&squad==1&&!major&&!initial,"queued rider lost native encounter binding");
 CHECK(location->initial_state==3&&location->default_state==4&&location->cluster_index==NONE&&
  location->command_list_index==NONE&&location->actor_variant_index==NONE&&location->noncombat_sequence_id==NONE,"queued rider lost native placement defaults");
 if(allocation_failed)return NONE;
 CHECK(rider_successes<4,"queued rider duplicated after release");
 rider_offsets[rider_successes]=(short)location->position.x;rider_variants[rider_successes]=variant;
 return rider_successes++;
}
static void encounter_update_status(long encounter) {(void)encounter;rider_statuses++;}
#include "rider_release.inc"

static long reset(void) {
 int i;long index;
 memset(objects,0,sizeof(objects));memset(identifiers,0,sizeof(identifiers));memset(&game,0,sizeof(game));
 memset(definitions,0,sizeof(definitions));
 for(i=0;i<8;i++){definitions[i].type=i;definitions[i].object.physics.index=0;definitions[i].object.bounding_radius=1;}
 enabled=coop=floor_present=1;connection=2;percent=50;multiplier=2;mode="per_player";blocked=sphere_blocked=allocation_failed=nearby=0;
 placement_overlap=NONE;headers.actual_count=1;global_structure_bsp_index=0;now=new_count=0;for(i=0;i<5;i++)game.players[i]=1;
 physics.mass_points.count=1;physics.mass_points.address=&mass_point;mass_point.radius=.2f;
 index=make(0,1);make(9,1);coop_vehicles_new_game();coop_vehicles_register(index,&sources[0]);return index;
}
static void settle(void) {int i;for(i=0;i<VEHICLE_SETTLE_TICKS;i++)coop_vehicles_update();}
static void drive(long index,float metres) {
 struct test_object *v=get(index);long driver=make(1,0);
 player.unit_index=driver;objects[1].unit.player_index=0;objects[1].object.parent_object_index=index;
 v->unit.driver_object_index=driver;coop_vehicles_update();v->object.position.x+=metres/3.048f;v->object.bounding_sphere_center.x+=metres/3.048f;coop_vehicles_update();
}
static void counts(void) {
 int p,x,m;reset();
 for(p=1;p<=128;p++)for(x=25;x<=200;x+=25)
  CHECK(vehicles_extra_count(p,x,0)==((p-1)*x*2)/300,"players %d percentage %d",p,x);
 CHECK(vehicles_extra_count(3,50,0)==0&&vehicles_extra_count(4,50,0)==1&&vehicles_extra_count(5,50,0)==1,"round down per vehicle");
 for(m=2;m<=32;m*=2)for(p=1;p<=128;p++)
  CHECK(vehicles_extra_count(p,50,m)==((m-1)*2)/3,"static vehicle count depends on players");
 mode="multiplier";multiplier=4;coop_vehicles_new_game();
 CHECK(vehicles_multiplier==4&&vehicles_enabled,"static config");
 mode="invalid";coop_vehicles_new_game();CHECK(!vehicles_enabled,"unknown mode enabled");
}
static void authority(void) {
 long index=reset();connection=1;coop_vehicles_new_game();coop_vehicles_register(index,&sources[0]);CHECK(!coop_vehicle_homes.count,"client registered");
 connection=2;enabled=0;coop_vehicles_new_game();coop_vehicles_register(index,&sources[0]);CHECK(!coop_vehicle_homes.count,"disabled registered");
 enabled=1;coop=0;coop_vehicles_new_game();coop_vehicles_register(index,&sources[0]);CHECK(!coop_vehicle_homes.count,"non-coop registered");
 coop=1;coop_vehicles_new_game();coop_vehicles_register(make(2,3),&sources[0]);coop_vehicles_register(make(3,6),&sources[1]);CHECK(!coop_vehicle_homes.count,"dropship/turret registered");
}
static void driving(void) {
 long index;int i;
 float radii[]={.2f,.7f,2.f,8.f};
 for(i=0;i<4;i++){
  float threshold=2*radii[i]*1.15f;
  index=reset();get(index)->object.bounding_sphere_radius=radii[i];settle();
  /* An offset model origin must not affect sphere separation. */
  get(index)->object.position.x=100;get(index)->object.bounding_sphere_center.x=100;coop_vehicles_update();CHECK(!new_count,"unoccupied movement copied");
  get(index)->object.position.x=0;get(index)->object.bounding_sphere_center.x=0;
  drive(index,(threshold-.01f)*3.048f);CHECK(!new_count,"before buffered clearance radius %f",radii[i]);
  get(index)->object.bounding_sphere_center.x=threshold+.01f;
  coop_vehicles_update();CHECK(new_count==1,"after buffered clearance radius %f",radii[i]);
 }
 reset();settle();identifiers[0]+=0x10000;coop_vehicles_update();CHECK(!new_count,"reused slot accepted");
}
static void delivery(void) {
 long index=reset();get(index)->object.parent_object_index=9;get(index)->object.position.z=100;settle();CHECK(!coop_vehicle_homes.homes[0].anchored,"cargo anchored");
 get(index)->object.parent_object_index=NONE;get(index)->object.position.z=.5f;floor_present=0;settle();CHECK(!coop_vehicle_homes.homes[0].anchored,"airborne anchored");
 floor_present=1;get(index)->vehicle.airborne_ticks=10;settle();CHECK(!coop_vehicle_homes.homes[0].anchored,"no ground contact");
 get(index)->vehicle.airborne_ticks=0;settle();CHECK(coop_vehicle_homes.homes[0].anchored&&coop_vehicle_homes.homes[0].placement.position.z==.5f,"unloaded anchor");
 drive(index,11);CHECK(new_count==1&&get(last_copy)->object.position.z==.5f,"copy not on ground pad");
}
static void chain(void) {
 long index=reset();game.players[5]=game.players[6]=game.players[7]=game.players[8]=1;settle();drive(index,11);CHECK(new_count==1,"first copy");
 drive(last_copy,11);CHECK(new_count==2,"second copy");drive(last_copy,11);CHECK(new_count==2,"budget amplified");
 index=reset();mode="multiplier";multiplier=4;coop_vehicles_new_game();coop_vehicles_register(index,&sources[0]);
 memset(&game,0,sizeof(game));game.players[0]=1;settle();
 drive(index,11);drive(last_copy,11);CHECK(new_count==2,"two-thirds static copies missing");
 drive(last_copy,11);CHECK(new_count==2,"static budget amplified");
}
static void blocked_test(void) {
 long index=reset();settle();blocked=1;drive(index,11);CHECK(!new_count,"occupied pad");
 blocked=0;sphere_blocked=1;coop_vehicles_update();CHECK(!new_count,"wall");
 sphere_blocked=0;global_structure_bsp_index=1;coop_vehicles_update();CHECK(!new_count,"wrong BSP");
 global_structure_bsp_index=0;headers.actual_count=MAXIMUM_OBJECTS_PER_MAP-512;coop_vehicles_update();CHECK(!new_count,"capacity");
 headers.actual_count=1;allocation_failed=1;coop_vehicles_update();CHECK(!new_count&&!coop_vehicle_homes.homes[0].copies,"failed allocation consumed budget");
 allocation_failed=0;nearby=1;coop_vehicles_update();CHECK(new_count==1,"unblocked pad never retried");
}
static void checkpoint(void) {
 long index=reset();settle();now=102;coop_vehicles_before_save();drive(index,11);CHECK(coop_vehicle_homes.homes[0].copies==1,"copy");
 coop_vehicles_after_load();CHECK(!coop_vehicle_homes.homes[0].copies&&coop_vehicle_homes.homes[0].current==index,"budget not restored");
 coop_vehicle_homes.homes[0].copies=1;coop_vehicles_before_save();coop_vehicle_homes.homes[0].copies=9;coop_vehicles_after_load();CHECK(coop_vehicle_homes.homes[0].copies==1,"consumed budget not restored");
 now=103;coop_vehicles_after_load();CHECK(!coop_vehicle_homes.count,"unrelated core retained handles");
}
static void recreated(void) {
 long index=reset();settle();drive(index,11);coop_vehicles_register(make(2,1),&sources[0]);CHECK(coop_vehicle_homes.count==1&&coop_vehicle_homes.homes[0].copies==1,"script recreation reset budget");
 identifiers[last_copy&65535]=0;coop_vehicles_register(make(3,1),&sources[0]);CHECK(coop_vehicle_homes.count==1&&coop_vehicle_homes.homes[0].copies==1,"destroyed copy reset budget");
}
static void rider_release(char const *name) {
 long ship=reset(),other;short i;
 memset(&coop_enemies,0,sizeof(coop_enemies));rider_attempts=rider_successes=rider_statuses=0;
 rider_scenario.ai_encounters.address=rider_encounters;rider_scenario.ai_encounters.count=4;
 rider_encounters[3].squads.address=rider_squads;rider_encounters[3].squads.count=2;
 rider_squads[1].default_state=4;rider_squads[1].initial_state=3;
 coop_enemies.vehicle_count=1;coop_enemies.vehicles[0].vehicle_index=ship;
 coop_enemies.vehicles[0].seated_count=1;coop_enemies.vehicles[0].seated_unit_indices[0]=make(2,0);
 coop_enemies.rider_count=2;
 for(i=0;i<2;i++) {coop_enemies.riders[i].vehicle_index=ship;coop_enemies.riders[i].encounter_index=0x10003;
  coop_enemies.riders[i].squad_index=1;coop_enemies.riders[i].variant_definition_index=5+i;}
 if(!strcmp(name,"riders-seated")) {
  objects[2].object.parent_object_index=ship;coop_enemies_update();CHECK(!rider_attempts,"released before any passenger disembarked");
  objects[2].object.parent_object_index=NONE;objects[2].object.damage_flags=FLAG(_object_dead_bit);
  coop_enemies_update();CHECK(!rider_attempts,"dead seated rider started a release");
  objects[2].object.damage_flags=0;coop_enemies_update();CHECK(rider_successes==1,"living disembarkation did not release reinforcement");return;
 }
 if(!strcmp(name,"riders-authority")) {
  connection=1;coop_enemies_update();connection=2;coop=0;coop_enemies_update();
  CHECK(!rider_attempts&&coop_enemies.rider_count==2,"non-host changed queued reinforcements");return;
 }
 if(!strcmp(name,"riders-recycled")) {
  identifiers[0]+=0x10000;coop_enemies_update();
  CHECK(!rider_attempts&&!coop_enemies.rider_count&&!coop_enemies.vehicle_count,"recycled craft inherited its old riders");return;
 }
 if(!strcmp(name,"riders-destroyed")) {
  other=make(3,1);coop_enemies.vehicles[1]=coop_enemies.vehicles[0];coop_enemies.vehicles[1].vehicle_index=other;
  coop_enemies.vehicle_count=2;coop_enemies.riders[1].vehicle_index=other;
  objects[0].object.damage_flags=FLAG(_object_dead_bit);allocation_failed=1;coop_enemies_update();
  CHECK(coop_enemies.rider_count==1&&coop_enemies.riders[0].vehicle_index==other&&coop_enemies.vehicle_count==1,"destroyed craft consumed another craft's queued rider");
  allocation_failed=0;now=RIDER_RELEASE_TICKS;coop_enemies_update();CHECK(rider_successes==1&&!coop_enemies.rider_count,"surviving craft lost its reinforcement");return;
 }
 allocation_failed=1;
 for(now=0;now<3*RIDER_RELEASE_TICKS;now++)coop_enemies_update();
 CHECK(rider_attempts==3&&!rider_successes&&coop_enemies.rider_count==2&&!coop_enemies.vehicles[0].released_count,"native allocation failure lost a queued passenger or release position");
 allocation_failed=0;coop_enemies_update();
 CHECK(rider_successes==1&&coop_enemies.rider_count==1&&coop_enemies.vehicles[0].released_count==1,"available native capacity did not release one passenger");
 coop_enemies_update();CHECK(rider_successes==1,"retry bypassed the bounded release interval");
 now+=RIDER_RELEASE_TICKS;coop_enemies_update();
 CHECK(rider_successes==2&&!coop_enemies.rider_count&&rider_variants[0]==5&&rider_variants[1]==6&&rider_offsets[0]==0&&rider_offsets[1]==1,"retry duplicated, changed or skipped an authored reinforcement");
 now+=RIDER_RELEASE_TICKS;coop_enemies_update();CHECK(rider_successes==2&&!coop_enemies.vehicle_count,"completed release did not retire its craft");
}
/* The existing native unsteered path, with object IO/rotation as boundaries. */
#define MAXIMUM_TRACKED_OBJECTS 1024
#define _network_vehicle_write_state 0
#define _network_vehicle_write_carry 1
static boolean path_cinematic;
static long path_vehicle;
struct object_iterator {long index;boolean done;};
static void object_iterator_new(struct object_iterator *it,unsigned long mask,long flags) {it->done=FALSE;}
static struct test_object *object_iterator_next(struct object_iterator *it)
{if(it->done)return NULL;it->done=TRUE;it->index=path_vehicle;return get(path_vehicle);}
static boolean cinematic_in_progress(void) {return path_cinematic;}
static boolean distributed_vehicle_unsteered(long index) {return get(index)->unit.driver_object_index==NONE;}
static void object_set_position(long index,real_point3d const *p,real_vector3d const *f,real_vector3d const *u)
{
 struct test_object *object=get(index);
 if(p)object->object.position=*p;
 if(f)object->object.forward=*f;
 if(u)object->object.up=*u;
}
static void network_objects_vehicle_written(long i,real_point3d const *p,real_vector3d const *f,real_vector3d const *u,short kind) {}
static boolean distributed_transform_valid(real_point3d const *p,real_vector3d const *f,real_vector3d const *u,
 real_vector3d const *v,real_vector3d const *a,real_vector3d *vf,real_vector3d *vu)
{*vf=*f;*vu=*u;return isfinite(p->x)&&isfinite(p->y)&&isfinite(p->z);}
static void unsteered_blend_facing(real_vector3d const *f0,real_vector3d const *u0,real_vector3d const *f1,
 real_vector3d const *u1,real t,real_vector3d *f,real_vector3d *u) {*f=*f1;*u=*u1;}
#include "path.inc"
#define MAXIMUM_REGIONS_PER_OBJECT 16
struct distributed_message_header {int unused;};
#define _distributed_message_coop_object_looks 1
static struct {boolean refresh;} host_resend;
static int appearance_sends;
static void send_to_clients(void const *message,int kind,short count,unsigned short size);
static struct unit_datum *unit_get(long index) { return get(index); }
#include "looks.inc"
static struct distributed_coop_object_look sent_look;
static void send_to_clients(void const *message,int kind,short count,unsigned short size) {
 CHECK(kind==_distributed_message_coop_object_looks&&count==1&&size==sizeof(sent_look),"wrong native look envelope");
 sent_look=((struct distributed_coop_object_looks_message const *)message)->looks[0];appearance_sends++;
}

static void initial_appearance(void) {
 path_vehicle=reset();get(path_vehicle)->object.type=_object_type_vehicle;
 get(path_vehicle)->object.scale=.3f;get(path_vehicle)->object.region_permutations[0]=2;
 CHECK(offsetof(struct distributed_coop_object_look,boarding)==2&&
       offsetof(struct distributed_coop_object_look,object_index)==4,"boarding state changed wire offsets");
 host_send_object_looks();
 CHECK(appearance_sends==1&&sent_look.object_index==path_vehicle&&sent_look.scale==.3f&&
       sent_look.region_permutations[0]==2,"scripted first appearance was not sent");
 host_send_object_looks();CHECK(appearance_sends==1,"unchanged look sent every tick");
 host_resend.refresh=TRUE;host_send_object_looks();CHECK(appearance_sends==2,"late object/peer cannot recover initial appearance");
 host_resend.refresh=FALSE;get(path_vehicle)->object.scale=1;host_send_object_looks();
 CHECK(appearance_sends==3&&sent_look.scale==1,"return to ordinary scale lost");
 identifiers[path_vehicle&65535]+=65536;path_vehicle+=65536;
 get(path_vehicle)->object.scale=.5f;host_send_object_looks();
 CHECK(appearance_sends==4&&sent_look.object_index==path_vehicle&&sent_look.scale==.5f,"reused slot lost fresh appearance");
 CHECK(sent_look.boarding==_coop_object_boarding_open&&sent_look.pad==0,"initial vehicle boarding omitted");
 get(path_vehicle)->unit.flags|=FLAG(_unit_not_enterable_by_player_bit);
 host_send_object_looks();
 CHECK(appearance_sends==5&&sent_look.boarding==_coop_object_boarding_locked,"vehicle lock did not send");
 host_resend.refresh=TRUE;host_send_object_looks();host_resend.refresh=FALSE;
 CHECK(appearance_sends==6&&sent_look.boarding==_coop_object_boarding_locked,"vehicle lock did not refresh");
 get(path_vehicle)->unit.flags&=~FLAG(_unit_not_enterable_by_player_bit);
 host_send_object_looks();
 CHECK(appearance_sends==7&&sent_look.boarding==_coop_object_boarding_open,"vehicle unlock did not send");
}

static void unsteered_path(char const *name) {
 real_point3d sample;real_vector3d f={1,0,0},u={0,0,1};
 real previous=0;long host_tick=0;
 path_vehicle=reset();path_cinematic=strcmp(name,"path-gameplay")!=0;
 coop=strcmp(name,"path-noncoop")!=0;
 for(now=100;now<140;now++) {
  if(now%4==0) {
   host_tick=now-2;sample=(real_point3d){host_tick*.7f,0,0};
   distributed_client_note_unsteered_path(path_vehicle,&sample,&f,&u,host_tick,FALSE);
  }
  distributed_client_carry_unsteered_vehicles();
  if(now>=108) {
   real x=get(path_vehicle)->object.position.x;
   if(path_cinematic&&coop)CHECK(fabsf(x-now*.7f)<.001f,"cinematic ship trails host timeline at %ld: %f",now,x);
   else CHECK(x<now*.7f-1.f&&x>=previous,"gameplay glide changed or moved backwards");
   previous=x;
  }
 }
 if(!strcmp(name,"path-stale")) {
  now=host_tick+40;distributed_client_carry_unsteered_vehicles();
  CHECK(fabsf(get(path_vehicle)->object.position.x-sample.x)<.001f,"stale path extrapolates forever");
  CHECK(get(path_vehicle)->object.translational_velocity.i==0,"stale path keeps moving");
 }
 if(!strcmp(name,"path-stop")) {
  sample.x=100;distributed_client_note_unsteered_path(path_vehicle,&sample,&f,&u,now-1,TRUE);
  distributed_client_carry_unsteered_vehicles();
  CHECK(fabsf(get(path_vehicle)->object.position.x-100)<.001f,"stopped ship extrapolated");
 }
 if(!strcmp(name,"path-teleport")) {
  sample.x=1000;distributed_client_note_unsteered_path(path_vehicle,&sample,&f,&u,now-1,FALSE);
  distributed_client_carry_unsteered_vehicles();
  CHECK(get(path_vehicle)->object.position.x==1000,"teleport converted into path speed");
 }
}
int main(int argc,char **argv) {
 if(argc!=2)return 2;
 if(!strncmp(argv[1],"clone-",6)) {
  long source=reset(),copy;char const *name=argv[1]+6;boolean refused=FALSE;
  struct test_object *v=get(source);
  v->object.type=_object_type_vehicle;v->object.scale=.5f;v->object.owner_team_index=2;
  v->object.variant_number=3;v->object.base_change_colors[0]=.25f;v->object.name_index=7;
  if(!strcmp(name,"local")){connection=0;coop=0;}
  if(!strcmp(name,"default")){coop=0;v->object.scale=0;}
  if(!strcmp(name,"identity")){coop=0;v->object.scale=1;}
  if(!strcmp(name,"ordinary")){coop=0;refused=TRUE;}
  if(!strcmp(name,"xbox")){clone_xbox=TRUE;refused=TRUE;}
  if(!strcmp(name,"client")){connection=1;refused=TRUE;}
  if(!strcmp(name,"permission")){clone_refused=TRUE;refused=TRUE;}
  if(!strcmp(name,"quota")){clone_quota=FALSE;refused=TRUE;}
  if(!strcmp(name,"scenery")){v->object.type=_object_type_scenery;refused=TRUE;}
  if(!strcmp(name,"attached")){v->object.parent_object_index=123;refused=TRUE;}
  if(!strcmp(name,"objective")){v->object.type=_object_type_weapon;clone_objective=TRUE;refused=TRUE;}
  if(!strcmp(name,"allocation")){allocation_failed=TRUE;refused=TRUE;}
  if(!strcmp(name,"bounds")) {
   real invalid[]={-1,100,INFINITY,NAN};int i;
   for(i=0;i<4;i++){v->object.scale=invalid[i];CHECK(cheat_forge_clone(source)==NONE&&!new_count,"unsupported scale created a replica");}
   return 0;
  }
  if(!strncmp(name,"position",8)) {
   real_point3d destination={12.5f,-3.25f,4.0f};
   real_point3d original=v->object.position;
   if(!strcmp(name,"position-outside")){clone_destination_outside=TRUE;refused=TRUE;}
   if(!strcmp(name,"position-nan")){destination.z=NAN;refused=TRUE;}
   if(!strcmp(name,"position-client")){connection=1;refused=TRUE;}
   copy=cheat_forge_clone_at(source,&destination);
   if(!refused) {
    CHECK(clone_placement.position.x==destination.x&&clone_placement.position.y==destination.y&&
      clone_placement.position.z==destination.z,"native creation missed requested coordinates");
    CHECK(v->object.position.x==original.x&&v->object.position.y==original.y&&
      v->object.position.z==original.z,"positioned clone moved its source");
   }
  } else {
   copy=cheat_forge_clone(source);
  }
  if(refused){CHECK(copy==NONE&&!new_count,"refused clone created an object");return 0;}
  CHECK(copy!=NONE&&copy!=source&&new_count==1&&get(copy)->object.scale==v->object.scale,"native scaled clone missing");
  CHECK(clone_placement.owner_team_index==2&&clone_placement.variant_number==3&&
   clone_placement.change_colors[0]==.25f&&clone_placement.forward.i==1&&clone_placement.up.k==1,
   "native clone lost authored placement");
  CHECK(clone_placement.flags&FLAG(_new_object_never_automatically_delete_bit),"clone became automatic cleanup loot");
  return 0;
 }
 if (!strncmp(argv[1], "place-", 6)) {
  char const *name=argv[1]+6;
  long source=reset(),copy;
  struct test_object *object=get(source);
  forge_viewer=make(8,1);
  object->object.type=_object_type_vehicle;
  object->object.position.z=1.0f;
  object->object.bounding_sphere_center.z=1.25f;
  object->object.bounding_sphere_radius=7.0f;
  physics.mass_points.count=1;
  physics.mass_points.address=&mass_point;
  mass_point.position.z=-.35f;
  mass_point.radius=.25f;
  forge_probe=forge_hit_found=TRUE;
  forge_hit.type=_collision_result_object;
  forge_hit.object_index=source;
  CHECK(cheat_forge_place()==NONE,"placed without selection");
  CHECK(cheat_forge_pick(),"visible native object could not be picked");
  forge_hit.type=_collision_result_structure;
  forge_hit.point.x=12.0f;
  forge_hit.point.y=-2.0f;
  forge_hit.plane.n.k=1.0f;
  if (!strcmp(name,"empty")) forge_hit_found=FALSE;
  if (!strcmp(name,"wall")) forge_hit.plane.n.k=0.0f;
  if (!strcmp(name,"object")) forge_hit.type=_collision_result_object;
  if (!strcmp(name,"occupied")) nearby=TRUE;
  if (!strcmp(name,"obstructed")) sphere_blocked=TRUE;
  if (!strcmp(name,"outside")) clone_destination_outside=TRUE;
  if (!strcmp(name,"stale")) identifiers[source&65535]+=0x10000;
  if (!strcmp(name,"restore")) cheats_forge_reset();
  if (!strcmp(name,"client")) connection=1;
  copy=cheat_forge_place();
  if (!strcmp(name,"clear")) {
   CHECK(copy!=NONE&&new_count==1,"visible clear floor did not accept copy");
   CHECK(clone_placement.position.x==12.0f&&clone_placement.position.y==-2.0f&&
    fabsf(clone_placement.position.z-.66f)<.0001f,"vehicle used model radius instead of its floor contact");
   CHECK(object->object.position.z==1.0f&&cheat_forge_undo()&&get(source)&&!get(copy),
    "pick/place/undo changed the original or left the copy");
  } else {
   CHECK(copy==NONE&&!new_count,"invalid aim placement created an object");
  }
  return 0;
 }
 if(!strncmp(argv[1],"move-",5)) {
  char const *name=argv[1]+5;
  long source=reset(),copy,selected;
  struct test_object *object;
  boolean scenery=!strncmp(name,"scenery-",8);
  boolean undo=!strcmp(name,"undo");
  if(scenery)connection=0;
  forge_viewer=make(8,1);
  get(source)->object.type=scenery?_object_type_scenery:_object_type_vehicle;
  copy=cheat_forge_clone(source);
  CHECK(copy!=NONE,"missing native copy");
  object=get(copy);
  object->object.type=get(source)->object.type;
  object->object.forward.i=1.f;
  object->object.up.k=1.f;
  object->object.translational_velocity.i=1.f;
  object->object.angular_velocity.k=1.f;
  object->object.flags|=FLAG(_object_at_rest_bit);
  physics.mass_points.count=1;
  physics.mass_points.address=&mass_point;
  mass_point.position.z=-.35f;
  mass_point.radius=.25f;
  selected=!strcmp(name,"original")?source:copy;
  forge_probe=forge_hit_found=TRUE;
  forge_hit.type=_collision_result_object;
  forge_hit.object_index=selected;
  CHECK(cheat_forge_pick(),"could not aim at the native Forge target");
  forge_hit.type=_collision_result_structure;
  forge_hit.point.x=12.f;
  forge_hit.point.y=-2.f;
  forge_hit.plane.n.k=1.f;
  if(strstr(name,"self")||!strcmp(name,"scenery-blocked"))placement_overlap=copy;
  if(strstr(name,"phased")) {
   placement_overlap=make(10,1);
   placement_phase_overlap=make(11,1);
   get(placement_overlap)->object.flags|=FLAG(_object_forge_phased_bit);
   get(placement_phase_overlap)->object.flags|=FLAG(_object_forge_phased_bit);
  }
  if(strstr(name,"blocked"))nearby=TRUE;
  if(!strcmp(name,"occupied"))object->object.first_child_object_index=123;
  if(!strcmp(name,"client"))connection=1;
  if(!strcmp(name,"clear")||strstr(name,"self")||!strcmp(name,"scenery-phased")||undo) {
   real_point3d old_position=object->object.position;
   float height=scenery?1.26f:.66f;
   CHECK(cheat_forge_move_selected()&&forge_activated==copy&&get(copy)==object,"copy was not moved in place");
   CHECK(object->object.position.x==12.f&&object->object.position.y==-2.f&&
    fabsf(object->object.position.z-height)<.0001f&&get(source)->object.position.x!=12.f,"placement did not use the native floor contact");
   CHECK(object->object.forward.i==1.f&&object->object.up.k==1.f&&
    !object->object.translational_velocity.i&&!object->object.angular_velocity.k&&
    !TEST_FLAG(object->object.flags,_object_at_rest_bit),"move discarded the pose or kept stale physics");
   if(undo) {
    object->object.forward.i=0.f;
    object->object.forward.j=1.f;
    nearby=TRUE;
    CHECK(!cheat_forge_undo_move()&&object->object.position.x==12.f,"undo overlapped an occupant");
    nearby=FALSE;
    global_structure_bsp_index=1;
    CHECK(!cheat_forge_undo_move(),"undo crossed BSPs");
    global_structure_bsp_index=0;
    connection=1;
    CHECK(!cheat_forge_undo_move(),"client undid an authoritative move");
    connection=0;
    blocked=TRUE; /* Only the broad bounds overlap. The native hull is clear. */
    CHECK(cheat_forge_undo_move()&&get(copy)==object&&
     object->object.position.x==old_position.x&&object->object.position.y==old_position.y&&
     object->object.position.z==old_position.z&&object->object.forward.i==1.f&&object->object.forward.j==0.f,
     "undo did not restore the same copy's previous pose");
    CHECK(!cheat_forge_undo_move(),"undo reused a consumed snapshot");
    nearby=TRUE;
    CHECK(!cheat_forge_redo_move()&&object->object.position.x==old_position.x,"redo overlapped an occupant");
    nearby=FALSE;
    connection=1;
    CHECK(!cheat_forge_redo_move(),"client redid an authoritative move");
    connection=0;
    CHECK(cheat_forge_redo_move()&&get(copy)==object&&object->object.position.x==12.f&&
     object->object.forward.i==1.f&&object->object.forward.j==0.f,"redo restored a drifted pose instead of the edit");
    CHECK(!cheat_forge_redo_move(),"redo reapplied an already current edit");
    CHECK(cheat_forge_undo_move(),"redone move could not be undone again");
    forge_hit.point.x=14.f;
    CHECK(cheat_forge_move_selected()&&!cheat_forge_redo_move(),"new move retained an obsolete redo");
   }
  } else {
   CHECK(!cheat_forge_move_selected()&&forge_activated==NONE&&object->object.position.x!=12.f&&
    get(source)->object.position.x!=12.f,"unsafe Forge target was moved");
  }
  CHECK(new_count==1&&get(source)&&get(copy),"move recreated or removed an existing object");
  return 0;
 }
 if(!strncmp(argv[1],"rotate-",7)) {
  boolean scenery=!strcmp(argv[1]+7,"scenery");
  boolean grounded=!strcmp(argv[1]+7,"grounded");
  long source=reset(),copy;
  struct test_object *object;
  real_point3d position;
  connection=0;
  forge_viewer=make(8,1);
  get(source)->object.type=scenery?_object_type_scenery:_object_type_vehicle;
  copy=cheat_forge_clone(source);
  CHECK(copy!=NONE,"missing native copy");
  object=get(copy);
  object->object.type=get(source)->object.type;
  object->object.forward=(real_vector3d){1.f,0.f,0.f};
  object->object.up=(real_vector3d){0.f,0.f,1.f};
  position=object->object.position;
  physics.mass_points.count=1;
  physics.mass_points.address=&mass_point;
  mass_point.radius=.25f;
  forge_selection=copy;
  placement_overlap=copy;
  nearby=TRUE;
  CHECK(!cheat_forge_rotate_selected()&&forge_activated==NONE&&object->object.forward.i==1.f,
   "rotation overlapped an occupant");
  nearby=FALSE;
  connection=1;
  CHECK(!cheat_forge_rotate_selected(),"client rotated a host copy");
  connection=0;
  if(grounded) {
   object->object.flags|=FLAG(_object_on_ground_bit);
   object->object.bounding_sphere_radius=1.f;
   forge_probe=forge_hit_found=forge_floor_contact=TRUE;
   forge_hit.type=_collision_result_structure;
   forge_hit.point=position;
   forge_hit.point.z-=.05f;
   forge_hit.plane.n.k=1.f;
  }
  CHECK(cheat_forge_rotate_selected()&&get(copy)==object&&forge_activated==copy,
   "rotation replaced or failed to activate the selected copy");
  CHECK(object->object.forward.i==0.f&&object->object.forward.j==1.f&&object->object.up.k==1.f&&
   object->object.position.x==position.x&&object->object.position.y==position.y&&
   fabsf(object->object.position.z-position.z-(grounded?.26f:0.f))<.0001f,
   "rotation failed to preserve the pivot or apply hull clearance");
  CHECK(!grounded||object->object.position.z-mass_point.radius>forge_hit.point.z,
   "rotated hull intersects the support plane");
  CHECK(cheat_forge_undo_move()&&object->object.forward.i==1.f&&object->object.forward.j==0.f,
   "undo did not restore the previous orientation");
  CHECK(cheat_forge_redo_move()&&object->object.forward.i==0.f&&object->object.forward.j==1.f&&
   fabsf(object->object.position.z-position.z-(grounded?.26f:0.f))<.0001f,
   "redo did not restore the same turned hull and clearance");
  CHECK(new_count==1&&get(source)&&get(copy),"rotation changed object count");
  return 0;
 }
 if(!strncmp(argv[1],"remove-",7)) {
  char const *name=argv[1]+7;
  long source=reset(),first,last,selected;
  get(source)->object.type=_object_type_vehicle;
  first=cheat_forge_clone(source);
  last=cheat_forge_clone(source);
  CHECK(first!=NONE&&last!=NONE&&first!=last,"missing two native copies");
  selected=!strcmp(name,"original")?source:first;
  forge_viewer=make(8,1);
  forge_probe=forge_hit_found=TRUE;
  forge_hit.type=_collision_result_object;
  forge_hit.object_index=selected;
  CHECK(cheat_forge_pick(),"could not select the target through native aim");
  if(!strcmp(name,"occupied")) get(first)->object.first_child_object_index=123;
  if(!strcmp(name,"stale")) identifiers[first&65535]+=0x10000;
  if(!strcmp(name,"client")) connection=1;
  if(!strcmp(name,"permission")) clone_refused=TRUE;
  if(!strcmp(name,"previous")) {
   CHECK(cheat_forge_remove_selected()&&!get(first)&&!cheat_forge_remove_selected(),
    "selected removal did not delete exactly the earlier copy");
   CHECK(get(last)&&cheat_forge_undo()&&!get(last),"selected removal erased the separate latest-copy undo");
  } else {
   CHECK(!cheat_forge_remove_selected()&&identifiers[selected&65535]&&get(last),"unsafe selected removal deleted an object");
  }
  CHECK(get(source),"selected removal touched the original");
  return 0;
 }
 if(!strncmp(argv[1],"undo-",5)) {
  char const *name=argv[1]+5;
  long source=reset(),first,copy;
  struct test_object *placed;
  get(source)->object.type=_object_type_vehicle;
  first=cheat_forge_clone(source);
  copy=cheat_forge_clone(source);
  CHECK(first!=NONE&&copy!=NONE&&first!=copy,"missing two native copies");
  placed=get(copy);
  placed->object.type=_object_type_vehicle;
  if(!strcmp(name,"occupied")) placed->object.first_child_object_index=123;
  if(!strcmp(name,"mounted") || !strcmp(name,"rider") || strstr(name,"-weapon")) {
   long attachment=make(31,1);
   struct test_object *child=get(attachment);

   placed->object.first_child_object_index=attachment;
   child->object.parent_object_index=copy;
   child->object.owner_object_index=!strcmp(name,"foreign-weapon")?first:copy;
   child->object.type=!strcmp(name,"rider")?_object_type_biped:_object_type_weapon;
   child->item.flags=FLAG(_item_attached_to_unit_bit);
   if(!strcmp(name,"owned-weapon")) child->item.flags|=FLAG(_item_belongs_to_player_bit);
   if(!strcmp(name,"objective-weapon")) clone_objective=TRUE;
  }
  if(!strcmp(name,"held")) {
   placed->object.type=_object_type_weapon;
   placed->item.flags=FLAG(_item_belongs_to_player_bit);
  }
  if(!strcmp(name,"claimed")) placed->object.name_index=7;
  if(!strcmp(name,"stale")) identifiers[copy&65535]+=0x10000;
  if(!strcmp(name,"pending-delete")) clone_header.flags=FLAG(_object_header_being_deleted_bit);
  if(!strcmp(name,"restore")) cheats_forge_reset();
  if(!strcmp(name,"client")) connection=1;
  if(!strcmp(name,"permission")) clone_refused=TRUE;
  if(!strcmp(name,"latest")||!strcmp(name,"mounted")) {
   CHECK(cheat_forge_undo()&&!get(copy),"undo did not delete last copy");
   CHECK(!cheat_forge_undo(),"undo reused deleted copy");
  } else {
   CHECK(!cheat_forge_undo()&&identifiers[copy&65535],"unsafe undo deleted a live object");
  }
  CHECK(get(first)&&get(source),"undo deleted the previous copy or original");
  return 0;
 }
 if(!strncmp(argv[1],"riders-",7)) {rider_release(argv[1]);return 0;}
 if(!strncmp(argv[1],"path-",5)) {unsteered_path(argv[1]);return 0;}
 if(!strcmp(argv[1],"initial-appearance")) {initial_appearance();return 0;}
 if(!strcmp(argv[1],"counts"))counts();else if(!strcmp(argv[1],"authority"))authority();
 else if(!strcmp(argv[1],"driving"))driving();else if(!strcmp(argv[1],"delivery"))delivery();
 else if(!strcmp(argv[1],"chain"))chain();else if(!strcmp(argv[1],"blocked"))blocked_test();
 else if(!strcmp(argv[1],"checkpoint"))checkpoint();else if(!strcmp(argv[1],"recreated"))recreated();else return 2;
 return 0;
}
