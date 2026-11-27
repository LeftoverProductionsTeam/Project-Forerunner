#ifndef FORERUNNER_PATHTRACE_PREVIEW_H
#define FORERUNNER_PATHTRACE_PREVIEW_H

struct render_camera;
struct structure_bsp;

#if defined(_WIN32) && defined(HALO_64BIT) && !defined(HALO_SERVER)
/* Private Win64 preview. The raster view stays available until a traced frame
 * for this camera is ready. */
int halo_pathtrace_prepare(const struct structure_bsp *structure, short bsp,
	const struct render_camera *camera, short x0, short y0, short x1, short y1);
void halo_pathtrace_composite(void);
#endif

#endif
