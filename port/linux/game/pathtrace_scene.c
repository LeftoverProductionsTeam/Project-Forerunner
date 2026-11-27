/* Stock BSP scene capture for the optional renderer's native acceptance. */
#if defined(_WIN32) && defined(HALO_64BIT) && !defined(HALO_SERVER)
#include "cseries.h"
#include "pathtrace_scene.h"

#include "cache/cache_files.h"
#include "game/game.h"
#include "rasterizer/rasterizer_vertex_byte_order.h"
#include "render/render_cameras.h"
#include "structures/structure_bsp_definitions.h"

#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>

extern const char *config_string(const char *name);
extern void platform_log(const char *format, ...);
extern void halo_test_screenshot(void);

enum
{
	compressed_vertex_stride = 32,
	maximum_scene_vertices = 262144,
	maximum_scene_surfaces = 262144
};

static long write_material(FILE *file, const struct structure_bsp *structure,
	const struct structure_material *material, long *total_vertices, long surface_budget)
{
	const unsigned char *vertices;
	long vertex_count = material->vertices.count;
	long first = material->first_surface_index;
	long count = material->surface_count;
	long index, triangles = 0;

	if (vertex_count <= 0 || vertex_count > material->compressed_vertex_data.size / compressed_vertex_stride ||
		vertex_count > maximum_scene_vertices - *total_vertices || count <= 0 || first < 0 ||
		count > structure->surfaces.count || first > structure->surfaces.count - count || count > surface_budget)
		return 0;
	vertices = xbox_pointer(material->compressed_vertex_data.address);
	if (!vertices)
		return 0;

	fprintf(file, "g shader_%08lx\n", (unsigned long)material->shader.index);
	for (index = 0; index < vertex_count; index++)
	{
		const unsigned char *vertex = vertices + index * compressed_vertex_stride;
		real_vector3d normal = uncompress_int32_to_real_vector3d(vertex_data_read_dword(vertex + 12));

		fprintf(file, "v %.9g %.9g %.9g\n", vertex_data_read_float(vertex), vertex_data_read_float(vertex + 4),
			vertex_data_read_float(vertex + 8));
		fprintf(file, "vt %.9g %.9g\n", vertex_data_read_float(vertex + 24), vertex_data_read_float(vertex + 28));
		fprintf(file, "vn %.9g %.9g %.9g\n", normal.i, normal.j, normal.k);
	}
	for (index = first; index < first + count; index++)
	{
		const struct structure_surface *surface = TAG_BLOCK_GET_ELEMENT(&structure->surfaces, index, struct structure_surface);
		long a = surface->vertex_indices[0], b = surface->vertex_indices[1], c = surface->vertex_indices[2];

		if (a >= vertex_count || b >= vertex_count || c >= vertex_count)
			continue;
		a += *total_vertices + 1;
		b += *total_vertices + 1;
		c += *total_vertices + 1;
		fprintf(file, "f %ld/%ld/%ld %ld/%ld/%ld %ld/%ld/%ld\n", a, a, a, b, b, b, c, c, c);
		triangles++;
	}
	*total_vertices += vertex_count;
	return triangles;
}

void pathtrace_scene_capture(const struct structure_bsp *structure, short bsp,
	const struct render_camera *camera)
{
	static boolean checked, attempted;
	static const char *path;
	const char *map;
	long map_index, material_index, vertices = 0, triangles = 0;
	FILE *file;
	int descriptor, failed;

	if (!checked)
	{
		path = config_string("debug.pathtrace_scene");
		checked = TRUE;
	}
	if (attempted || !path || !*path || !structure || !camera || camera->mirrored ||
		camera->vertical_field_of_view <= 0.0f || !game_in_progress() || game_time_get() < 30)
		return;
	map = cache_file_loaded_map_name();
	if (!map || !*map || !strcmp(map, "ui") || structure->surfaces.count <= 0 ||
		structure->surfaces.count > maximum_scene_surfaces)
		return;

	attempted = TRUE;
	descriptor = _open(path, _O_WRONLY | _O_CREAT | _O_EXCL | _O_BINARY, _S_IREAD | _S_IWRITE);
	file = descriptor < 0 ? NULL : _fdopen(descriptor, "wb");
	if (!file && descriptor >= 0)
		_close(descriptor);
	if (!file)
	{
		platform_log("Path tracing scene capture could not create %s", path);
		return;
	}
	fprintf(file, "# Forerunner BSP %d map %s\n", bsp, map);
	fprintf(file, "# camera %.9g %.9g %.9g %.9g %.9g %.9g %.9g %.9g %.9g %.9g %.9g %.9g %.9g\n",
		camera->position.x, camera->position.y, camera->position.z,
		camera->forward.i, camera->forward.j, camera->forward.k, camera->up.i, camera->up.j, camera->up.k,
		camera->vertical_field_of_view, camera->z_near, camera->z_far,
		(double)(camera->viewport_bounds.x1 - camera->viewport_bounds.x0) /
			MAX(1, camera->viewport_bounds.y1 - camera->viewport_bounds.y0));
	for (map_index = 0; map_index < structure->lightmaps.count; map_index++)
	{
		const struct structure_lightmap *lightmap = TAG_BLOCK_GET_ELEMENT(&structure->lightmaps, map_index, struct structure_lightmap);

		for (material_index = 0; material_index < lightmap->materials.count; material_index++)
		{
			const struct structure_material *material =
				TAG_BLOCK_GET_ELEMENT(&lightmap->materials, material_index, struct structure_material);

			triangles += write_material(file, structure, material, &vertices, maximum_scene_surfaces - triangles);
		}
	}

	failed = ferror(file);
	failed |= fclose(file) != 0;
	if (!failed && triangles > 0 && *config_string("debug.screenshot_directory"))
		halo_test_screenshot();
	platform_log("Path tracing scene capture: %s BSP %d, %ld vertices, %ld triangles%s",
		map, bsp, vertices, triangles, failed ? " (write failed)" : "");
}
#endif
