#include "pch.h"
#include "engine.h"
#include "Vulkan/instance.h"
#include "Vulkan/device.h"
#include "Vulkan/logging.h"
#include "Vulkan/swapchain.h"

Engine::Engine() 
{

	if (debugMode) {
		std::cout << "Making a graphics engine\n";
	}

	build_glfw_window();
	create_instance();
	make_device();
}

void Engine::build_glfw_window() {

	//initialize glfw
	glfwInit();

	//no default rendering client, we'll hook vulkan up
	//to the window later
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	//resizing breaks the swapchain, we'll disable it for now
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	//GLFWwindow* glfwCreateWindow (int width, int height, const char *title, GLFWmonitor *monitor, GLFWwindow *share)
	if (window = glfwCreateWindow(width, height, "ID Tech 12", nullptr, nullptr)) {
		if (debugMode) {
			std::cout << "Successfully made a glfw window called \"ID Tech 12\", width: " << width << ", height: " << height << '\n';
		}
	}
	else {
		if (debugMode) {
			std::cout << "GLFW window creation failed\n";
		}
	}
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
	swapchainFrames = bundle.frames;
	swapchainFormat = bundle.format;
	swapchainExtent = bundle.extent;
}

Engine::~Engine() 
{

	if (debugMode) {
		std::cout << "Goodbye see you!\n";
	}
	for (vkUtil::SwapChainFrame frame : swapchainFrames)
	{
		device.destroyImageView(frame.imageView);
	}


	device.destroySwapchainKHR(swapchain);
	device.destroy();

	instance.destroySurfaceKHR(surface);
	
	
	if (debugMode)
	{
		instance.destroyDebugUtilsMessengerEXT(debugMessenger, nullptr, dldi);
	}
	//terminate glfw
	glfwTerminate();
}