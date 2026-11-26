#ifndef HALO_SCENARIO_LIMITS_H
#define HALO_SCENARIO_LIMITS_H

/* The scenario reference limit and its native placement mask are shared
 * by the game and Spark's read-only runtime discovery. */
enum
{
	SCENARIO_PLACEMENT_BSP_MEMBERSHIP_BITS = 16,
	MAXIMUM_STRUCTURE_BSPS_PER_SCENARIO = SCENARIO_PLACEMENT_BSP_MEMBERSHIP_BITS
};

#endif
