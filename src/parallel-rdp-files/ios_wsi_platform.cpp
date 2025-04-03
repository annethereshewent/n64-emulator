#include "ios_wsi_platform.hpp"
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_metal.h>
#include <vulkan/vulkan_ios.h>

VkSurfaceKHR IOS_WSIPlatform::create_surface(VkInstance instance, VkPhysicalDevice gpu)
{
	VkSurfaceKHR surface = nullptr;
	// bool result = SDL_Vulkan_CreateSurface(window, instance, NULL, &surface);

	// if (result != true)
	// {
	// 	printf("Error creating surface\n");
	// }
	// return surface;

    VkMetalSurfaceCreateInfoEXT surfaceCreateInfo = {};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT;
    surfaceCreateInfo.pLayer = metalLayer;

	PFN_vkCreateMetalSurfaceEXT pfnCreateMetalSurfaceEXT =
    	(PFN_vkCreateMetalSurfaceEXT)vkGetInstanceProcAddr(instance, "vkCreateMetalSurfaceEXT");

    VkResult result = pfnCreateMetalSurfaceEXT(instance, &surfaceCreateInfo, nullptr, &surface);

	return surface;
}

void IOS_WSIPlatform::destroy_surface(VkInstance instance, VkSurfaceKHR surface)
{
    // TODO
}

std::vector<const char *> IOS_WSIPlatform::get_instance_extensions()
{
	// unsigned int extensionCount = 0;
	// char const *const *extensions = SDL_Vulkan_GetInstanceExtensions(&extensionCount);
	// if (extensions == NULL)
	// {
	// 	printf("Error getting instance extensions\n");
	// }

	// std::vector<const char *> extensionNames;
	// for (int i = 0; i < extensionCount; ++i)
	// {
	// 	extensionNames.push_back(extensions[i]);
	// }
	// return extensionNames;
    // TODO: actually figure these out
    return { "VK_KHR_surface", "VK_EXT_metal_surface", "VK_MVK_ios_surface" };
}

uint32_t IOS_WSIPlatform::get_surface_width()
{
    return 640;
}

uint32_t IOS_WSIPlatform::get_surface_height()
{
    return 480;
}

bool IOS_WSIPlatform::alive(Vulkan::WSI &wsi)
{
	return true;
}

void IOS_WSIPlatform::poll_input()
{
	// TODO
}

void IOS_WSIPlatform::poll_input_async(Granite::InputTrackerHandler *handler)
{
	// TODO
}

void IOS_WSIPlatform::setLayer(void* layer) {
	metalLayer = layer;
}

void IOS_WSIPlatform::setWidth(uint32_t width) {
	this->width = width;
}

void IOS_WSIPlatform::setHeight(uint32_t height) {
	this->height = height;
}
