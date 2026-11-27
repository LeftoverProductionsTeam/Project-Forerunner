#ifndef FORERUNNER_RENDER_SKY_LIGHTING_H
#define FORERUNNER_RENDER_SKY_LIGHTING_H

#include "math/real_math.h"

struct render_sky_outdoor_lighting
{
	real_vector3d sunlight_direction;
	real_rgb_color sunlight_radiance;
	real_rgb_color ambient_radiance;
	real sunlight_diameter;
};

/* Returns the strongest authored outdoor directional light plus ambient
 * radiosity from the selected sky. The direction points into the scene. */
boolean render_sky_get_outdoor_lighting(short sky_index, struct render_sky_outdoor_lighting *lighting);

#endif
