#ifndef GRAPHICS_IMAGE
#define GRAPHICS_IMAGE

#include "vulkan/vulkan.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include <memory>
#include <string>
#include <vector>



class Graphics_Image
{
	friend class Graphics_Engine; // Graphics_Engine has full control over its images
private:
	std::string vertShaderPath;
	std::string fragShaderPath;

	VkImage textureImage = nullptr;
	VkImageView textureImageView = nullptr;
	VkDeviceSize textureMemoryOffset = 0;

	std::string graphicsPipeline;

	std::vector<char> pushConstants;
	uint32_t pushConstantsSizeBytes = 0;

	VkDescriptorPool descriptorPool = nullptr;
	std::vector<VkDescriptorSet> descriptorSets;
	

};


#endif // GRAPHICS_IMAGE