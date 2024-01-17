#pragma once
#include "queue_families.h"
#include "frame.h"

namespace vkInit
{
	struct commandBufferInputChunk {
		vk::Device device;
		vk::CommandPool commandPool;
		std::vector<vkUtil::SwapChainFrame>& frames;
	};
	
	vk::CommandPool make_command_pool(vk::Device device, vk::PhysicalDevice physicaldevice, vk::SurfaceKHR surface, bool debug);
	vk::CommandBuffer make_command_buffer(vkInit::commandBufferInputChunk inputChunk, bool debug);
	void make_frame_command_buffers(commandBufferInputChunk inputChunk, bool debug);
}