/* Native controller mapping and PTT ownership, with located XR actions supplied
 * by a fixture. This exercises no loader, headset, microphone or renderer. */
#include "harness.h"
#include <math.h>
#include "types.inc"
typedef int BOOL;
typedef short SHORT;
typedef unsigned char BYTE;
static struct vr_openxr_frame input;
static BOOL connected = TRUE;
static char const *talk = "off", *voice = "push_to_talk";
static BOOL openxr_talk_armed, openxr_talk_held;
static int openxr_talk_binding, mouse_lock;
static unsigned int openxr_talk_reference;
static unsigned long pad_used_ms;
static unsigned long SDL_GetTicks(void) { return 100; }
static void pthread_mutex_lock(int *lock) {}
static void pthread_mutex_unlock(int *lock) {}
static int vr_openxr_input_read(struct vr_openxr_frame *frame) { *frame = input; return connected; }
static char const *config_string(char const *name) { return !strcmp(name, "vr.push_to_talk") ? talk : voice; }
#include "under_test.inc"

static void reset(void)
{
 memset(&input, 0, sizeof(input));
 input.focused = input.should_render = TRUE;
 input.hand[0].grip.valid = input.hand[1].grip.valid = TRUE;
 input.reference_serial = 1;
 connected = TRUE;
 talk = "off";
 voice = "push_to_talk";
 openxr_talk_armed = openxr_talk_held = FALSE;
 openxr_talk_binding = 0;
 openxr_talk_reference = 0;
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
 } else return 2;
 return 0;
}
