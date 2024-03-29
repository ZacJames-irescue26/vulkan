#pragma once

namespace vkInit{
	
	bool supported(std::vector<const char*>& extensions, std::vector<const char*>& layers, bool debug);

	vk::Instance make_instance(bool debug, const char* applicationName);
	
}