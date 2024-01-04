#include "pch.h"
#include "swapchain.h"
#include "queue_families.h"
#include "frame.h"


vk::SurfaceFormatKHR vkInit::choose_swapchain_surface_format(std::vector<vk::SurfaceFormatKHR> formats)
{
	for (vk::SurfaceFormatKHR format : formats)
	{
		if (format.format == vk::Format::eB8G8R8A8Unorm && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
		{
			return format;
		}
	}

	return formats[0];
}

vk::PresentModeKHR vkInit::choose_swapchain_present_mode(std::vector <vk::PresentModeKHR> presentModes)
{
	for (vk::PresentModeKHR presentMode : presentModes)
	{
		if (presentMode == vk::PresentModeKHR::eMailbox)
		{
			return presentMode;
		}
	}
	return vk::PresentModeKHR::eFifo;
}

vk::Extent2D vkInit::choose_swapchain_extent(uint32_t width, uint32_t height, vk::SurfaceCapabilitiesKHR capabilities)
{
	if (capabilities.currentExtent.width != UINT32_MAX) {
		return capabilities.currentExtent;
	}
	else {
		vk::Extent2D extent = { width, height };

		extent.width = std::min(
			capabilities.maxImageExtent.width,
			std::max(capabilities.minImageExtent.width, extent.width)
		);

		extent.height = std::min(
			capabilities.maxImageExtent.height,
			std::max(capabilities.minImageExtent.height, extent.height)
		);

		return extent;
	}
}

vkInit::SwapChainBundle vkInit::create_swapchain(vk::Device logicalDevice, vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface, int width, int height, bool debug)
{
	SwapChainSupportDetails support = query_swapchain_support(physicalDevice, surface, debug);

	vk::SurfaceFormatKHR format = choose_swapchain_surface_format(support.formats);

	vk::PresentModeKHR presentMode = choose_swapchain_present_mode(support.presentModes);

	vk::Extent2D extent = choose_swapchain_extent(width, height, support.capabilities);

	uint32_t imageCount = std::min(
		support.capabilities.maxImageCount,
		support.capabilities.minImageCount + 1
	);

	vk::SwapchainCreateInfoKHR createInfo = vk::SwapchainCreateInfoKHR(
		vk::SwapchainCreateFlagsKHR(), surface, imageCount, format.format, format.colorSpace,
		extent, 1, vk::ImageUsageFlagBits::eColorAttachment
	);


	vkUtil::QueueFamilyIndices indices = vkUtil::findQueueFamilies(physicalDevice, surface, debug);
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = vk::SharingMode::eExclusive;
	}

	createInfo.preTransform = support.capabilities.currentTransform;
	createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	createInfo.oldSwapchain = vk::SwapchainKHR(nullptr);

	SwapChainBundle bundle{};
	try {
		bundle.swapchain = logicalDevice.createSwapchainKHR(createInfo);
	}
	catch (vk::SystemError err) {
		throw std::runtime_error("failed to create swap chain!");
	}

	std::vector<vk::Image> images = logicalDevice.getSwapchainImagesKHR(bundle.swapchain);
	bundle.frames.resize(images.size());

	for (size_t i = 0; i < images.size(); i++)
	{

		vk::ImageViewCreateInfo createInfo = {};
		createInfo.image = images[i];
		createInfo.viewType = vk::ImageViewType::e2D;
		createInfo.components.r = vk::ComponentSwizzle::eIdentity;
		createInfo.components.g = vk::ComponentSwizzle::eIdentity;
		createInfo.components.b = vk::ComponentSwizzle::eIdentity;
		createInfo.components.a = vk::ComponentSwizzle::eIdentity;
		createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;
		createInfo.format = format.format;

		bundle.frames[i].image = images[i];
		bundle.frames[i].imageView = logicalDevice.createImageView(createInfo);
	}
	bundle.format = format.format;
	bundle.extent = extent;

	return bundle;
}

vkInit::SwapChainSupportDetails vkInit::query_swapchain_support(vk::PhysicalDevice device, vk::SurfaceKHR surface, bool debug)
{
	SwapChainSupportDetails support;

	support.capabilities = device.getSurfaceCapabilitiesKHR(surface);

	support.formats = device.getSurfaceFormatsKHR(surface);

	support.presentModes = device.getSurfacePresentModesKHR(surface);


	return support;
}