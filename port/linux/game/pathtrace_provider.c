/* Private OpenRemix direct SDK preview. Build with the pinned OpenRemix headers. */
#if defined(_WIN32) && defined(HALO_64BIT) && !defined(HALO_SERVER)
#include "cseries.h"
#ifdef HALO_OPENREMIX_SDK
#include "pathtrace_provider.h"
#include "pathtrace_scene.h"
#include "bitmaps/bitmap_group.h"
#include "bitmaps/bitmap_group_lookup.h"
#include "bitmaps/bitmaps_sampling.h"
#include "cache/cache_files.h"
#include "cache/texture_cache.h"
#include "game/game.h"
#include "render/render_cameras.h"
#include "render/render_sky_lighting.h"
#include "structures/structure_bsp_definitions.h"

#define REMIX_WINAPI_NO_INCLUDE
#define REMIX_WINAPI_NO_LIBRARY_LOADER
#include <remix/remix_editor_ext.h>
#include <remix/remix_native_ext.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

/* Use real Win32 symbols for the opaque external GPU handles. */
#undef CloseHandle
#undef GetLastError
__declspec(dllimport) int __stdcall CloseHandle(void *);
__declspec(dllimport) unsigned long __stdcall GetLastError(void);
__declspec(dllimport) remixapi_HMODULE __stdcall LoadLibraryExA(const char *, void *, unsigned long);
__declspec(dllimport) remixapi_loader_PROC __stdcall GetProcAddress(remixapi_HMODULE, const char *);
__declspec(dllimport) int __stdcall FreeLibrary(remixapi_HMODULE);
__declspec(dllimport) unsigned long __stdcall GetFullPathNameA(const char *, unsigned long, char *, char **);

extern void platform_log(const char *format, ...);

enum
{
	maximum_preview_material_sections = 1024,
	maximum_preview_textures = 128,
	maximum_preview_texture_side = 1024,
	maximum_preview_texture_bytes = 32 * 1024 * 1024
};

struct preview_texture
{
	long group;
	short page;
	remixapi_TextureHandle handle;
	remixapi_MaterialHandle material[2];
	uint64_t hash;
};

static struct
{
	remixapi_HMODULE module;
	remixapi_Interface api;
	PFN_remixapi_GetBackendCapabilitiesEXT capabilities;
	remixapi_MaterialHandle material;
	remixapi_LightHandle sunlight, ambient;
	remixapi_MeshHandle mesh;
	struct preview_texture textures[maximum_preview_textures];
	unsigned int texture_count, texture_bytes;
	const struct structure_bsp *structure;
	char map[128];
	short bsp, sky;
	long last_tick;
	unsigned long long mesh_generation;
	int started, submitted, sky_available;
} preview;

typedef int (REMIXAPI_CALL *win32_configure_frame)(remixapi_EditorSharedFrameWin32 *);

static int status(remixapi_ErrorCode result, const char *action)
{
	if (result != REMIXAPI_ERROR_CODE_SUCCESS)
		platform_log("OpenRemix preview: %s failed (%d)", action, (int)result);
	return result == REMIXAPI_ERROR_CODE_SUCCESS;
}

void pathtrace_provider_close_handles(struct win32_shared_frame_description *description)
{
	if (description->memory_handle) CloseHandle(description->memory_handle);
	if (description->ready_handle) CloseHandle(description->ready_handle);
	if (description->released_handle) CloseHandle(description->released_handle);
	description->memory_handle = NULL;
	description->ready_handle = NULL;
	description->released_handle = NULL;
}

static void retire_bsp(void)
{
	unsigned int i;

	if (preview.mesh)
		preview.api.DestroyMesh(preview.mesh);
	preview.mesh = NULL;
	for (i = 0; i < preview.texture_count; i++)
	{
		struct preview_texture *texture = &preview.textures[i];
		if (texture->material[0]) preview.api.DestroyMaterial(texture->material[0]);
		if (texture->material[1]) preview.api.DestroyMaterial(texture->material[1]);
		if (texture->handle) preview.api.DestroyTexture(texture->handle);
	}
	preview.texture_count = preview.texture_bytes = 0;
}

