/* Stock BSP scene capture for the optional renderer's native acceptance. */
#if defined(_WIN32) && defined(HALO_64BIT) && !defined(HALO_SERVER)
#include "cseries.h"
#include "pathtrace_scene.h"

#include "bitmaps/bitmap_group.h"
#include "bitmaps/bitmap_group_lookup.h"
#include "bitmaps/bitmaps_sampling.h"
#include "cache/cache_files.h"
#include "cache/texture_cache.h"
#include "shaders/shader_definitions.h"
#include "shaders/shaders.h"
#include "game/game.h"
#include "rasterizer/rasterizer_geometry_environment.h"
#include "render/render_cameras.h"
#include "structures/structure_bsp_definitions.h"

#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>

extern const char *config_string(const char *name);
extern int config_boolean(const char *name);
extern void platform_log(const char *format, ...);
extern void halo_test_screenshot(void);

enum
{
	compressed_vertex_stride = 32,
	compressed_lightmap_stride = 8,
	maximum_scene_vertices = 262144,
	maximum_scene_surfaces = 262144,
	maximum_texture_side = 1024,
	maximum_capture_textures = 128,
	capture_texture_bytes = 32 * 1024 * 1024
};

int pathtrace_scene_visit(const struct structure_bsp *structure, pathtrace_material_sink sink, void *context)
{
	long total_vertices = 0, total_surfaces = 0;
	long map_index, material_index;

	if (!structure || !sink || structure->surfaces.count <= 0 ||
		structure->surfaces.count > maximum_scene_surfaces)
		return FALSE;

	for (map_index = 0; map_index < structure->lightmaps.count; map_index++)
	{
		const struct structure_lightmap *lightmap = TAG_BLOCK_GET_ELEMENT(&structure->lightmaps, map_index, struct structure_lightmap);

		for (material_index = 0; material_index < lightmap->materials.count; material_index++)
		{
			const struct structure_material *material =
				TAG_BLOCK_GET_ELEMENT(&lightmap->materials, material_index, struct structure_material);
			long vertex_count = material->vertices.count;
			long first = material->first_surface_index;
			long count = material->surface_count;
			long lightmap_count = material->lightmap_vertices.count;
			long bytes = material->compressed_vertex_data.size;
			long index;
			boolean alpha_tested;
			struct pathtrace_material section;

			if (!count)
				continue;
			if (vertex_count <= 0 || vertex_count > bytes / compressed_vertex_stride ||
				vertex_count > maximum_scene_vertices - total_vertices || count < 0 || first < 0 ||
				count > structure->surfaces.count || first > structure->surfaces.count - count ||
				count > maximum_scene_surfaces - total_surfaces)
				return FALSE;

			section.vertices = xbox_pointer(material->compressed_vertex_data.address);
			if (!section.vertices)
				return FALSE;
			section.surfaces = TAG_BLOCK_GET_ELEMENT(&structure->surfaces, first, struct structure_surface);
			section.vertex_count = vertex_count;
			section.surface_count = count;
			section.shader = material->shader.index;
			section.diffuse_group = shader_environment_base_bitmap_group(
				section.shader == NONE ? NULL : shader_definition_get(section.shader), &alpha_tested);
			section.alpha_tested = alpha_tested;
			section.diffuse_bitmap = NONE;
			if (section.diffuse_group != NONE)
			{
				const struct bitmap_group *group = bitmap_group_get(section.diffuse_group);

				if (group && group->bitmaps.count > 0)
					section.diffuse_bitmap = (short)(MAX(0, material->permutation_index) % group->bitmaps.count);
			}
			section.permutation = material->permutation_index;
			section.lightmap_group = structure->lightmap_group.index;
			section.lightmap_bitmap = lightmap->bitmap_index;
			section.lightmap_vertices = NULL;
			if (lightmap_count == vertex_count &&
				lightmap_count <= (bytes - vertex_count * compressed_vertex_stride) / compressed_lightmap_stride)
				section.lightmap_vertices = section.vertices + vertex_count * compressed_vertex_stride;

			for (index = 0; index < count; index++)
			{
				const word *indices = section.surfaces[index].vertex_indices;
				if (indices[0] >= vertex_count || indices[1] >= vertex_count || indices[2] >= vertex_count)
					return FALSE;
			}
			if (!sink(context, &section))
				return FALSE;
			total_vertices += vertex_count;
			total_surfaces += count;
		}
	}
	return total_surfaces > 0;
}

