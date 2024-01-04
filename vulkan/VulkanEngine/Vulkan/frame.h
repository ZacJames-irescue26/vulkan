#pragma once
#include "vulkan/vulkan.hpp"

namespace vkUtil
{
	struct SwapChainFrame
	{
		vk::Image image;
		vk::ImageView imageView;
	};

}
