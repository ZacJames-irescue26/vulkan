#pragma once
#include "frame.h"
namespace vkInit
{
	struct SwapChainSupportDetails
	{
		vk::SurfaceCapabilitiesKHR capabilities;
		std::vector<vk::SurfaceFormatKHR> formats;
		std::vector<vk::PresentModeKHR> presentModes;
	};



	struct SwapChainBundle {
		vk::SwapchainKHR swapchain;
		std::vector<vkUtil::SwapChainFrame> frames;
		vk::Format format;
		vk::Extent2D extent;
	};


	SwapChainSupportDetails query_swapchain_support(vk::PhysicalDevice device, vk::SurfaceKHR surface, bool debug);

	vk::SurfaceFormatKHR choose_swapchain_surface_format(std::vector<vk::SurfaceFormatKHR> formats);

	vk::PresentModeKHR choose_swapchain_present_mode(std::vector < vk::PresentModeKHR >presentModes);

	vk::Extent2D choose_swapchain_extent(uint32_t width, uint32_t height, vk::SurfaceCapabilitiesKHR capabilities);

	SwapChainBundle create_swapchain(vk::Device logicalDevice, vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface, int width, int height, bool debug);
}