#include "pch.h"
#include "commands.h"

vk::CommandPool vkInit::make_command_pool(
	vk::Device device, vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface, bool debug) {

	vkUtil::QueueFamilyIndices queueFamilyIndices = vkUtil::findQueueFamilies(physicalDevice, surface, debug);

	vk::CommandPoolCreateInfo poolInfo;
	poolInfo.flags = vk::CommandPoolCreateFlags() | vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

	try {
		return device.createCommandPool(poolInfo);
	}
	catch (vk::SystemError err) {

		if (debug) {
			std::cout << "Failed to create Command Pool" << std::endl;
		}

		return nullptr;
	}
}

/**
	Make a command buffer for each swapchain frame and return a main command buffer.

	\param inputChunk the required input info
	\param debug whether the system is running in debug mode
	\returns the main command buffer
*/
vk::CommandBuffer vkInit::make_command_buffer(commandBufferInputChunk inputChunk, bool debug) {

	vk::CommandBufferAllocateInfo allocInfo = {};
	allocInfo.commandPool = inputChunk.commandPool;
	allocInfo.level = vk::CommandBufferLevel::ePrimary;
	allocInfo.commandBufferCount = 1;


	//Make a "main" command buffer for the engine
	try {
		vk::CommandBuffer commandBuffer = inputChunk.device.allocateCommandBuffers(allocInfo)[0];

		if (debug) {
			std::cout << "Allocated main command buffer " << std::endl;
		}

		return commandBuffer;
	}
	catch (vk::SystemError err) {

		if (debug) {
			std::cout << "Failed to allocate main command buffer " << std::endl;
		}

		return nullptr;
	}
}

/**
* Make a command buffer for each frame
*
* @param	inputChunk the various fields
* @param	debug whether to print extra information
*/
void vkInit::make_frame_command_buffers(commandBufferInputChunk inputChunk, bool debug) {

	vk::CommandBufferAllocateInfo allocInfo = {};
	allocInfo.commandPool = inputChunk.commandPool;
	allocInfo.level = vk::CommandBufferLevel::ePrimary;
	allocInfo.commandBufferCount = 1;

	//Make a command buffer for each frame
	for (int i = 0; i < inputChunk.frames.size(); ++i) {
		try {
			inputChunk.frames[i].commandBuffer = inputChunk.device.allocateCommandBuffers(allocInfo)[0];

			if (debug) {
				std::cout << "Allocated command buffer for frame " << i << std::endl;
			}
		}
		catch (vk::SystemError err) {

			if (debug) {
				std::cout << "Failed to allocate command buffer for frame " << i << std::endl;
			}
		}
	}
}


	