void pathtrace_scene_read_vertex(const struct pathtrace_material *material, long index, struct pathtrace_vertex *vertex)
{
	const unsigned char *data = material->vertices + index * compressed_vertex_stride;
	const struct environment_vertex_compressed *environment = (const void *)data;
	real_point3d position;
	real_vector3d normal;
	real_point2d texcoord;

	environment_vertex_compressed_get_point(environment, &position);
	environment_vertex_compressed_get_normal(environment, &normal);
	environment_vertex_compressed_get_texcoord(environment, &texcoord);
	vertex->position[0] = position.x;
	vertex->position[1] = position.y;
	vertex->position[2] = position.z;
	vertex->normal[0] = normal.i;
	vertex->normal[1] = normal.j;
	vertex->normal[2] = normal.k;
	vertex->texcoord[0] = texcoord.x;
	vertex->texcoord[1] = texcoord.y;
	texcoord.x = texcoord.y = 0.0f;
	if (material->lightmap_vertices)
		environment_lightmap_vertex_compressed_get_texcoord(
			(const void *)(material->lightmap_vertices + index * compressed_lightmap_stride), &texcoord);
	vertex->lightmap_texcoord[0] = texcoord.x;
	vertex->lightmap_texcoord[1] = texcoord.y;
}

struct pathtrace_obj
{
	FILE *file;
	const char *texture_prefix;
	long vertices, triangles;
	long texture_bytes, texture_count;
	struct
	{
		long group;
		short index;
	} textures[maximum_capture_textures];
};

static FILE *open_capture_file(const char *path)
{
	int descriptor = _open(path, _O_WRONLY | _O_CREAT | _O_EXCL | _O_BINARY, _S_IREAD | _S_IWRITE);
	FILE *file = descriptor < 0 ? NULL : _fdopen(descriptor, "wb");

	if (!file && descriptor >= 0)
		_close(descriptor);
	return file;
}

static void write_texture(struct pathtrace_obj *obj, long group, short index)
{
	pixel32 row[maximum_texture_side];
	struct bitmap_data *bitmap;
	char filename[512];
	unsigned char header[18] = {0};
	long i, size;
	int length;
	short y;
	FILE *file;
	int failed;

	if (!obj->texture_prefix || group == NONE || index == NONE || obj->texture_count >= maximum_capture_textures)
		return;
	for (i = 0; i < obj->texture_count; i++)
		if (obj->textures[i].group == group && obj->textures[i].index == index)
			return;
	obj->textures[i].group = group;
	obj->textures[i].index = index;
	obj->texture_count++;
	bitmap = bitmap_group_try_and_get_bitmap(group, index);
	if (!bitmap || bitmap->width <= 0 || bitmap->height <= 0 ||
		bitmap->width > maximum_texture_side || bitmap->height > maximum_texture_side)
		return;
	size = sizeof(header) + bitmap->width * bitmap->height * 4L;
	if (size > capture_texture_bytes - obj->texture_bytes ||
		!texture_cache_bitmap_load(bitmap) || !bitmap_2d_read_color_row(bitmap, 0, row))
		return;

	length = snprintf(filename, sizeof(filename), "%s-%08lx-%d.tga", obj->texture_prefix, (unsigned long)group, index);
	if (length < 0 || length >= sizeof(filename))
		return;
	file = open_capture_file(filename);
	if (!file)
		return;
	/* RGBA Targa stores blue first, with rows starting at the upper left. */
	header[2] = 2;
	header[12] = (unsigned char)bitmap->width;
	header[13] = (unsigned char)(bitmap->width >> 8);
	header[14] = (unsigned char)bitmap->height;
	header[15] = (unsigned char)(bitmap->height >> 8);
	header[16] = 32;
	header[17] = 0x28;
	failed = fwrite(header, 1, sizeof(header), file) != sizeof(header);
	for (y = 0; y < bitmap->height && !failed; y++)
	{
		if (y != 0 && !bitmap_2d_read_color_row(bitmap, y, row))
			failed = TRUE;
		else
			failed = fwrite(row, 4, bitmap->width, file) != bitmap->width;
	}
	failed |= fclose(file) != 0;
	if (failed)
		_unlink(filename);
	else
		obj->texture_bytes += size;
}

