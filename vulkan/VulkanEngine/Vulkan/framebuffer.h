#pragma once

#include "frame.h"

namespace vkInit
{
	struct framebufferInput
	{
		vk::Device device;
		vk::RenderPass renderpass;
		vk::Extent2D swapchainExtent;
	};

	void make_framebuffers(framebufferInput inputChunk, std::vector<vkUtil::SwapChainFrame>& frames, bool debug);
}