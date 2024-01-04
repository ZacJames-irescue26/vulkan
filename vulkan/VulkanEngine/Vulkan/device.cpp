#include "pch.h"
#include "device.h"
#include "queue_families.h"


vk::PhysicalDevice vkInit::choose_physical_device(vk::Instance& instance, bool debug)
{

	
	if (debug)
	{
		std::cout << "Choosing physical device..." << std::endl;
	}

	/*
		* Choose a suitable physical device from a list of candidates.
		* Note: Physical devices are neither created nor destroyed, they exist
		* independently to the program.
		*/

	/*
	* ResultValueType<std::vector<PhysicalDevice, PhysicalDeviceAllocator>>::type
		Instance::enumeratePhysicalDevices( Dispatch const & d )

	  std::vector<vk::PhysicalDevice> instance.enumeratePhysicalDevices( Dispatch const & d = static/default )
	*/
	std::vector<vk::PhysicalDevice> availableDevices = instance.enumeratePhysicalDevices();

	if (debug) {
		std::cout << "There are " << availableDevices.size() << " physical devices available on this system\n";
	}

	/*
	* check if a suitable device can be found
	*/
	for (vk::PhysicalDevice device : availableDevices) {

		if (debug) {
			log_device_properties(device);
		}
		if (isSuitable(device, debug)) {
			return device;
		}
	}

	return nullptr;

}

void vkInit::log_device_properties(const vk::PhysicalDevice& device) {
	/*
	* void vkGetPhysicalDeviceProperties(
		VkPhysicalDevice                            physicalDevice,
		VkPhysicalDeviceProperties*                 pProperties);
	*/

	vk::PhysicalDeviceProperties properties = device.getProperties();

	/*
	* typedef struct VkPhysicalDeviceProperties {
		uint32_t                            apiVersion;
		uint32_t                            driverVersion;
		uint32_t                            vendorID;
		uint32_t                            deviceID;
		VkPhysicalDeviceType                deviceType;
		char                                deviceName[VK_MAX_PHYSICAL_DEVICE_NAME_SIZE];
		uint8_t                             pipelineCacheUUID[VK_UUID_SIZE];
		VkPhysicalDeviceLimits              limits;
		VkPhysicalDeviceSparseProperties    sparseProperties;
		} VkPhysicalDeviceProperties;
	*/

	std::cout << "Device name: " << properties.deviceName << '\n';

	std::cout << "Device type: ";
	switch (properties.deviceType) {

	case (vk::PhysicalDeviceType::eCpu):
		std::cout << "CPU\n";
		break;

	case (vk::PhysicalDeviceType::eDiscreteGpu):
		std::cout << "Discrete GPU\n";
		break;

	case (vk::PhysicalDeviceType::eIntegratedGpu):
		std::cout << "Integrated GPU\n";
		break;

	case (vk::PhysicalDeviceType::eVirtualGpu):
		std::cout << "Virtual GPU\n";
		break;

	default:
		std::cout << "Other\n";
	}
}

/**
	Check whether the physical device can support the given extensions.

	\param device the physical device to check
	\param requestedExtensions a list of extension names to check against
	\param debug whether the system is running in debug mode
	\returns whether all of the extensions are requested
*/
bool vkInit::checkDeviceExtensionSupport(
	const vk::PhysicalDevice& device,
	const std::vector<const char*>& requestedExtensions,
	const bool& debug
) {

	/*
	* Check if a given physical device can satisfy a list of requested device
	* extensions.
	*/

	std::set<std::string> requiredExtensions(requestedExtensions.begin(), requestedExtensions.end());

	if (debug) {
		std::cout << "Device can support extensions:\n";
	}

	for (vk::ExtensionProperties& extension : device.enumerateDeviceExtensionProperties()) {

		if (debug) {
			std::cout << "\t\"" << extension.extensionName << "\"\n";
		}

		//remove this from the list of required extensions (set checks for equality automatically)
		requiredExtensions.erase(extension.extensionName);
	}

	//if the set is empty then all requirements have been satisfied
	return requiredExtensions.empty();
}

/**
	Check whether the given physical device is suitable for the system.

	\param device the physical device to check.
	\debug whether the system is running in debug mode.
	\returns whether the device is suitable.
*/
bool vkInit::isSuitable(const vk::PhysicalDevice& device, const bool debug) {

	if (debug) {
		std::cout << "Checking if device is suitable\n";
	}

	/*
	* A device is suitable if it can present to the screen, ie support
	* the swapchain extension
	*/
	const std::vector<const char*> requestedExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	if (debug) {
		std::cout << "We are requesting device extensions:\n";

		for (const char* extension : requestedExtensions) {
			std::cout << "\t\"" << extension << "\"\n";
		}

	}

	if (bool extensionsSupported = checkDeviceExtensionSupport(device, requestedExtensions, debug)) {

		if (debug) {
			std::cout << "Device can support the requested extensions!\n";
		}
	}
	else {

		if (debug) {
			std::cout << "Device can't support the requested extensions!\n";
		}

		return false;
	}
	return true;
}




vk::Device vkInit::create_logical_device(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface, bool debug)
{
	vkUtil::QueueFamilyIndices indices = vkUtil::findQueueFamilies(physicalDevice, surface, debug);
	std::vector<uint32_t> uniqueIndices;
	uniqueIndices.push_back(indices.graphicsFamily.value());
	if (indices.graphicsFamily.value() != indices.presentFamily.value())
	{
		uniqueIndices.push_back(indices.presentFamily.value());
	}
	float queuePriority = 1.0f;
	
	std::vector<vk::DeviceQueueCreateInfo> queueCreateInfo;
	for (uint32_t queueFamilyIndex : uniqueIndices)
	{
		queueCreateInfo.push_back(vk::DeviceQueueCreateInfo(
			vk::DeviceQueueCreateFlags(),queueFamilyIndex,
			1, &queuePriority
		));

	}

	std::vector<const char*> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};


	vk::PhysicalDeviceFeatures deviceFeatures = vk::PhysicalDeviceFeatures();
	std::vector<const char*> enabledLayers;
	if (debug)
	{
		enabledLayers.push_back("VK_LAYER_KHRONOS_validation");
	}
	vk::DeviceCreateInfo deviceInfo = vk::DeviceCreateInfo(
		vk::DeviceCreateFlags(), queueCreateInfo.size(),queueCreateInfo.data(), enabledLayers.size(), enabledLayers.data(),
		deviceExtensions.size(), deviceExtensions.data(), &deviceFeatures
	);

	try {
		vk::Device device = physicalDevice.createDevice(deviceInfo);
		if (debug)
		{
			std::cout << "GPU has been successfully abstracted! " << std::endl;
		}
		return device;
	}
	catch (vk::SystemError err)
	{
		if (debug)
		{
			std::cout << "Device creation failed! " <<std::endl;
		}
	}
	return nullptr;
}

std::array<vk::Queue, 2> vkInit::get_queue(vk::PhysicalDevice physicalDevice, vk::Device device, vk::SurfaceKHR surface, bool debug)
{
	vkUtil::QueueFamilyIndices indices = vkUtil::findQueueFamilies(physicalDevice, surface, debug);

	return {{device.getQueue(indices.graphicsFamily.value(), 0),
			 device.getQueue(indices.presentFamily.value(),  0)}};
}