static int write_material(void *context, const struct pathtrace_material *material)
{
	struct pathtrace_obj *obj = context;
	long index;

	fprintf(obj->file, "g shader_%08lx\n", (unsigned long)material->shader);
	fprintf(obj->file, "# material permutation %d lightmap %08lx %d diffuse %08lx %d alpha_test %d\n",
		material->permutation, (unsigned long)material->lightmap_group, material->lightmap_bitmap,
		(unsigned long)material->diffuse_group, material->diffuse_bitmap, material->alpha_tested);
	write_texture(obj, material->diffuse_group, material->diffuse_bitmap);
	if (material->lightmap_vertices)
		write_texture(obj, material->lightmap_group, material->lightmap_bitmap);
	for (index = 0; index < material->vertex_count; index++)
	{
		struct pathtrace_vertex vertex;

		pathtrace_scene_read_vertex(material, index, &vertex);
		fprintf(obj->file, "v %.9g %.9g %.9g\n", vertex.position[0], vertex.position[1], vertex.position[2]);
		fprintf(obj->file, "vt %.9g %.9g\n", vertex.texcoord[0], vertex.texcoord[1]);
		fprintf(obj->file, "vn %.9g %.9g %.9g\n", vertex.normal[0], vertex.normal[1], vertex.normal[2]);
		if (material->lightmap_vertices)
			fprintf(obj->file, "# lightmap_uv %ld %.9g %.9g\n", obj->vertices + index + 1,
				vertex.lightmap_texcoord[0], vertex.lightmap_texcoord[1]);
	}
	for (index = 0; index < material->surface_count; index++)
	{
		const word *indices = material->surfaces[index].vertex_indices;
		long a = indices[0] + obj->vertices + 1;
		long b = indices[1] + obj->vertices + 1;
		long c = indices[2] + obj->vertices + 1;

		fprintf(obj->file, "f %ld/%ld/%ld %ld/%ld/%ld %ld/%ld/%ld\n", a, a, a, b, b, b, c, c, c);
	}
	obj->vertices += material->vertex_count;
	obj->triangles += material->surface_count;
	return !ferror(obj->file);
}

void pathtrace_scene_capture(const struct structure_bsp *structure, short bsp,
	const struct render_camera *camera)
{
	static boolean checked, attempted;
	static const char *path;
	const char *map;
	struct pathtrace_obj obj = {0};
	FILE *file;
	int failed;

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
	file = open_capture_file(path);
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
	obj.file = file;
	if (config_boolean("debug.pathtrace_material_textures"))
		obj.texture_prefix = path;
	failed = !pathtrace_scene_visit(structure, write_material, &obj);
	failed |= fclose(file) != 0;
	if (!failed && *config_string("debug.screenshot_directory"))
		halo_test_screenshot();
	platform_log("Path tracing scene capture: %s BSP %d, %ld vertices, %ld triangles%s",
		map, bsp, obj.vertices, obj.triangles, failed ? " (incomplete capture)" : "");
}
#endif
