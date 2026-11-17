/* Native controller mapping and PTT ownership, with located XR actions supplied
 * by a fixture. This exercises no loader, headset, microphone or renderer. */
#include "harness.h"
#include <math.h>
#include "types.inc"
typedef int BOOL;
typedef short SHORT;
typedef unsigned char BYTE;
typedef unsigned long DWORD;
static struct vr_openxr_frame input;
static BOOL connected = TRUE;
static char const *talk = "off", *voice = "push_to_talk";
static char const *aim_mode="controller",*dominant="right",*swap_sticks="auto",*turn_mode="snap";
static BOOL openxr_layout_known,openxr_layout_wait;
static int openxr_layout_weapon,openxr_layout_turn,openxr_layout_grab,openxr_layout_support;
static BOOL grab_enabled,support_enabled;
static int config_boolean(char const *name){if(!strcmp(name,"vr.two_handed"))return support_enabled;CHECK(!strcmp(name,"vr.weapon_grab"),"unknown boolean %s",name);return grab_enabled;}
static BOOL openxr_talk_armed, openxr_talk_held;
static int openxr_talk_binding, mouse_lock;
static unsigned int openxr_talk_reference;
static unsigned long pad_used_ms;
static unsigned long SDL_GetTicks(void) { return 100; }
static void pthread_mutex_lock(int *lock) {}
static void pthread_mutex_unlock(int *lock) {}
static int vr_openxr_input_read(struct vr_openxr_frame *frame) { *frame = input; return connected; }
static char const *config_string(char const *name)
{
 if(!strcmp(name,"vr.push_to_talk"))return talk;
 if(!strcmp(name,"audio.voice_chat"))return voice;
 if(!strcmp(name,"vr.aim"))return aim_mode;
 if(!strcmp(name,"vr.dominant_hand"))return dominant;
 if(!strcmp(name,"vr.swap_sticks"))return swap_sticks;
 if(!strcmp(name,"vr.turn"))return turn_mode;
 if(!strcmp(name,"vr.weapon_offset"))return "0.15,-0.22,0.30";
 CHECK(FALSE,"unknown setting %s",name);return "";
}
#include "under_test.inc"

/* Bounded native world/math adapters; actual role selection, convergence,
 * weapon-camera calibration, node reflection and cull scope are compiled. */
#define HALO_OPENXR 1
#define ABS(x) ((x)<0?-(x):(x))
#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))
#define VALID_INDEX(i,n) ((i)>=0&&(i)<(n))
#define NUMBEROF(a) (sizeof(a)/sizeof((a)[0]))
#define TICKS_PER_SECOND 30
#define MAXIMUM_WEAPONS_PER_UNIT 4
#define PIN(v,a,b) ((v)<(a)?(a):((v)>(b)?(b):(v)))
#define _pi 3.14159265358979323846f
#define _2pi (2.0*_pi)
#define METRES_PER_WORLD_UNIT 3.048f
#define _object_mask_biped 1
#define _object_mask_weapon 2
#define _object_dead_bit 0
#define _director_perspective_first_person 0
#define MAXIMUM_COLLISION_USER_STACK_DEPTH 8
#define _collision_user_aim_assist 0
#define _collision_test_for_line_of_sight_flags 0
#define _collision_test_objects_bipeds_bit 0
typedef struct {real scale;real_vector3d forward,left,up;real_point3d position;} real_matrix4x3;
typedef struct {real yaw,pitch;} real_euler_angles2d;
typedef struct {real i,j;} real_vector2d;
struct render_camera {real_point3d position;real_vector3d forward,up;};
struct unit_datum {struct {long parent_object_index,damage_flags;} object;struct {long player_index;short current_weapon_index;long weapon_object_indices[4];} unit;};
struct player_datum {long unit_index;short action_result;long action_object_index;};
struct object_datum {long definition_index;struct {long parent_object_index;real_point3d bounding_sphere_center;real bounding_sphere_radius;real_vector3d forward;} object;};
static struct object_datum offered,held_weapon;
static char const *held_name="weapons\\assault rifle\\assault rifle";
static char const *tag_get_name(long handle){return handle==1?held_name:NULL;}
struct collision_result {real_point3d point;};
static struct unit_datum unit={{NONE,0},{1}};
static struct player_datum player={42};
static real_vector3d world_up={0,0,1};
static real_vector3d const *global_up3d=&world_up;
static struct {BOOL active,origin_valid,aim_synced,snap_armed,weapon_camera_valid,weapon_mirrored;
 short eye;long aim_unit;int aim_hand,turn_hand;unsigned int reference_serial;
 real body_yaw,origin[3],yaw_cos,yaw_sin;struct render_camera weapon_camera;
 BOOL grab_armed;long grab_unit,grab_object,grab_until;unsigned int grab_reference;int grab_hand;
 BOOL support_armed,support_held;long support_unit,support_weapon,support_tick;unsigned int support_reference;
 int support_hand;real support_weight;real_vector3d support_line;} stereo;
