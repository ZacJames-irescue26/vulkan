#pragma once


namespace vkInit
{

	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool isComplete()
		{
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};

	vk::PhysicalDevice choose_physical_device(vk::Instance& instance, bool debug);
	void log_device_properties(const vk::PhysicalDevice& device);
	bool checkDeviceExtensionSupport(const vk::PhysicalDevice& device, const std::vector<const char*>& requestedExtensions, const bool& debug);
	bool isSuitable(const vk::PhysicalDevice& device, const bool debug);

	QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice device, bool debug);

	vk::Device create_logical_device(vk::PhysicalDevice physicalDevice, bool debug);
}