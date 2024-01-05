#include "pch.h"
#include "engine.h"
#include "Vulkan/instance.h"
#include "Vulkan/device.h"
#include "Vulkan/logging.h"
#include "Vulkan/swapchain.h"
#include "Vulkan/pipeline.h"
#include "framebuffer.h"
#include "commands.h"
#include "sync.h"
#include "render_structs.h"

Engine::Engine(int width, int height, GLFWwindow* window, bool debugMode)
{
	this->width = width;
	this->height = height;
	this->window = window;
	this->debugMode = debugMode;
	if (debugMode) {
		std::cout << "Making a graphics engine\n";
	}
	create_instance();
	make_device();
	make_pipeline();
	finalize_setup();
}


void Engine::create_instance()
{
	instance = vkInit::make_instance(debugMode, "ID Tech 12");
	dldi = vk::DispatchLoaderDynamic(instance, vkGetInstanceProcAddr);
	if (debugMode) {
		debugMessenger = vkInit::make_debug_messenger(instance, dldi);
	}
	VkSurfaceKHR c_style_Surface;
	if(glfwCreateWindowSurface(instance, window, nullptr, &c_style_Surface) != VK_SUCCESS)
	{
		if (debugMode)
		{
			std::cout << "Failed to abstract the glfw surface for Vulkan" << std::endl;
		}
	}
	surface = c_style_Surface;

}

void Engine::make_debug_messenger()
{
	debugMessenger = vkInit::make_debug_messenger(instance, dldi);
}

void Engine::make_device()
{
	physicalDevice = vkInit::choose_physical_device(instance, debugMode);
	device = vkInit::create_logical_device(physicalDevice, surface, debugMode);
	std::array<vk::Queue,2> queues = vkInit::get_queue(physicalDevice, device, surface, debugMode);
	graphicsQueue = queues[0];
	presentQueue = queues[1];
	vkInit::SwapChainBundle bundle = vkInit::create_swapchain(device, physicalDevice, surface, width, height, debugMode);
	swapchain = bundle.swapchain;
	swapchainFrames = bundle.frames;
	swapchainFormat = bundle.format;
	swapchainExtent = bundle.extent;
	maxFramesInFlight = static_cast<int>(swapchainFrames.size());
	frameNumber = 0;
}

void Engine::make_pipeline()
{
	vkInit::GraphicsPipelineInBundle specification = {};
	specification.device = device;
	specification.vertexFilepath = "shaders/vert.spv";
	specification.fragmentFilepath = "shaders/frag.spv";
	specification.swapchainExtent = swapchainExtent;
	specification.swapchainImageFormat = swapchainFormat;

	vkInit::GraphicsPipelineOutBundle output = vkInit::create_graphics_pipeline(
		specification, debugMode
	);

	pipelineLayout = output.layout;
	renderpass = output.renderpass;
	pipeline = output.pipeline;
}

void Engine::finalize_setup()
{
	vkInit::framebufferInput frameBufferInput;
	frameBufferInput.device = device;
	frameBufferInput.renderpass = renderpass;
	frameBufferInput.swapchainExtent = swapchainExtent;
	vkInit::make_framebuffers(frameBufferInput, swapchainFrames, debugMode);

	commandPool = vkInit::make_command_pool(device, physicalDevice, surface, debugMode);

	vkInit::commandBufferInputChunk commandBufferInput = { device, commandPool, swapchainFrames };
	mainCommandBuffer = vkInit::make_command_buffers(commandBufferInput, debugMode);

	for (vkUtil::SwapChainFrame& frame : swapchainFrames)
	{
		frame.inFlight = vkInit::make_fence(device, debugMode);
		frame.imageAvailable = vkInit::make_semaphore(device, debugMode);
		frame.renderFinished = vkInit::make_semaphore(device, debugMode);
		
	}

}

