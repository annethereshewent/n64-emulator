#include "wsi_platform.hpp"
#include <SDL2/SDL_vulkan.h>

VkSurfaceKHR SDL_WSIPlatform::create_surface(VkInstance instance, VkPhysicalDevice gpu)
{
	VkSurfaceKHR surface = nullptr;
	bool result = SDL_Vulkan_CreateSurface(window, instance, &surface);
	if (result != true)
	{
		printf("Error creating surface\n");
	}
	std::println("great success!");
	return surface;
}

void SDL_WSIPlatform::destroy_surface(VkInstance instance, VkSurfaceKHR surface)
{
	// do nothing!
}

std::vector<const char *> SDL_WSIPlatform::get_instance_extensions()
{
	unsigned int extensionCount = 0;
	// const char ** extensionNames;
	// SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, extensionNames);

	// for (const char* extensionName: extensionNames) {
	// 	std::println("{}", extensionName);
	// }

	// return extensionNames;
	std::println("getting extension names");
	SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, NULL);
	std::println("got extensionNames, size = {}", extensionCount);

	std::vector<const char*> extensionNames(extensionCount);

	SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, &extensionNames[0]);

	for (const char* name: extensionNames) {
		std::println("name = {}", name);
	}

	return extensionNames;
}

uint32_t SDL_WSIPlatform::get_surface_width()
{
	int w, h;
	SDL_GetWindowSize(window, &w, &h);
	return w;
}

uint32_t SDL_WSIPlatform::get_surface_height()
{
	int w, h;
	SDL_GetWindowSize(window, &w, &h);
	return h;
}

bool SDL_WSIPlatform::alive(Vulkan::WSI &wsi)
{
	return true;
}

void SDL_WSIPlatform::poll_input()
{
	SDL_PumpEvents();
}

void SDL_WSIPlatform::poll_input_async(Granite::InputTrackerHandler *handler)
{
	SDL_PumpEvents();
}

void SDL_WSIPlatform::set_window(SDL_Window *_window)
{
	window = _window;
}

void SDL_WSIPlatform::do_resize()
{
	resize = true;
}
