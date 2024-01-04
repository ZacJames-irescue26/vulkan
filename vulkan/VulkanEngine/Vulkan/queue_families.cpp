#include "pch.h"
#include "queue_families.h"

vkUtil::QueueFamilyIndices vkUtil::findQueueFamilies(vk::PhysicalDevice device, vk::SurfaceKHR surface, bool debug)
{
	vkUtil::QueueFamilyIndices indices;
	std::vector<vk::QueueFamilyProperties> queueFamilies = device.getQueueFamilyProperties();

	if (debug)
	{
		std::cout << "System can support " << queueFamilies.size() << " queue families" << std::endl;
	}

	int i = 0;
	for (vk::QueueFamilyProperties queueFamily : queueFamilies)
	{
		if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
		{
			indices.graphicsFamily = i;


		}
		if (device.getSurfaceSupportKHR(i, surface))
		{
			indices.presentFamily = i;

		}
		if (indices.isComplete())
		{
			break;
		}
		i++;
	}

	return indices;

}
