#pragma once

namespace vkUtil
{
	/**
		Read a file.

		\param filename a string representing the path to the file
		\param debug whether the system is running in debug mode
		\returns the contents as a vector of raw binary characters
	*/
	std::vector<char> readFile(std::string filename, bool debug);

	vk::ShaderModule createModule(std::string filename, vk::Device device, bool debug);
}
