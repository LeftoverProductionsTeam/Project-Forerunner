#ifndef FORERUNNER_WIN32_PATHTRACE_PROVIDER_H
#define FORERUNNER_WIN32_PATHTRACE_PROVIDER_H

#include "win32_shared_frame.h"

struct render_camera;
struct structure_bsp;

int pathtrace_provider_open(const char *path, unsigned int width, unsigned int height,
	struct win32_shared_frame_description *description);
enum pathtrace_provider_result
{
	pathtrace_provider_error = -2,
	pathtrace_provider_unavailable = -1,
	pathtrace_provider_warming_up,
	pathtrace_provider_traced
};
/* Every result except pathtrace_provider_error follows a successful Present.
 * The consumer must release that frame even if it uses the native view. */
int pathtrace_provider_draw(const struct structure_bsp *structure, short bsp, const struct render_camera *camera);
void pathtrace_provider_close_handles(struct win32_shared_frame_description *description);
void pathtrace_provider_close(void);

#endif
