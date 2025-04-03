#include "wsi_platform.hpp"
#include "config.hpp"
#ifdef USING_SDL2
    #include <SDL2/SDL_vulkan.h>
#else
    #include <SDL3/SDL_vulkan.h>
#endif

VkSurfaceKHR SDL_WSIPlatform::create_surface(VkInstance instance, VkPhysicalDevice gpu)
{
	VkSurfaceKHR surface = nullptr;
	bool result;
	#ifdef USING_SDL2
	 	result = SDL_Vulkan_CreateSurface(window, instance, &surface);
	#else
		result = SDL_Vulkan_CreateSurface(window, instance, NULL, &surface);
	#endif

	if (result != true)
	{
		printf("Error creating surface\n");
	}
	return surface;
}

void SDL_WSIPlatform::destroy_surface(VkInstance instance, VkSurfaceKHR surface)
{
	#ifdef USING_SDL3
		SDL_Vulkan_DestroySurface(instance, surface, NULL);
	#endif
	// do nothing for SDL2.
}

std::vector<const char *> SDL_WSIPlatform::get_instance_extensions()
{
	#ifdef USING_SDL3
		unsigned int extensionCount = 0;
		char const *const *extensions = SDL_Vulkan_GetInstanceExtensions(&extensionCount);
		if (extensions == NULL)
		{
			printf("Error getting instance extensions\n");
		}

		std::vector<const char *> extensionNames;
		for (int i = 0; i < extensionCount; ++i)
		{
			extensionNames.push_back(extensions[i]);
		}
		return extensionNames;
	#else
		unsigned int extensionCount = 0;

		SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, NULL);

		std::vector<const char*> extensionNames(extensionCount);

		SDL_Vulkan_GetInstanceExtensions(window, &extensionCount, &extensionNames[0]);

		return extensionNames;
	#endif
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
