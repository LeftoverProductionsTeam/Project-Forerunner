#ifndef FORERUNNER_PATHTRACE_SCENE_H
#define FORERUNNER_PATHTRACE_SCENE_H

struct render_camera;
struct structure_bsp;

/* Private, one-shot capture from an actual render view and its loaded BSP. */
void pathtrace_scene_capture(const struct structure_bsp *structure, short bsp,
	const struct render_camera *camera);

#endif
