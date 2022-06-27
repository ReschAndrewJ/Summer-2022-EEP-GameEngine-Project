#ifndef GRAPHICS_ENGINE
#define GRAPHICS_ENGINE

#include "vulkan/vulkan.h"
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include "Graphics_Image.h"
#include "Validation_Layers.h"

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <map>
#include <unordered_set>


#include "image_create_info.h"

class Graphics_Engine
{

	const std::vector<const char*> deviceExtensions = { 
		VK_KHR_SWAPCHAIN_EXTENSION_NAME 
	};
	
	VkDeviceSize imageMemoryAllocationBytes = 1 << 20; 
	// 1<<20 = 1MB

public:
	bool framebufferResized = false;


private:
	std::unordered_map<std::string, image_create_info> imageCreateQueue{}; // sets and maps to detect duplicate entries
	std::unordered_set<std::string> imageDestroyQueue{};
	std::unordered_map<std::string, std::vector<unsigned char>> pushConstantUpdateQueue{};

private:
	std::unordered_map<std::string, Graphics_Image> images{};

	VkImage depthImage;
	VkImageView depthImageView;
	VkDeviceMemory depthImageMemory;

	VkDeviceMemory generalImageMemory;
	/* boundMemory is an ordered map to allow iterating through the existing bindings
	{first: bytes offset, second: size in bytes} */
	std::map<VkDeviceSize, VkDeviceSize> boundMemory{};
	VkDeviceSize imageMemoryAlignment;

	std::unordered_map<std::string, std::pair<VkPipeline, VkPipelineLayout>> pipelines;
	VkSampler textureSampler;
	VkDescriptorSetLayout generalImagesDescriptorSetLayout;

private:
	Validation_Layers vulkan_validation;

	VkInstance vulkanInstance;

	VkDevice logicalDevice;
	VkPhysicalDevice physicalDevice;
	VkQueue graphicsQueue;
	VkQueue presentQueue;

	VkSurfaceKHR surface;

	size_t maxFramesInFlight = 2;
	size_t currentFrame = 0;

	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	std::vector<VkImageView> swapChainImageViews;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;

	VkRenderPass renderPass;
	std::vector<VkFramebuffer> framebuffers;

	std::vector<VkCommandBuffer> drawCommandBuffers;
	VkCommandPool drawCommandPool;
	VkCommandPool helperCommandPool;

	std::vector<VkSemaphore> imageAvailableSemaphores; //GPU->GPU
	std::vector<VkSemaphore> renderFinishedSemaphores; //GPU->GPU
	std::vector<VkFence> inFlightFences; //GPU->CPU
	VkFence singleCommandsFence;


public:
	void startupEngine(GLFWwindow*);
	void shutdownEngine();

public:
	/* adds an image to the createQueue using the image_to_add struct,
	returns the new identifier for the image, invalidates the passed image_create_info */
	std::string queueAddImage(image_create_info&);
	// adds an image identifier to the destroy queue
	void queueDestroyImage(std::string);
	/* queues an image's push constants to be updated
	queuePushConstantsUpdate(std::string imageIdentifier, vector<unsigned char>& data) */
	void queuePushConstantsUpdate(std::string, std::vector<unsigned char>&);


public:
	void createQueuedImages();
	void updatePushConstants();
	void destroyQueuedImages();

public:
	void drawFrame(GLFWwindow*);

private:
	void rebuild(GLFWwindow*);

// HELPERS
	
	void createVulkanInstance();
	std::vector<const char*> getRequiredExtensions();
	
	void setupDevices();
	bool isDeviceSuitable(VkPhysicalDevice);
	bool checkDeviceExtensionSupport(VkPhysicalDevice);

	void createSwapChain(GLFWwindow*);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>&);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>&);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR&, GLFWwindow*);
	void createSwapChainImageViews();

	void createRenderPass();
	void createFramebuffers();

	void createDrawCommandPool();
	void createDrawCommandBuffers();
	void createHelperCommandPool();
	
	void createFencesSemaphores();

	void createDepthImage();
	void setupGeneralImageMemory();

	void createTextureSampler();
	void createDescriptorSetLayout();
	
	/*createVulkanImage(uint width, uint height, VKFormat format, VkImageTiling tiling, 
	VkImageUsageFlags usage, VkImage image) */
	void createVulkanImage(uint32_t, uint32_t, VkFormat, VkImageTiling, VkImageUsageFlags, VkImage&);
	void createVulkanImageView(VkImage, VkImageView&, VkFormat, VkImageAspectFlags);

	void createGraphicsPipeline(Graphics_Image&);

	void createVulkanBuffer(VkDeviceSize, VkBufferUsageFlags, VkMemoryPropertyFlags, VkBuffer&, VkDeviceMemory&);

	void bindImageMemoryAllocation(Graphics_Image&, VkDeviceSize);

	VkCommandBuffer startVulkanImmediateCommands();
	void submitVulkanImmediateCommands(VkCommandBuffer);

	void transitionVulkanImageLayoutCreateFirst(VkImage);
	void transitionVulkanImageLayoutCreateSecond(VkImage);

	void createDescriptors(Graphics_Image&);

	void recordDrawCommandBuffer(uint32_t);

};


#endif // GRAPHICS_ENGINE