void pathtrace_provider_close(void)
{
	if (preview.started)
	{
		retire_bsp();
		if (preview.sunlight) preview.api.DestroyLight(preview.sunlight);
		if (preview.ambient) preview.api.DestroyLight(preview.ambient);
		if (preview.material) preview.api.DestroyMaterial(preview.material);
		preview.api.Shutdown();
	}
	if (preview.module)
		FreeLibrary(preview.module);
	memset(&preview, 0, sizeof(preview));
}

int pathtrace_provider_open(const char *path, unsigned int width, unsigned int height,
	struct win32_shared_frame_description *description)
{
	remixapi_InitializeLibraryInfo library = {0};
	remixapi_EditorStartupEXT editor = {0};
	remixapi_StartupInfo startup = {0};
	remixapi_EditorSharedFrameWin32 frame = {0};
	remixapi_MaterialInfoOpaqueEXT opaque = {0};
	remixapi_MaterialInfo material = {0};
	PFN_remixapi_InitializeLibrary initialize;
	win32_configure_frame configure;
	char full_path[1024];
	unsigned long length = GetFullPathNameA(path, sizeof(full_path), full_path, NULL);

	if (!length || length >= sizeof(full_path))
		return FALSE;
	preview.module = LoadLibraryExA(full_path, NULL,
		REMIX_WINAPI_LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | REMIX_WINAPI_LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
	if (!preview.module)
	{
		platform_log("OpenRemix preview: provider library could not load (%lu)", GetLastError());
		return FALSE;
	}
	initialize = (PFN_remixapi_InitializeLibrary)GetProcAddress(preview.module, "remixapi_InitializeLibrary");
	configure = (win32_configure_frame)GetProcAddress(preview.module, "remixapi_EditorConfigureSharedFrameWin32");
	preview.capabilities = (PFN_remixapi_GetBackendCapabilitiesEXT)GetProcAddress(preview.module, "remixapi_GetBackendCapabilitiesEXT");
	if (!initialize || !configure || !preview.capabilities)
		return FALSE;

	library.sType = REMIXAPI_STRUCT_TYPE_INITIALIZE_LIBRARY_INFO;
	library.version = REMIXAPI_VERSION_MAKE(REMIXAPI_VERSION_MAJOR, REMIXAPI_VERSION_MINOR, REMIXAPI_VERSION_PATCH);
	if (!status(initialize(&library, &preview.api), "initialize") ||
		!preview.api.Startup || !preview.api.Shutdown || !preview.api.SetConfigVariable ||
		!preview.api.CreateMaterial || !preview.api.DestroyMaterial ||
		!preview.api.CreateTexture || !preview.api.DestroyTexture ||
		!preview.api.CreateLight || !preview.api.DrawLightInstance || !preview.api.DestroyLight ||
		!preview.api.CreateMeshBatched || !preview.api.DrawInstance || !preview.api.DestroyMesh ||
		!preview.api.SetupCamera || !preview.api.Present)
		return FALSE;
	editor.sType = REMIXAPI_STRUCT_TYPE_EDITOR_STARTUP_EXT;
	editor.version = 1;
	editor.headless = editor.disableProfiles = editor.disableMods = 1;
	startup.sType = REMIXAPI_STRUCT_TYPE_STARTUP_INFO;
	startup.pNext = &editor;
	platform_log("OpenRemix preview: starting the direct provider");
	if (!status(preview.api.Startup(&startup), "start"))
		return FALSE;
	preview.started = TRUE;
	if (!status(preview.api.SetConfigVariable("rtx.native.backend", "pathtrace"), "select path tracing") ||
		!status(preview.api.SetConfigVariable("rtx.native.pathtrace.upscaler", "off"), "select native resolution") ||
		!status(preview.api.SetConfigVariable("rtx.native.pathtrace.denoiser", "off"), "disable denoising"))
		return FALSE;

	opaque.sType = REMIXAPI_STRUCT_TYPE_MATERIAL_INFO_OPAQUE_EXT;
	opaque.albedoConstant = (remixapi_Float3D){0.7f, 0.65f, 0.56f};
	opaque.opacityConstant = 1.0f;
	opaque.roughnessConstant = 0.85f;
	opaque.alphaTestType = 7;
	material.sType = REMIXAPI_STRUCT_TYPE_MATERIAL_INFO;
	material.pNext = &opaque;
	material.hash = 0x201;
	if (!status(preview.api.CreateMaterial(&material, &preview.material), "create preview material"))
		return FALSE;

	frame.version = 1;
	frame.width = width;
	frame.height = height;
	platform_log("OpenRemix preview: exporting the provider image");
	if (configure(&frame))
		return FALSE;
	description->width = frame.width;
	description->height = frame.height;
	description->memory_type = frame.memory_type;
	description->allocation_size = frame.allocation_size;
	memcpy(description->device_uuid, frame.device_uuid, sizeof(description->device_uuid));
	description->memory_handle = frame.memory_handle;
	description->ready_handle = frame.ready_handle;
	description->released_handle = frame.released_handle;
	return frame.width == width && frame.height == height &&
		frame.memory_handle && frame.ready_handle && frame.released_handle;
}

struct preview_mesh
{
	remixapi_MeshInfoSurfaceTriangles surfaces[maximum_preview_material_sections];
	unsigned int count, textured;
};

static void upload_texture(struct preview_texture *texture)
{
	struct bitmap_data *bitmap = bitmap_group_try_and_get_bitmap(texture->group, texture->page);
	remixapi_TextureInfo info = {0};
	pixel32 *pixels;
	unsigned int bytes;
	short y;

	if (!bitmap || bitmap->width <= 0 || bitmap->height <= 0 || bitmap->width > maximum_preview_texture_side ||
		bitmap->height > maximum_preview_texture_side)
		return;
	bytes = bitmap->width * bitmap->height * sizeof(*pixels);
	if (bytes > maximum_preview_texture_bytes - preview.texture_bytes || !texture_cache_bitmap_load(bitmap))
		return;
	pixels = malloc(bytes);
	if (!pixels)
		return;
	for (y = 0; y < bitmap->height; y++)
		if (!bitmap_2d_read_color_row(bitmap, y, pixels + y * bitmap->width))
			break;
	if (y == bitmap->height)
	{
		info.sType = REMIXAPI_STRUCT_TYPE_TEXTURE_INFO;
		info.hash = texture->hash;
		info.width = bitmap->width;
		info.height = bitmap->height;
		info.depth = info.mipLevels = 1;
		info.format = REMIXAPI_FORMAT_B8G8R8A8_SRGB;
		info.data = pixels;
		info.dataSize = bytes;
		if (status(preview.api.CreateTexture(&info, &texture->handle), "upload base bitmap"))
			preview.texture_bytes += bytes;
	}
	free(pixels);
}

static remixapi_MaterialHandle surface_material(const struct pathtrace_material *material)
{
	remixapi_MaterialInfo info = {0};
	remixapi_MaterialInfoOpaqueEXT opaque = {0};
	struct preview_texture *texture;
	unsigned int i;
	int alpha = material->alpha_tested != 0;
	wchar_t uri[24];

	if (material->diffuse_group == NONE || material->diffuse_bitmap == NONE)
		return preview.material;
	for (i = 0; i < preview.texture_count; i++)
		if (preview.textures[i].group == material->diffuse_group && preview.textures[i].page == material->diffuse_bitmap)
			break;
	if (i == preview.texture_count)
	{
		if (i == maximum_preview_textures)
			return preview.material;
		texture = &preview.textures[preview.texture_count++];
		memset(texture, 0, sizeof(*texture));
		texture->group = material->diffuse_group;
		texture->page = material->diffuse_bitmap;
		texture->hash = 0x100000000ULL + (preview.mesh_generation << 16) + i;
		upload_texture(texture);
	}
	texture = &preview.textures[i];
	if (!texture->handle)
		return preview.material;
	if (!texture->material[alpha])
	{
		_snwprintf(uri, NUMBEROF(uri), L"0x%016llx", (unsigned long long)texture->hash);
		opaque.sType = REMIXAPI_STRUCT_TYPE_MATERIAL_INFO_OPAQUE_EXT;
		opaque.albedoConstant = (remixapi_Float3D){1.0f, 1.0f, 1.0f};
		opaque.opacityConstant = 1.0f;
		opaque.roughnessConstant = 0.85f;
		opaque.alphaTestType = alpha ? 4 : 7; /* VK_COMPARE_OP_GREATER / ALWAYS */
		opaque.alphaReferenceValue = 0x7f;
		info.sType = REMIXAPI_STRUCT_TYPE_MATERIAL_INFO;
		info.pNext = &opaque;
		info.hash = 0x200000000ULL + (preview.mesh_generation << 16) + i * 2 + alpha;
		info.albedoTexture = uri;
		status(preview.api.CreateMaterial(&info, &texture->material[alpha]), "bind base bitmap");
	}
	return texture->material[alpha] ? texture->material[alpha] : preview.material;
}

static int upload_surface(void *context, const struct pathtrace_material *material)
{
	struct preview_mesh *mesh = context;
	remixapi_MeshInfoSurfaceTriangles *surface;
	remixapi_HardcodedVertex *vertices;
	uint32_t *indices;
	long i;

	/* Native transparency is drawn over the traced opaque frame. */
	if (!material->opaque)
		return TRUE;
	if (mesh->count >= maximum_preview_material_sections)
		return FALSE;

	vertices = malloc(material->vertex_count * sizeof(*vertices));
	indices = malloc(material->surface_count * 3 * sizeof(*indices));
	if (!vertices || !indices)
	{
		free(vertices);
		free(indices);
		return FALSE;
	}
	memset(vertices, 0, material->vertex_count * sizeof(*vertices));
	surface = &mesh->surfaces[mesh->count++];
	surface->vertices_values = vertices;
	surface->vertices_count = material->vertex_count;
	surface->indices_values = indices;
	surface->indices_count = material->surface_count * 3;
	surface->material = surface_material(material);
	if (surface->material != preview.material)
		mesh->textured++;
	for (i = 0; i < material->vertex_count; i++)
	{
		struct pathtrace_vertex vertex;

		pathtrace_scene_read_vertex(material, i, &vertex);
		vertices[i].position[0] = vertex.position[0];
		vertices[i].position[1] = vertex.position[2];
		vertices[i].position[2] = vertex.position[1];
		vertices[i].normal[0] = vertex.normal[0];
		vertices[i].normal[1] = vertex.normal[2];
		vertices[i].normal[2] = vertex.normal[1];
		memcpy(vertices[i].texcoord, vertex.texcoord, sizeof(vertex.texcoord));
		vertices[i].color = 0xffffffff;
	}
	/* Swapping Halo's Y/Z reflects the basis, so the winding also flips. */
	for (i = 0; i < material->surface_count; i++)
	{
		const word *triangle = material->surfaces[i].vertex_indices;

		indices[i * 3] = triangle[0];
		indices[i * 3 + 1] = triangle[2];
		indices[i * 3 + 2] = triangle[1];
	}
	return TRUE;
}

static int upload_bsp(const struct structure_bsp *structure)
{
	struct preview_mesh *upload = malloc(sizeof(*upload));
	remixapi_MeshInfo geometry = {0};
	remixapi_MeshHandle mesh = NULL;
	unsigned int i;

	if (!upload)
		return FALSE;
	++preview.mesh_generation;
	memset(upload, 0, sizeof(*upload));
	if (pathtrace_scene_visit(structure, upload_surface, upload) && upload->count)
	{
		geometry.sType = REMIXAPI_STRUCT_TYPE_MESH_INFO;
		geometry.hash = 0x10000 + preview.mesh_generation;
		geometry.surfaces_count = upload->count;
		geometry.surfaces_values = upload->surfaces;
		status(preview.api.CreateMeshBatched(&geometry, &mesh), "upload live BSP");
	}
	for (i = 0; i < upload->count; i++)
	{
		free((void *)upload->surfaces[i].vertices_values);
		free((void *)upload->surfaces[i].indices_values);
	}
	if (mesh)
		platform_log("OpenRemix preview: uploaded the resident BSP (%u/%u textured sections, %u base texture bytes)",
			upload->textured, upload->count, preview.texture_bytes);
	free(upload);
	preview.mesh = mesh;
	return mesh != NULL;
}

/* The provider's direct API takes one sun and one dome per frame. Explicit
 * zero-radiance entries prevent a prior BSP's lighting carrying forward. */
static int upload_sky(short sky)
{
	struct render_sky_outdoor_lighting native;
	remixapi_LightInfo light = {0};
	remixapi_LightInfoDistantEXT distant = {0};
	remixapi_LightInfoDomeEXT dome = {0};

	if (preview.sunlight) preview.api.DestroyLight(preview.sunlight);
	if (preview.ambient) preview.api.DestroyLight(preview.ambient);
	preview.sunlight = preview.ambient = NULL;
	preview.sky = sky;
	preview.sky_available = render_sky_get_outdoor_lighting(sky, &native);
	if (!preview.sky_available)
	{
		platform_log("OpenRemix preview: sky %d has no outdoor light, using the native raster view", sky);
		return TRUE;
	}

	light.sType = REMIXAPI_STRUCT_TYPE_LIGHT_INFO;
	distant.sType = REMIXAPI_STRUCT_TYPE_LIGHT_INFO_DISTANT_EXT;
	distant.direction = (remixapi_Float3D){native.sunlight_direction.i, native.sunlight_direction.k,
		native.sunlight_direction.j};
	if (distant.direction.x == 0.0f && distant.direction.y == 0.0f && distant.direction.z == 0.0f)
		distant.direction.y = -1.0f;
	/* The provider samples a cone about this axis. Cap it at a hemisphere
	 * to keep even an over-broad authored source a valid direction. */
	distant.angularDiameterDegrees = MIN(native.sunlight_diameter * 57.2957795f, 179.0f);
	light.pNext = &distant;
	light.hash = 0x203;
	light.radiance = (remixapi_Float3D){native.sunlight_radiance.red, native.sunlight_radiance.green,
		native.sunlight_radiance.blue};
	if (!status(preview.api.CreateLight(&light, &preview.sunlight), "upload native sky sun"))
		return FALSE;

	dome.sType = REMIXAPI_STRUCT_TYPE_LIGHT_INFO_DOME_EXT;
	dome.transform.matrix[0][0] = dome.transform.matrix[1][1] = dome.transform.matrix[2][2] = 1.0f;
	light.pNext = &dome;
	light.hash = 0x204;
	light.radiance = (remixapi_Float3D){native.ambient_radiance.red, native.ambient_radiance.green, native.ambient_radiance.blue};
	if (!status(preview.api.CreateLight(&light, &preview.ambient), "upload native ambient sky"))
		return FALSE;
	platform_log("OpenRemix preview: sky %d sun dir (%.3f %.3f %.3f), radiance (%.3f %.3f %.3f), diameter %.2f deg; ambient (%.3f %.3f %.3f)",
		sky, distant.direction.x, distant.direction.y, distant.direction.z,
		native.sunlight_radiance.red, native.sunlight_radiance.green, native.sunlight_radiance.blue,
		distant.angularDiameterDegrees, light.radiance.x, light.radiance.y, light.radiance.z);
	return TRUE;
}

int pathtrace_provider_draw(const struct structure_bsp *structure, short bsp, short sky, const struct render_camera *camera)
{
	const char *map = cache_file_loaded_map_name();
	long tick = game_time_get();
	remixapi_CameraInfo camera_info = {0};
	remixapi_CameraInfoParameterizedEXT pose = {0};
	remixapi_InstanceInfo instance = {0};
	remixapi_BackendCapabilitiesEXT capabilities = {0};
	int new_scene;

	if (!structure || !map || !*map || !strcmp(map, "ui") || strlen(map) >= sizeof(preview.map))
		return pathtrace_provider_error;
	new_scene = preview.structure != structure || preview.bsp != bsp ||
		tick < preview.last_tick || strcmp(map, preview.map);
	if (new_scene)
	{
		retire_bsp();
		preview.structure = structure;
		preview.bsp = bsp;
		strcpy(preview.map, map);
	}
	preview.last_tick = tick;
	if ((new_scene || sky != preview.sky) && !upload_sky(sky))
		return pathtrace_provider_error;
	if (!preview.sky_available)
		return pathtrace_provider_no_sky;
	if (!preview.mesh && !upload_bsp(structure))
		return pathtrace_provider_error;

	pose.sType = REMIXAPI_STRUCT_TYPE_CAMERA_INFO_PARAMETERIZED_EXT;
	pose.position = (remixapi_Float3D){camera->position.x, camera->position.z, camera->position.y};
	pose.forward = (remixapi_Float3D){camera->forward.i, camera->forward.k, camera->forward.j};
	pose.up = (remixapi_Float3D){camera->up.i, camera->up.k, camera->up.j};
	pose.right = (remixapi_Float3D){
		pose.up.y * pose.forward.z - pose.up.z * pose.forward.y,
		pose.up.z * pose.forward.x - pose.up.x * pose.forward.z,
		pose.up.x * pose.forward.y - pose.up.y * pose.forward.x};
	pose.aspect = (float)(camera->viewport_bounds.x1 - camera->viewport_bounds.x0) /
		(camera->viewport_bounds.y1 - camera->viewport_bounds.y0);
	pose.fovYInDegrees = camera->vertical_field_of_view * 57.2957795f;
	pose.nearPlane = camera->z_near;
	pose.farPlane = camera->z_far;
	camera_info.sType = REMIXAPI_STRUCT_TYPE_CAMERA_INFO;
	camera_info.pNext = &pose;
	instance.sType = REMIXAPI_STRUCT_TYPE_INSTANCE_INFO;
	instance.mesh = preview.mesh;
	instance.doubleSided = 1;
	instance.transform.matrix[0][0] = instance.transform.matrix[1][1] = instance.transform.matrix[2][2] = 1.0f;
	if (!status(preview.api.SetupCamera(&camera_info), "update live camera") ||
		!status(preview.api.DrawInstance(&instance), "submit live BSP") ||
		!status(preview.api.DrawLightInstance(preview.sunlight), "submit native sky sun") ||
		!status(preview.api.DrawLightInstance(preview.ambient), "submit native ambient sky") ||
		!status(preview.api.Present(NULL), "render shared frame"))
		return pathtrace_provider_error;
	if (!preview.submitted)
	{
		preview.submitted = TRUE;
		platform_log("OpenRemix preview: submitted the first live camera");
	}
	capabilities.structSize = sizeof(capabilities);
	if (!status(preview.capabilities(&capabilities), "query active backend") ||
		!(capabilities.supportedBackends & (1u << REMIXAPI_RENDER_BACKEND_PATHTRACE_EXT)))
		return pathtrace_provider_unavailable;
	return capabilities.activeBackend == REMIXAPI_RENDER_BACKEND_PATHTRACE_EXT ?
		pathtrace_provider_traced : pathtrace_provider_warming_up;
}
#else
struct render_camera;
struct structure_bsp;
struct win32_shared_frame_description;

int pathtrace_provider_open(const char *path, unsigned int width, unsigned int height,
	struct win32_shared_frame_description *description)
{
	(void)path; (void)width; (void)height; (void)description;
	return 0;
}

int pathtrace_provider_draw(const struct structure_bsp *structure, short bsp, short sky, const struct render_camera *camera)
{
	(void)structure; (void)bsp; (void)sky; (void)camera;
	return -1;
}

void pathtrace_provider_close_handles(struct win32_shared_frame_description *description)
{
	(void)description;
}

void pathtrace_provider_close(void)
{
}
#endif
#endif