static long now;
static long game_time_get(void){return now;}
static int global_current_collision_user_depth,global_current_collision_users[8];
static BOOL openxr_mirrored_draws;
static unsigned long config_changes(void){return 0;}
static double config_real(char const *name){return 30.0;}
static long local_player_get_player_index(short local){return 1;}
static struct player_datum *player_try_and_get(long handle){return &player;}
static void *object_try_and_get_and_verify_type(long handle,long mask){if(mask==_object_mask_weapon)return handle==77?&offered:(handle==88||handle==89?&held_weapon:NULL);return handle==42?&unit:NULL;}
static int director_get_perspective(short local){return 0;}
static int cinematic_in_progress(void){return FALSE;}
static real cosine(real x){return cosf(x);}
static real sine(real x){return sinf(x);}
static real arctangent(real y,real x){return atan2f(y,x);}
static real dot_product3d(real_vector3d const *a,real_vector3d const *b){return a->i*b->i+a->j*b->j+a->k*b->k;}
static void cross_product3d(real_vector3d const *a,real_vector3d const *b,real_vector3d *c){*c=(real_vector3d){a->j*b->k-a->k*b->j,a->k*b->i-a->i*b->k,a->i*b->j-a->j*b->i};}
static void vector_from_points3d(real_point3d const *a,real_point3d const *b,real_vector3d *v){*v=(real_vector3d){b->x-a->x,b->y-a->y,b->z-a->z};}
static real magnitude_squared3d(real_vector3d const *v){return dot_product3d(v,v);}
static real normalize3d(real_vector3d *v){real length=sqrtf(magnitude_squared3d(v));if(length>0){v->i/=length;v->j/=length;v->k/=length;}return length;}
static void scale_vector3d(real_vector3d const *v,real scale,real_vector3d *out){*out=(real_vector3d){v->i*scale,v->j*scale,v->k*scale};}
static real distance3d(real_point3d const *a,real_point3d const *b){real_vector3d v;vector_from_points3d(a,b,&v);return sqrtf(magnitude_squared3d(&v));}
static int valid_real_point3d(real_point3d const *p){return isfinite(p->x)&&isfinite(p->y)&&isfinite(p->z);}
static int valid_real_vector3d(real_vector3d const *v){return isfinite(v->i)&&isfinite(v->j)&&isfinite(v->k);}
static int vr_openxr_tracking_read(struct vr_openxr_frame *frame){*frame=input;return connected;}
static long object_get_ultimate_parent(long object){return object;}
static void director_camera_deterministic(long object,real_point3d *eye,real_vector3d *forward){*eye=(real_point3d){0,0,0};*forward=(real_vector3d){1,0,0};}
static int collision_test_vector(long flags,real_point3d const *p,real_vector3d const *v,long ignore,struct collision_result *hit){return FALSE;}
#include "cull.inc"
#include "pose.inc"

static void reset(void)
{
 memset(&input, 0, sizeof(input));
 input.focused = input.should_render = TRUE;
 input.hand[0].grip.valid = input.hand[1].grip.valid = TRUE;
 input.reference_serial = 1;
 connected = TRUE;
 grab_enabled=support_enabled=FALSE;now=100;
 talk = "off";
 voice = "push_to_talk";
 openxr_talk_armed = openxr_talk_held = FALSE;
 openxr_talk_binding = 0;
 openxr_talk_reference = 0;
 aim_mode="controller";dominant="right";swap_sticks="auto";turn_mode="snap";
 openxr_layout_known=openxr_layout_wait=FALSE;
 memset(&stereo,0,sizeof(stereo));stereo.eye=NONE;stereo.aim_unit=NONE;
}

static void talk_button(int binding, BOOL down)
{
 input.hand[0].squeeze = input.hand[1].squeeze = 0.f;
 input.hand[0].buttons = input.hand[1].buttons = 0;
 if (binding < 2) input.hand[binding].squeeze = down ? 1.f : 0.f;
 else input.hand[binding - 2].buttons = down ? _vr_button_stick : 0;
}

