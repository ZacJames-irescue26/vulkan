#pragma once


namespace vkInit
{





	vk::PhysicalDevice choose_physical_device(vk::Instance& instance, bool debug);
	void log_device_properties(const vk::PhysicalDevice& device);
	bool checkDeviceExtensionSupport(const vk::PhysicalDevice& device, const std::vector<const char*>& requestedExtensions, const bool& debug);
	bool isSuitable(const vk::PhysicalDevice& device, const bool debug);

	

	vk::Device create_logical_device(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface, bool debug);
	std::array<vk::Queue,2> get_queue(vk::PhysicalDevice physicalDevice, vk::Device device, vk::SurfaceKHR surface, bool debug);

	

}