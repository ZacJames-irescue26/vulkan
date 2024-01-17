#pragma once
#include "vulkan/vulkan.hpp"
#include "frame.h"
#include "scene.h"
/*
* including the prebuilt header from the lunarg sdk will load
* most functions, but not all.
*
* Functions can also be dynamically loaded, using the call
*
* PFN_vkVoidFunction vkGetInstanceProcAddr(
	VkInstance                                  instance,
	const char*                                 pName);

 or

 PFN_vkVoidFunction vkGetDeviceProcAddr(
	VkDevice                                    device,
	const char*                                 pName);

	We will look at this later, once we've created an instance and device.
*/

class Engine {

public:

	Engine(int width, int height, GLFWwindow* window, bool debugMode);

	~Engine();

	void render(Scene* scene);

private:

	//whether to print debug messages in functions
	bool debugMode = true;

	//glfw window parameters
	int width;
	int height;
	
	GLFWwindow* window{nullptr};
	// instance related variables
	vk::Instance instance{ nullptr };
	vk::DebugUtilsMessengerEXT debugMessenger{ nullptr };
	vk::DispatchLoaderDynamic dldi;
	vk::SurfaceKHR surface;
	
	// device related variables
	vk::PhysicalDevice physicalDevice{ nullptr };
	vk::Device device{nullptr};
	vk::Queue graphicsQueue{nullptr};
	vk::Queue presentQueue{nullptr};
	vk::SwapchainKHR swapchain;
	std::vector<vkUtil::SwapChainFrame> swapchainFrames;
	vk::Format swapchainFormat;
	vk::Extent2D swapchainExtent;


	//pipeline-related variables
	vk::PipelineLayout pipelineLayout;
	vk::RenderPass renderpass;
	vk::Pipeline pipeline;


	//command related variables
	vk::CommandPool commandPool;
	vk::CommandBuffer mainCommandBuffer;

	int maxFramesInFlight, frameNumber;




	void make_instance();
	/*void make_debug_messenger();*/

	void make_device();
	void make_swapchain();
	void recreate_swapchain();
	
	void make_pipeline();

	void finalize_setup();
	void record_draw_commands(vk::CommandBuffer commandBuffer, uint32_t imageIndex, Scene* scene);
	void make_framebuffers();
	void make_frame_sync_objects();

	void cleanup_swapchain();
};