int main(int argc, char **argv)
{
 char const *case_name = argv[1];
 XINPUT_GAMEPAD pad, neutral;
 int binding;
 static char const *bindings[] = { "left_grip", "right_grip", "left_stick", "right_stick" };
 reset();
 memset(&neutral, 0, sizeof(neutral));
 if(!strncmp(case_name,"support-",8)) {
  real_euler_angles2d facing={0,0};real_vector2d throttle={0,0};
  struct render_camera base={{0,0,0},{1,0,0},{0,0,1}};
  int i,hand=!strcmp(case_name+8,"left")?0:1,offhand=1-hand;real before;
  if(!hand)dominant="left";
  support_enabled=TRUE;stereo.origin_valid=TRUE;stereo.reference_serial=1;stereo.yaw_cos=1;
  input.hand[0].aim.valid=input.hand[1].aim.valid=TRUE;
  input.hand[0].aim.orientation[3]=input.hand[1].aim.orientation[3]=1;
  input.hand[offhand].grip.position[0]=.1f;input.hand[offhand].grip.position[2]=-.4f;
  unit.unit.current_weapon_index=0;unit.unit.weapon_object_indices[0]=88;
  held_weapon.definition_index=1;held_weapon.object.parent_object_index=42;
  if(!strcmp(case_name+8,"mapping")) {
   openxr_gamepad_read(&pad,FALSE);input.hand[offhand].squeeze=1;openxr_gamepad_read(&pad,FALSE);
   CHECK(!pad.bAnalogButtons[XINPUT_GAMEPAD_WHITE],"support grip also emitted its native action");
   talk=offhand?"right_grip":"left_grip";input.hand[offhand].squeeze=0;openxr_gamepad_read(&pad,FALSE);
   input.hand[offhand].squeeze=1;openxr_gamepad_read(&pad,FALSE);
   CHECK(openxr_talk_held&&!pad.bAnalogButtons[XINPUT_GAMEPAD_WHITE],"support grip stole PTT");
   talk="off";support_enabled=FALSE;openxr_gamepad_read(&pad,FALSE);
   CHECK(!memcmp(&pad,&neutral,sizeof(pad)),"changing support layout retained held gameplay input");
   input.hand[offhand].squeeze=0;openxr_gamepad_read(&pad,FALSE);
   input.hand[offhand].squeeze=1;openxr_gamepad_read(&pad,FALSE);
   CHECK(pad.bAnalogButtons[XINPUT_GAMEPAD_WHITE],"disabled support lost native grip mapping");return 0;
  }
  if(!strcmp(case_name+8,"held"))input.hand[offhand].squeeze=1;
  CHECK(vr_render_player_aim(0,42,TRUE,0,0,&facing,&throttle),"support setup lost native aim");
  if(!strcmp(case_name+8,"held")) {
   CHECK(!stereo.support_weight&&!stereo.support_held,"held grip attached on activation");
   input.hand[offhand].squeeze=0;now++;vr_render_player_aim(0,42,TRUE,facing.yaw,0,&facing,&throttle);
  }
  if(!strcmp(case_name+8,"near"))input.hand[offhand].grip.position[2]=-.01f;
  if(!strcmp(case_name+8,"far"))input.hand[offhand].grip.position[2]=-1.f;
  if(!strcmp(case_name+8,"side"))input.hand[offhand].grip.position[0]=.4f;
  input.hand[offhand].squeeze=1;
  now++;vr_render_player_aim(0,42,TRUE,facing.yaw,0,&facing,&throttle);
  if(!strcmp(case_name+8,"duplicate")) {
   before=stereo.support_weight;
   CHECK(before>0&&before<1,"blend did not start progressively");
   for(i=0;i<8;i++) {
    vr_render_player_aim(0,42,TRUE,facing.yaw,0,&facing,&throttle);prepare_weapon_camera(&base,&input);
   }
   CHECK(stereo.support_weight==before,"same-tick/eye calls advanced support blend");return 0;
  }
  for(i=0;i<8;i++){now++;vr_render_player_aim(0,42,TRUE,facing.yaw,0,&facing,&throttle);}
  if(!strcmp(case_name+8,"near")||!strcmp(case_name+8,"far")||!strcmp(case_name+8,"side")) {
   CHECK(!stereo.support_weight&&!stereo.support_held,"out-of-reach squeeze braced a weapon");
   input.hand[offhand].grip.position[0]=.1f;input.hand[offhand].grip.position[2]=-.4f;
   now++;vr_render_player_aim(0,42,TRUE,facing.yaw,0,&facing,&throttle);
   CHECK(!stereo.support_weight,"held failed squeeze attached when moved into range");return 0;
  }
  prepare_weapon_camera(&base,&input);
  CHECK(stereo.support_held&&stereo.support_weight==1&&sinf(facing.yaw)<-.15f&&
   stereo.weapon_camera_valid&&stereo.weapon_camera.forward.j<-.15f,"native aim and weapon posing did not share two-hand direction");
  if(!strcmp(case_name+8,"loop")||!strcmp(case_name+8,"left")||!strcmp(case_name+8,"held"))return 0;
  if(!strcmp(case_name+8,"tracking"))input.hand[offhand].grip.valid=FALSE;
  if(!strcmp(case_name+8,"reference"))input.reference_serial=stereo.reference_serial=2;
  if(!strcmp(case_name+8,"ownership"))held_weapon.object.parent_object_index=43;
  if(!strcmp(case_name+8,"one-handed"))held_name="weapons\\pistol\\pistol";
  if(!strcmp(case_name+8,"ptt"))talk=offhand?"right_grip":"left_grip";
  if(!strcmp(case_name+8,"nonfinite"))input.hand[offhand].grip.position[0]=NAN;
  if(!strcmp(case_name+8,"weapon"))unit.unit.weapon_object_indices[0]=89;
  if(!strcmp(case_name+8,"seat"))unit.object.parent_object_index=99;
  if(!strcmp(case_name+8,"release"))input.hand[offhand].squeeze=0;
  if(!strcmp(case_name+8,"disabled"))support_enabled=FALSE;
  if(!strcmp(case_name+8,"gap"))now+=30;
  for(i=0;i<8;i++){now++;vr_render_player_aim(0,42,TRUE,facing.yaw,0,&facing,&throttle);}
  CHECK(!stereo.support_held&&!stereo.support_weight,"unavailable state retained two-handed aim");
  if(!strcmp(case_name+8,"one-handed")) {
   static char const *names[]={"weapons\\needler\\needler","weapons\\flag\\flag","weapons\\ball\\ball"};
   for(i=0;i<3;i++){held_name=names[i];input.hand[offhand].squeeze=0;now++;vr_render_player_aim(0,42,TRUE,facing.yaw,0,&facing,&throttle);
    input.hand[offhand].squeeze=1;now++;vr_render_player_aim(0,42,TRUE,facing.yaw,0,&facing,&throttle);CHECK(!stereo.support_weight,"one-handed native weapon braced");}
  }
  input.hand[offhand].grip.valid=TRUE;input.hand[offhand].grip.position[0]=.1f;
  held_weapon.object.parent_object_index=42;held_name="weapons\\assault rifle\\assault rifle";
  unit.object.parent_object_index=NONE;talk="off";support_enabled=TRUE;input.hand[offhand].squeeze=1;
  now++;vr_render_player_aim(0,42,TRUE,facing.yaw,0,&facing,&throttle);
  if(strcmp(case_name+8,"release"))CHECK(!stereo.support_held&&!stereo.support_weight,"held grip reattached after loss or rebinding");
  input.hand[offhand].squeeze=0;now++;vr_render_player_aim(0,42,TRUE,facing.yaw,0,&facing,&throttle);
  input.hand[offhand].squeeze=1;
  for(i=0;i<8;i++){now++;vr_render_player_aim(0,42,TRUE,facing.yaw,0,&facing,&throttle);}
  CHECK(stereo.support_held&&stereo.support_weight==1,"fresh squeeze did not restore support grip");return 0;
 }
 if(!strncmp(case_name,"grab-",5)) {
  unsigned long flags=0;
  int hand=!strcmp(case_name+5,"left")?0:1;
  if(!hand)dominant="left";
  grab_enabled=TRUE;stereo.origin_valid=stereo.aim_synced=TRUE;
  stereo.aim_unit=42;stereo.reference_serial=1;stereo.yaw_cos=1;
  player.unit_index=42;player.action_result=_player_action_result_pickup_weapon;player.action_object_index=77;
  offered.object.parent_object_index=NONE;offered.object.bounding_sphere_center=(real_point3d){.1f,0,0};
  offered.object.bounding_sphere_radius=.05f;offered.object.forward=(real_vector3d){1,0,0};
  input.hand[hand].grip.position[2]=-.3048f;
  if(!strcmp(case_name+5,"mapping")) {
   openxr_gamepad_read(&pad,FALSE);input.hand[hand].squeeze=1;input.hand[0].buttons=_vr_button_primary;
   openxr_gamepad_read(&pad,FALSE);
   CHECK(!pad.bAnalogButtons[XINPUT_GAMEPAD_BLACK]&&pad.bAnalogButtons[XINPUT_GAMEPAD_X],"physical grab toggled its native grip action or removed manual Action");
   grab_enabled=FALSE;openxr_gamepad_read(&pad,FALSE);
   CHECK(!memcmp(&pad,&neutral,sizeof(pad)),"live grab change did not require neutral input");
   input.hand[0].buttons=0;input.hand[hand].squeeze=0;openxr_gamepad_read(&pad,FALSE);
   input.hand[hand].squeeze=1;openxr_gamepad_read(&pad,FALSE);
   CHECK(pad.bAnalogButtons[XINPUT_GAMEPAD_BLACK],"disabled grab lost the existing grip binding");return 0;
  }
  if(!strcmp(case_name+5,"held")) {
   input.hand[hand].squeeze=1;vr_render_player_grab(0,42,TRUE,&flags);
   CHECK(!flags,"already-held grip picked up on activation");input.hand[hand].squeeze=0;
  }
  vr_render_player_grab(0,42,TRUE,&flags);
  if(!strcmp(case_name+5,"reach"))input.hand[hand].grip.position[0]=1.f;
  if(!strcmp(case_name+5,"ownership"))offered.object.parent_object_index=42;
  if(!strcmp(case_name+5,"tracking"))input.focused=FALSE;
  if(!strcmp(case_name+5,"ptt"))talk=hand?"right_grip":"left_grip";
  if(!strcmp(case_name+5,"reference"))input.reference_serial++;
  if(!strcmp(case_name+5,"nonfinite"))input.hand[hand].grip.position[0]=NAN;
  if(!strcmp(case_name+5,"trigger"))input.hand[hand].trigger=1.f;
  if(!strcmp(case_name+5,"disabled"))grab_enabled=FALSE;
  input.hand[hand].squeeze=1;vr_render_player_grab(0,42,TRUE,&flags);
  if(!strcmp(case_name+5,"reach")||!strcmp(case_name+5,"ownership")||!strcmp(case_name+5,"tracking")||!strcmp(case_name+5,"ptt")||
     !strcmp(case_name+5,"reference")||!strcmp(case_name+5,"nonfinite")||!strcmp(case_name+5,"trigger")||!strcmp(case_name+5,"disabled")) {
   CHECK(!flags,"unavailable reach/ownership/tracking or PTT caused pickup");
   if(!strcmp(case_name+5,"ptt")) {
    input.hand[hand].squeeze=0;openxr_gamepad_read(&pad,FALSE);
    input.hand[hand].squeeze=1;openxr_gamepad_read(&pad,FALSE);
    CHECK(openxr_talk_held&&!pad.bAnalogButtons[XINPUT_GAMEPAD_BLACK],"physical grab stole grip PTT");
   }
   input.hand[hand].grip.position[0]=0;offered.object.parent_object_index=NONE;input.focused=TRUE;talk="off";
   input.reference_serial=1;input.hand[hand].trigger=0;grab_enabled=TRUE;
   vr_render_player_grab(0,42,TRUE,&flags);CHECK(!flags,"held grip picked up after a disallowed state");
   input.hand[hand].squeeze=0;vr_render_player_grab(0,42,TRUE,&flags);
   input.hand[hand].squeeze=1;vr_render_player_grab(0,42,TRUE,&flags);
  }
  CHECK(flags==(FLAG(_unit_control_action_bit)|FLAG(_unit_control_swap_weapons_bit)),"native pickup/swap input was not emitted");
  flags=0;now+=4;vr_render_player_grab(0,42,TRUE,&flags);CHECK(!flags,"held grip repeated its pickup");
  player.action_object_index+=0x10000;vr_render_player_grab(0,42,TRUE,&flags);
  CHECK(!flags,"a reused native object slot inherited a grip request");
  return 0;
 }
 CASE("mapping") {
  input.hand[0].buttons = input.hand[1].buttons = _vr_button_primary | _vr_button_secondary;
  input.hand[0].squeeze = input.hand[1].squeeze = 1.f;
  input.hand[0].trigger = .5f;
  input.hand[1].trigger = 1.f;
  input.hand[0].stick[0] = -1.f; input.hand[0].stick[1] = 1.f;
  CHECK(openxr_gamepad_read(&pad, FALSE), "XR pad not connected");
  CHECK(pad.bAnalogButtons[XINPUT_GAMEPAD_A] == 255 && pad.bAnalogButtons[XINPUT_GAMEPAD_B] == 255 &&
   pad.bAnalogButtons[XINPUT_GAMEPAD_X] == 255 && pad.bAnalogButtons[XINPUT_GAMEPAD_Y] == 255, "native face buttons not mapped");
  CHECK(pad.bAnalogButtons[XINPUT_GAMEPAD_WHITE] == 255 && pad.bAnalogButtons[XINPUT_GAMEPAD_BLACK] == 255, "native grip actions missing");
  CHECK(pad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] == 127 && pad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] == 255,
   "native trigger actions missing");
  CHECK(pad.sThumbLX == -32767 && pad.sThumbLY == 32767, "native walking axes wrong");
  CHECK(pad_used_ms == 100, "XR actions did not update the native input hint");
 } else CASE("focus") {
  input.hand[1].trigger = 1.f; input.hand[0].stick[1] = 1.f; input.hand[1].buttons = _vr_button_primary;
  input.focused = FALSE; openxr_talk_armed = openxr_talk_held = TRUE;
  CHECK(openxr_gamepad_read(&pad, FALSE) && !memcmp(&pad, &neutral, sizeof(pad)), "unfocused actions reached Halo");
  CHECK(!openxr_talk_held && !openxr_talk_armed, "focus loss retained microphone action");
  input.focused = TRUE; connected = FALSE;
  CHECK(!openxr_gamepad_read(&pad, FALSE) && !memcmp(&pad, &neutral, sizeof(pad)), "disconnected actions reached Halo");
 } else CASE("pause") {
  input.hand[0].buttons = input.hand[1].buttons = _vr_button_stick;
  input.hand[1].trigger = input.hand[0].squeeze = 1.f;
  openxr_gamepad_read(&pad, FALSE);
  neutral.wButtons = XINPUT_GAMEPAD_START;
  CHECK(!memcmp(&pad, &neutral, sizeof(pad)), "pause combination fired, threw or zoomed");
 } else CASE("ptt") {
  for (binding = 0; binding < 4; binding++) {
   reset(); talk = bindings[binding];
   talk_button(binding, TRUE); openxr_gamepad_read(&pad, FALSE);
   CHECK(!openxr_talk_held && !memcmp(&pad, &neutral, sizeof(pad)), "held binding was armed or used for gameplay");
   talk_button(binding, FALSE); openxr_gamepad_read(&pad, FALSE);
   talk_button(binding, TRUE); openxr_gamepad_read(&pad, FALSE);
   CHECK(openxr_talk_held && !memcmp(&pad, &neutral, sizeof(pad)), "PTT not isolated from grenade/zoom actions");
   input.hand[binding % 2].grip.valid = FALSE; openxr_gamepad_read(&pad, FALSE);
   CHECK(!openxr_talk_held && !memcmp(&pad, &neutral, sizeof(pad)), "tracking gap leaked assigned gameplay action");
   input.hand[binding % 2].grip.valid = TRUE; openxr_gamepad_read(&pad, FALSE);
   CHECK(!openxr_talk_held, "tracking recovery transmitted held microphone");
   talk_button(binding, FALSE); openxr_gamepad_read(&pad, FALSE);
   talk_button(binding, TRUE); openxr_gamepad_read(&pad, FALSE);
   CHECK(openxr_talk_held, "release did not rearm PTT");
   openxr_gamepad_read(&pad, TRUE); CHECK(!openxr_talk_held, "menu transmitted microphone");
   openxr_gamepad_read(&pad, FALSE); CHECK(!openxr_talk_held, "menu return transmitted held microphone");
  }
 } else CASE("reference") {
  talk = "left_grip";
  openxr_gamepad_read(&pad, FALSE); input.hand[0].squeeze = 1.f; openxr_gamepad_read(&pad, FALSE);
  CHECK(openxr_talk_held, "PTT did not start");
  input.reference_serial++; openxr_gamepad_read(&pad, FALSE);
  CHECK(!openxr_talk_held, "reference change retained PTT");
 } else CASE("privacy") {
  talk = "left_grip"; voice = "off";
  openxr_gamepad_read(&pad, FALSE); input.hand[0].squeeze = 1.f; openxr_gamepad_read(&pad, FALSE);
  CHECK(!openxr_talk_held && pad.bAnalogButtons[XINPUT_GAMEPAD_WHITE] == 255, "voice-off changed privacy or grip action");
  voice = "open_mic"; openxr_gamepad_read(&pad, FALSE);
  CHECK(!openxr_talk_held && pad.bAnalogButtons[XINPUT_GAMEPAD_WHITE] == 255, "open-mic mode reserved PTT binding");
 } else CASE("axes") {
  input.hand[0].stick[0] = nanf(""); input.hand[0].stick[1] = 2.f;
  input.hand[1].trigger = nanf("");
  openxr_gamepad_read(&pad, FALSE);
  CHECK(!pad.sThumbLX && pad.sThumbLY == 32767 && !pad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER], "axis conversion out of range");
 } else CASE("left-handed") {
  dominant="left";input.hand[0].trigger=1.f;input.hand[1].trigger=.25f;
  input.hand[0].squeeze=1.f;input.hand[1].buttons=_vr_button_primary;
  input.hand[0].buttons=_vr_button_stick;input.hand[1].stick[1]=1.f;
  openxr_gamepad_read(&pad,FALSE);
  CHECK(pad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER]==255&&pad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER]==63,
   "left weapon/secondary triggers not assigned");
  CHECK(pad.bAnalogButtons[XINPUT_GAMEPAD_BLACK]==255&&!pad.bAnalogButtons[XINPUT_GAMEPAD_WHITE]&&
   pad.bAnalogButtons[XINPUT_GAMEPAD_A]==255,"grip role or physical face label changed");
  CHECK(pad.sThumbLY==32767&&(pad.wButtons&XINPUT_GAMEPAD_RIGHT_THUMB),"left-handed sticks/clicks not assigned");
 } else CASE("stick-choice") {
  dominant="left";swap_sticks="no";input.hand[0].stick[1]=1.f;input.hand[1].stick[0]=.5f;
  openxr_gamepad_read(&pad,FALSE);
  CHECK(pad.sThumbLY==32767&&pad.sThumbRX==16383&&vr_openxr_weapon_hand()==0,"independent stick choice ignored");
  openxr_gamepad_read(&pad,TRUE);
  CHECK(pad.sThumbLY==32767&&pad.sThumbRX==16383,"menu sticks were swapped");
  reset();dominant="left";swap_sticks="yes";aim_mode="gamepad";
  input.hand[0].trigger=1.f;input.hand[1].stick[0]=1.f;openxr_gamepad_read(&pad,FALSE);
  CHECK(vr_openxr_weapon_hand()==1&&vr_openxr_turn_hand()==1&&
   pad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER]==255&&pad.sThumbRX==32767,"gamepad aim changed native halves");
 } else CASE("layout-change") {
  input.hand[0].trigger=1.f;openxr_gamepad_read(&pad,FALSE);
  dominant="left";openxr_gamepad_read(&pad,FALSE);
  CHECK(!memcmp(&pad,&neutral,sizeof(pad)),"held grenade became fire during rebinding");
  input.hand[0].buttons=input.hand[1].buttons=_vr_button_stick;openxr_gamepad_read(&pad,FALSE);
  CHECK(pad.wButtons==XINPUT_GAMEPAD_START&&!pad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER],"rebind wait blocked pause or fired");
  input.hand[0].buttons=input.hand[1].buttons=0;input.hand[0].trigger=0.f;
  input.hand[0].squeeze=nanf("");openxr_gamepad_read(&pad,FALSE);CHECK(openxr_layout_wait,"invalid sample armed a new layout");
  input.hand[0].squeeze=0.f;openxr_gamepad_read(&pad,FALSE);
  input.hand[0].trigger=1.f;openxr_gamepad_read(&pad,FALSE);
  CHECK(pad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER]==255&&!pad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER],"neutral layout did not rearm left fire");
 } else CASE("physical-ptt") {
  dominant="left";talk="left_grip";openxr_gamepad_read(&pad,FALSE);
  input.hand[0].squeeze=input.hand[1].squeeze=1.f;openxr_gamepad_read(&pad,FALSE);
  CHECK(openxr_talk_held&&!pad.bAnalogButtons[XINPUT_GAMEPAD_BLACK]&&pad.bAnalogButtons[XINPUT_GAMEPAD_WHITE]==255,
   "physical left PTT moved hands or leaked its new weapon-grip action");
 } else CASE("left-pose") {
  real_euler_angles2d facing={0,0};real_vector2d throttle={0,0};
  struct render_camera base={{0,0,0},{1,0,0},{0,0,1}};
  dominant="left";stereo.active=stereo.origin_valid=TRUE;stereo.eye=0;stereo.reference_serial=1;
  stereo.yaw_cos=1.f;stereo.origin[1]=1.5f;
  input.hand[0].aim.valid=input.hand[1].aim.valid=TRUE;
  input.hand[0].aim.orientation[3]=1.f;input.hand[1].aim.orientation[1]=input.hand[1].aim.orientation[3]=.70710677f;
  input.hand[0].aim.position[0]=-.2f;input.hand[1].aim.position[0]=.2f;
  input.hand[0].aim.position[1]=input.hand[1].aim.position[1]=1.5f;
  CHECK(vr_render_player_aim(0,42,TRUE,0,0,&facing,&throttle)&&fabsf(facing.yaw)<.05f,"left native facing came from right controller");
  prepare_weapon_camera(&base,&input);
  CHECK(stereo.weapon_camera_valid&&stereo.weapon_mirrored&&stereo.weapon_camera.forward.i>.99f,"left native model camera used wrong controller");
  dominant="right";
  CHECK(vr_render_player_aim(0,42,TRUE,facing.yaw,0,&facing,&throttle)&&facing.yaw>.5f,"dominant-hand change lost native aim or body heading");
  prepare_weapon_camera(&base,&input);
  CHECK(stereo.weapon_camera_valid&&!stereo.weapon_mirrored&&stereo.weapon_camera.forward.j>.99f,"right native model camera did not resume");
 } else CASE("reflection") {
  real_matrix4x3 original={2,{1,0,0},{0,1,0},{0,0,1},{4,4,5}},nodes[2];
  stereo.active=stereo.weapon_camera_valid=stereo.weapon_mirrored=TRUE;stereo.eye=0;
  stereo.weapon_camera=(struct render_camera){{1,2,3},{1,0,0},{0,0,1}};
  nodes[0]=nodes[1]=original;vr_render_weapon_nodes(0,&nodes[0],1);
  stereo.eye=1;vr_render_weapon_nodes(0,&nodes[1],1);
  CHECK(!memcmp(&nodes[0],&nodes[1],sizeof(nodes[0]))&&nodes[0].position.x==4&&nodes[0].position.y==0&&
   nodes[0].position.z==5&&nodes[0].left.j==-1&&nodes[0].scale==2,"eye poses differ or reflection changed depth/scale");
  vr_render_weapon_draw_begin();
  CHECK(draw_cull_mode(D3DCULL_CW)==D3DCULL_CCW&&draw_cull_mode(D3DCULL_CCW)==D3DCULL_CW&&
   draw_cull_mode(D3DCULL_NONE)==D3DCULL_NONE,"mirrored winding or two-sided draw incorrect");
  vr_render_weapon_draw_end();CHECK(draw_cull_mode(D3DCULL_CW)==D3DCULL_CW,"winding leaked to world/HUD");
  stereo.eye=NONE;nodes[0]=original;vr_render_weapon_nodes(0,&nodes[0],1);vr_render_weapon_draw_begin();
  CHECK(!memcmp(&nodes[0],&original,sizeof(original))&&!openxr_mirrored_draws,"flat rendering changed");
 } else CASE("snap-hand") {
  dominant="left";input.hand[1].stick[0]=.9f;
  CHECK(controller_turn_delta(&input,0)==0,"move stick caused snap turning");
  input.hand[0].stick[0]=.9f;
  CHECK(fabsf(controller_turn_delta(&input,0)+_pi/6.f)<.0001f,"left turn stick did not snap");
  CHECK(controller_turn_delta(&input,0)==0,"held stick repeated snap");
  swap_sticks="no";CHECK(controller_turn_delta(&input,0)==0,"physical turn-stick change reused armed latch");
  input.hand[1].stick[0]=0;controller_turn_delta(&input,0);input.hand[1].stick[0]=-.9f;
  CHECK(fabsf(controller_turn_delta(&input,0)-_pi/6.f)<.0001f,"explicit right turn stick did not rearm");
 } else return 2;
 return 0;
}
