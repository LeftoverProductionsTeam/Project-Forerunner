/*
WIN32_WINDOW.C

Keep borderless OpenGL windows on the desktop. NVIDIA's driver treats
SDL's WS_POPUP fullscreen window as exclusive even without a display mode,
which blanks the monitor when the player switches to another application.
*/

#include <windows.h>
#include <SDL3/SDL.h>

bool win32_window_set_icon(SDL_Window *window)
{
	HWND handle = SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
	HINSTANCE application = GetModuleHandleW(NULL);
	HICON small, large;

	if (!handle || !application)
		return false;

	/* Use the native size-specific frames from halo.rc. Shared icon handles
	 * remain valid until the process exits. */
	small = LoadImageW(application, MAKEINTRESOURCEW(1), IMAGE_ICON,
		GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_SHARED);
	large = LoadImageW(application, MAKEINTRESOURCEW(1), IMAGE_ICON,
		GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), LR_SHARED);
	if (!small || !large)
		return false;

	SendMessageW(handle, WM_SETICON, ICON_SMALL, (LPARAM)small);
	SendMessageW(handle, WM_SETICON, ICON_BIG, (LPARAM)large);
	return true;
}

void win32_window_borderless_apply(SDL_Window *window)
{
	HWND handle;
	LONG_PTR style;

	if (!(SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN) || SDL_GetWindowFullscreenMode(window))
		return;
	handle = SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
	if (!handle)
		return;
	style = GetWindowLongPtr(handle, GWL_STYLE);
	if (!(style & WS_POPUP) && !(GetWindowLongPtr(handle, GWL_EXSTYLE) & WS_EX_TOPMOST))
		return;
	SetWindowLongPtr(handle, GWL_STYLE, style & ~WS_POPUP);
	SetWindowPos(handle, HWND_NOTOPMOST, 0, 0, 0, 0,
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_FRAMECHANGED);
}