void Engine::record_draw_commands(vk::CommandBuffer commandBuffer, uint32_t imageIndex, Scene* scene)
{
	vk::CommandBufferBeginInfo beginInfo = {};

	try {
		commandBuffer.begin(beginInfo);
	}
	catch (vk::SystemError err) {
		if (debugMode) {
			std::cout << "Failed to begin recording command buffer!" << std::endl;
		}
	}

	vk::RenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.renderPass = renderpass;
	renderPassInfo.framebuffer = swapchainFrames[imageIndex].framebuffer;
	renderPassInfo.renderArea.offset.x = 0;
	renderPassInfo.renderArea.offset.y = 0;
	renderPassInfo.renderArea.extent = swapchainExtent;

	vk::ClearValue clearColor = { std::array<float, 4>{1.0f, 0.5f, 0.25f, 1.0f} };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	commandBuffer.beginRenderPass(&renderPassInfo, vk::SubpassContents::eInline);

	commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
	for (glm::vec3 position : scene->trianglePositions)
	{
		glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
		vkUtil::ObjectData objectData;
		objectData.model = model;
		commandBuffer.pushConstants(pipelineLayout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(objectData), & objectData);
		commandBuffer.draw(3, 1, 0, 0);
	}

	commandBuffer.endRenderPass();

	try {
		commandBuffer.end();
	}
	catch (vk::SystemError err) {

		if (debugMode) {
			std::cout << "failed to record command buffer!" << std::endl;
		}
	}

}

Engine::~Engine() 
{
	device.waitIdle();
	if (debugMode) {
		std::cout << "Goodbye see you!\n";
	}

	


	device.destroyCommandPool(commandPool);

	device.destroyPipeline(pipeline);
	device.destroyPipelineLayout(pipelineLayout);
	device.destroyRenderPass(renderpass);

	for (vkUtil::SwapChainFrame frame : swapchainFrames) {
		device.destroyImageView(frame.imageView);
		device.destroyFramebuffer(frame.framebuffer);
		device.destroyFence(frame.inFlight);
		device.destroySemaphore(frame.imageAvailable);
		device.destroySemaphore(frame.renderFinished);
	}

	device.destroySwapchainKHR(swapchain);
	device.destroy();

	instance.destroySurfaceKHR(surface);
	if (debugMode) {
		instance.destroyDebugUtilsMessengerEXT(debugMessenger, nullptr, dldi);
	}
	/*
	* from vulkan_funcs.hpp:
	*
	* void Instance::destroy( Optional<const VULKAN_HPP_NAMESPACE::AllocationCallbacks> allocator = nullptr,
											Dispatch const & d = ::vk::getDispatchLoaderStatic())
	*/
	instance.destroy();

	//terminate glfw
	glfwTerminate();
}

void Engine::render(Scene* scene)
{
	device.waitForFences(1, &swapchainFrames[frameNumber].inFlight, VK_TRUE, UINT64_MAX);
	device.resetFences(1, &swapchainFrames[frameNumber].inFlight);

	//acquireNextImageKHR(vk::SwapChainKHR, timeout, semaphore_to_signal, fence)
	uint32_t imageIndex{ device.acquireNextImageKHR(swapchain, UINT64_MAX, swapchainFrames[frameNumber].imageAvailable, nullptr).value };

	vk::CommandBuffer commandBuffer = swapchainFrames[frameNumber].commandBuffer;

	commandBuffer.reset();

	record_draw_commands(commandBuffer, imageIndex, scene);

	vk::SubmitInfo submitInfo = {};

	vk::Semaphore waitSemaphores[] = { swapchainFrames[frameNumber].imageAvailable };
	vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vk::Semaphore signalSemaphores[] = { swapchainFrames[frameNumber].renderFinished };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	try {
		graphicsQueue.submit(submitInfo, swapchainFrames[frameNumber].inFlight);
	}
	catch (vk::SystemError err) {

		if (debugMode) {
			std::cout << "failed to submit draw command buffer!" << std::endl;
		}
	}

	vk::PresentInfoKHR presentInfo = {};
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	vk::SwapchainKHR swapChains[] = { swapchain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;

	presentInfo.pImageIndices = &imageIndex;

	presentQueue.presentKHR(presentInfo);
	frameNumber = (frameNumber + 1) % maxFramesInFlight;
}
