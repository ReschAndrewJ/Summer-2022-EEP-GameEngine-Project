#include "Graphics_Engine.h"

#include <set>
#include <algorithm>
#include <array>
#include <fstream>
#include <optional>
#include <random>


////- STARTUP FUNCTION

void Graphics_Engine::startupEngine(GLFWwindow* window) {
	// create vulkan instance
	createVulkanInstance();
	// setup validation layers
	if (Validation_Layers::enableValidationLayers) {
		if (!Validation_Layers::checkValidationLayersSupport()) {
			std::string err = "tried to set up validation layers, but missing validation layer support\n";
			printf(err.c_str());
		}
		else {
			vulkan_validation.setupDebugMessenger(vulkanInstance);
		}
	}
	
	// create draw surface
	VkResult result = glfwCreateWindowSurface(vulkanInstance, window, nullptr, &surface);
	if (result != VK_SUCCESS) {
		std::string err = "failed to create window surface: VKResult " + std::to_string((int)result) + "\n";
		printf(err.c_str());
	}

	// setup logical device
	setupDevices();

	// create swapChain
	createSwapChain(window);
	createSwapChainImageViews();

	// create render pass
	createRenderPass();
	// create depth image
	createDepthImage();
	// create framebuffers
	createFramebuffers();

	// create draw command pool & buffers
	createDrawCommandPool();
	createDrawCommandBuffers();

	// create helper buffers command pool
	createHelperCommandPool();

	// create fences & semaphores
	createFencesSemaphores();

	// create texture sampler
	createTextureSampler();

	// create descriptor set layout for general images pipeline
	createDescriptorSetLayout();

	// setup images memory
	setupGeneralImageMemory();
}

//-

void Graphics_Engine::createVulkanInstance() {
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "";
	appInfo.applicationVersion = VK_MAKE_API_VERSION(0,0,0,0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_API_VERSION(0,0,0,0);
	appInfo.apiVersion = VK_API_VERSION_1_3;

	VkInstanceCreateInfo instCreateInfo{};
	instCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instCreateInfo.pApplicationInfo = &appInfo;

	auto extensions = getRequiredExtensions();
	instCreateInfo.enabledExtensionCount = (uint32_t)extensions.size();
	instCreateInfo.ppEnabledExtensionNames = extensions.data();

	VkDebugUtilsMessengerCreateInfoEXT preDebugCreateInfo{};
	if (Validation_Layers::enableValidationLayers && Validation_Layers::checkValidationLayersSupport()) {
		instCreateInfo.enabledLayerCount = (uint32_t)Validation_Layers::validationLayers.size();
		instCreateInfo.ppEnabledLayerNames = Validation_Layers::validationLayers.data();
		Validation_Layers::populateDebugMessengerCreateInfo(preDebugCreateInfo);
		instCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&preDebugCreateInfo;
	}
	else {
		instCreateInfo.enabledLayerCount = 0;
		instCreateInfo.pNext = nullptr;
	}

	VkResult result = vkCreateInstance(&instCreateInfo, nullptr, &vulkanInstance);
	if (result != VK_SUCCESS) {
		std::string err = "failed to create Vulkan instance, VkResult: " + std::to_string((int)result);
		throw std::runtime_error(err);
	}
}

std::vector<const char*> Graphics_Engine::getRequiredExtensions() {
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
	if (Validation_Layers::enableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}
	return extensions;
}

//-

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamilyIndex;
	uint32_t graphicsFamilyQueueCount = 0;
	std::optional<uint32_t> presentFamilyIndex;
	bool isComplete() {
		return graphicsFamilyIndex.has_value()
			&& presentFamilyIndex.has_value();
	}
	static QueueFamilyIndices queryDeviceIndices(VkPhysicalDevice, VkSurfaceKHR);
};

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities{};
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
	static SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice, VkSurfaceKHR);
};

//-

void Graphics_Engine::setupDevices() {
	uint32_t physicalDeviceCount = 0;
	vkEnumeratePhysicalDevices(vulkanInstance, &physicalDeviceCount, nullptr);
	if (physicalDeviceCount == 0) {
		throw std::runtime_error("failed to find physical devices with Vulkan");
	}
	std::vector<VkPhysicalDevice> availableDevices(physicalDeviceCount);
	vkEnumeratePhysicalDevices(vulkanInstance, &physicalDeviceCount, availableDevices.data());
	QueueFamilyIndices deviceQueueFamilyIndices;
	for (const auto& device : availableDevices) {
		if (isDeviceSuitable(device)) {
			physicalDevice = device;
			deviceQueueFamilyIndices = QueueFamilyIndices::queryDeviceIndices(device, surface);
			break;
		}
	}

	if (physicalDevice == nullptr) {
		throw std::runtime_error("failed to find a suitable physical device");
	}

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = {
		deviceQueueFamilyIndices.graphicsFamilyIndex.value(),
		deviceQueueFamilyIndices.presentFamilyIndex.value()
	};

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};
	deviceFeatures.samplerAnisotropy = VK_TRUE; // allow anistropic filtering

	VkDeviceCreateInfo logicalDeviceCreateInfo{};

	logicalDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	logicalDeviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	logicalDeviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
	logicalDeviceCreateInfo.pEnabledFeatures = &deviceFeatures;
	logicalDeviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	logicalDeviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if (Validation_Layers::enableValidationLayers) {
		logicalDeviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(Validation_Layers::validationLayers.size());
		logicalDeviceCreateInfo.ppEnabledLayerNames = Validation_Layers::validationLayers.data();
	}
	else {
		logicalDeviceCreateInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(physicalDevice, &logicalDeviceCreateInfo, nullptr, &logicalDevice) != VK_SUCCESS) {
		throw std::runtime_error("failed to create logical device");
	}

	vkGetDeviceQueue(logicalDevice, deviceQueueFamilyIndices.graphicsFamilyIndex.value(), 0, &graphicsQueue);
	vkGetDeviceQueue(logicalDevice, deviceQueueFamilyIndices.presentFamilyIndex.value(), 0, &presentQueue);
	//if (deviceQueueFamilyIndices.graphicsFamilyQueueCount > 0)
}

bool Graphics_Engine::isDeviceSuitable(VkPhysicalDevice device) {
	QueueFamilyIndices indices = QueueFamilyIndices::queryDeviceIndices(device, surface);
	bool extensionsSupported = checkDeviceExtensionSupport(device);

	bool swapChainAdequate = false;
	if (extensionsSupported) {
		SwapChainSupportDetails swapChainSupport = SwapChainSupportDetails::querySwapChainSupport(device, surface);
		swapChainAdequate = !swapChainSupport.formats.empty()
			&& !swapChainSupport.presentModes.empty();
	}

	return indices.isComplete() && swapChainAdequate;
}

bool Graphics_Engine::checkDeviceExtensionSupport(VkPhysicalDevice device) {
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}
	return requiredExtensions.empty();
}

//-

QueueFamilyIndices QueueFamilyIndices::queryDeviceIndices(VkPhysicalDevice device, VkSurfaceKHR drawSurface) {
	QueueFamilyIndices indices;
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT
			&& queueFamily.queueCount > indices.graphicsFamilyQueueCount) {
			indices.graphicsFamilyIndex = i;
			indices.graphicsFamilyQueueCount = queueFamily.queueCount;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, drawSurface, &presentSupport);

		if (presentSupport) { indices.presentFamilyIndex = i; }
		if (indices.isComplete()) { break; }

		i++;
	}

	return indices;
}

SwapChainSupportDetails SwapChainSupportDetails::querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR drawSurface) {
	SwapChainSupportDetails details;
	uint32_t formatCount;
	uint32_t presentModeCount;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, drawSurface, &details.capabilities);
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, drawSurface, &formatCount, nullptr);
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, drawSurface, &presentModeCount, nullptr);

	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, drawSurface, &formatCount, details.formats.data());
	}
	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, drawSurface, &presentModeCount, details.presentModes.data());
	}

	return details;
}

//-

void Graphics_Engine::createSwapChain(GLFWwindow* window) {
	SwapChainSupportDetails swapSupport = SwapChainSupportDetails::querySwapChainSupport(physicalDevice, surface);
	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapSupport.presentModes);
	VkExtent2D extent = chooseSwapExtent(swapSupport.capabilities, window);

	uint32_t imageCount = std::clamp(swapSupport.capabilities.minImageCount + 1,
		swapSupport.capabilities.minImageCount,
		swapSupport.capabilities.maxImageCount);

	VkSwapchainCreateInfoKHR swapCreateInfo{};
	swapCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapCreateInfo.surface = surface;
	swapCreateInfo.minImageCount = imageCount;
	swapCreateInfo.imageFormat = surfaceFormat.format;
	swapCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
	swapCreateInfo.imageExtent = extent;
	swapCreateInfo.imageArrayLayers = 1;
	swapCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	
	QueueFamilyIndices familyIndices = QueueFamilyIndices::queryDeviceIndices(physicalDevice, surface);
	if (familyIndices.graphicsFamilyIndex != familyIndices.presentFamilyIndex) {
		uint32_t queueFamilyIndices[] = {
			familyIndices.graphicsFamilyIndex.value(),
			familyIndices.presentFamilyIndex.value()
		};

		swapCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		swapCreateInfo.queueFamilyIndexCount = 2;
		swapCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		swapCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	swapCreateInfo.preTransform = swapSupport.capabilities.currentTransform;
	swapCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapCreateInfo.presentMode = presentMode;
	swapCreateInfo.clipped = VK_TRUE;

	if (vkCreateSwapchainKHR(logicalDevice, &swapCreateInfo, nullptr, &swapChain) != VK_SUCCESS) {
		throw std::runtime_error("failed to create swapchain");
	}

	vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, nullptr);
	swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, swapChainImages.data());

	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;
}

VkSurfaceFormatKHR Graphics_Engine::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats) {
	for (const auto& availableFormat : formats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
			availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}
	return formats[0];
}

VkPresentModeKHR Graphics_Engine::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& presentModes) {
	for (const auto& availableMode : presentModes) {
		if (availableMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availableMode;
		}
	}
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Graphics_Engine::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window) {
	if (capabilities.currentExtent.width != UINT32_MAX) {
		return capabilities.currentExtent;
	}
	else {
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		VkExtent2D actualExtent {};
		actualExtent.width = width;
		actualExtent.height = height;
		
		actualExtent.width = std::clamp(actualExtent.width,
			capabilities.minImageExtent.width,
			capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height,
			capabilities.minImageExtent.height,
			capabilities.maxImageExtent.height);

		return actualExtent;
	}
}

//-

void Graphics_Engine::createSwapChainImageViews() {
	swapChainImageViews.resize(swapChainImages.size());

	for (size_t i = 0; i < swapChainImages.size(); i++) {
		createVulkanImageView(swapChainImages[i], swapChainImageViews[i], swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
	}
}

//-

void Graphics_Engine::createRenderPass() {
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = swapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription depthAttachment{};
	depthAttachment.format = VK_FORMAT_D32_SFLOAT;
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef{};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;

	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

	std::array<VkAttachmentDescription, 2> attachmentDescriptions = {colorAttachment, depthAttachment};
	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size());
	renderPassInfo.pAttachments = attachmentDescriptions.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(logicalDevice, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
		std::string err = "failed to create render pass\n";
		printf(err.c_str());
	}
}

//-

void Graphics_Engine::createFramebuffers() {
	framebuffers.resize(swapChainImageViews.size());
	for (size_t i = 0; i < swapChainImageViews.size(); i++) {
		std::array<VkImageView, 2> attachments = { swapChainImageViews[i], depthImageView };

		VkFramebufferCreateInfo frameBufferInfo{};
		frameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		frameBufferInfo.renderPass = renderPass;
		frameBufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		frameBufferInfo.pAttachments = attachments.data();
		frameBufferInfo.width = swapChainExtent.width;
		frameBufferInfo.height = swapChainExtent.height;
		frameBufferInfo.layers = 1;

		if (vkCreateFramebuffer(logicalDevice, &frameBufferInfo, nullptr, &framebuffers[i]) != VK_SUCCESS) {
			std::string err = "failed to create framebuffer\n";
			printf(err.c_str());
			return;
		}
	}
}

//-

void Graphics_Engine::createDrawCommandPool() {
	QueueFamilyIndices indices = QueueFamilyIndices::queryDeviceIndices(physicalDevice, surface);

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = indices.graphicsFamilyIndex.value();
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	if (vkCreateCommandPool(logicalDevice, &poolInfo, nullptr, &drawCommandPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create draw command pool");
	}

}

void Graphics_Engine::createDrawCommandBuffers() {
	drawCommandBuffers.resize(framebuffers.size());

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = drawCommandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)drawCommandBuffers.size();

	if (vkAllocateCommandBuffers(logicalDevice, &allocInfo, drawCommandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers");
	}
}

//-

void Graphics_Engine::createHelperCommandPool() {
	QueueFamilyIndices indices = QueueFamilyIndices::queryDeviceIndices(physicalDevice, surface);

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = indices.graphicsFamilyIndex.value();
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	VkResult result = vkCreateCommandPool(logicalDevice, &poolInfo, nullptr, &helperCommandPool);
	if (result != VK_SUCCESS) {
		std::string err = "failed to create helper command pool: " + std::to_string((int)result);
		throw std::runtime_error(err);
	}
}

//-

void Graphics_Engine::createFencesSemaphores() {
	imageAvailableSemaphores.resize(maxFramesInFlight);
	renderFinishedSemaphores.resize(maxFramesInFlight);
	inFlightFences.resize(maxFramesInFlight);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < maxFramesInFlight; i++) {
		if (vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(logicalDevice, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create synchronization objects");
		}
	}
	VkResult result = vkCreateFence(logicalDevice, &fenceInfo, nullptr, &singleCommandsFence);
	if (result != VK_SUCCESS) {
		std::string err = "failed to create single commands fence, VkResult: " + std::to_string((int)result);
		throw std::runtime_error(err);
	}
}

//-

void Graphics_Engine::createVulkanImage(uint32_t width, uint32_t height, VkFormat format,
	VkImageTiling tiling, VkImageUsageFlags usage, VkImage& image) {

	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format;
	imageInfo.tiling = tiling;
	imageInfo.usage = usage;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VkResult result = vkCreateImage(logicalDevice, &imageInfo, nullptr, &image);
	if (result != VK_SUCCESS) {
		std::string err = "failed to create image, VkResult: " + std::to_string((int)result) + "\n";
		printf(err.c_str());
		return;
	}
}

void Graphics_Engine::createVulkanImageView(VkImage image, VkImageView& imageView, 
	VkFormat format, VkImageAspectFlags aspectFlags) {
	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = aspectFlags;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	VkResult result = vkCreateImageView(logicalDevice, &viewInfo, nullptr, &imageView);
	if (result != VK_SUCCESS) {
		std::string err = "failed to create texture image view, VkResult: " + std::to_string((int)result) + "\n";
		printf(err.c_str());
	}
}

//-

void Graphics_Engine::createDepthImage() {
	createVulkanImage(swapChainExtent.width, swapChainExtent.height, VK_FORMAT_D32_SFLOAT,
		VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		depthImage);
	
	// allocate memory
	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(logicalDevice, depthImage, &memRequirements);

	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
	uint32_t memTypeIndex = -1;
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i) {
		if (memRequirements.memoryTypeBits & (1 << i)) {
			memTypeIndex = i;
			break;
		}
	}
	if (memTypeIndex == -1) throw std::runtime_error("failed to find suitable memory type for depth image");

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.pNext = nullptr;
	allocInfo.memoryTypeIndex = memTypeIndex;
	
	vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &depthImageMemory);
	vkBindImageMemory(logicalDevice, depthImage, depthImageMemory, 0);

	createVulkanImageView(depthImage, depthImageView, VK_FORMAT_D32_SFLOAT, VK_IMAGE_ASPECT_DEPTH_BIT);
}

//-

void Graphics_Engine::createTextureSampler() {
	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_NEAREST; // nearest = no oversample filtering (imageRes > screenRes)
	samplerInfo.minFilter = VK_FILTER_NEAREST;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

	/*
	VkPhysicalDeviceFeatures features;
	vkGetPhysicalDeviceFeatures(physicalDevice, &features);
	if (features.samplerAnisotropy) {
		samplerInfo.anisotropyEnable = VK_TRUE; // true = enable undersample filtering (screenRes > imageRes)
		VkPhysicalDeviceProperties properties;
		vkGetPhysicalDeviceProperties(physicalDevice, &properties);
		samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy; // max undersample filtering, 
																			//lower value = better performance
	}*/
	//else {
		samplerInfo.anisotropyEnable = VK_FALSE;
		samplerInfo.maxAnisotropy = 1;
	//}

	samplerInfo.borderColor = VK_BORDER_COLOR_INT_TRANSPARENT_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE; // false = [0,1) texture coordinates
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST; // mipmap filter
	samplerInfo.mipLodBias = 0;
	samplerInfo.minLod = 0;
	samplerInfo.maxLod = 0;

	vkCreateSampler(logicalDevice, &samplerInfo, nullptr, &textureSampler);
}

//-

void Graphics_Engine::createDescriptorSetLayout() {
	VkDescriptorSetLayoutBinding samplerBinding{};
	samplerBinding.binding = 0;
	samplerBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerBinding.descriptorCount = 1;
	samplerBinding.pImmutableSamplers = nullptr;
	samplerBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	
	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &samplerBinding;
	layoutInfo.pNext = nullptr;

	VkResult result = vkCreateDescriptorSetLayout(logicalDevice, &layoutInfo, nullptr, &generalImagesDescriptorSetLayout);
	if (result != VK_SUCCESS) {
		std::string err = "failed to create descriptor set layput, VkResult: " + std::to_string((int)result);
		throw std::runtime_error(err);
	}
}

//-

void Graphics_Engine::setupGeneralImageMemory() {
	VkImage setupImage;
	createVulkanImage(1, 1, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, setupImage);

	VkMemoryRequirements imageMemRequirements;
	vkGetImageMemoryRequirements(logicalDevice, setupImage, &imageMemRequirements);
	imageMemoryAlignment = imageMemRequirements.alignment;
	
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
	uint32_t memTypeIndex = -1;
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i) {
		if (imageMemRequirements.memoryTypeBits & (1 << i)) {
			memTypeIndex = i;
			break;
		}
	}
	if (memTypeIndex == -1) throw std::runtime_error("failed to find suitable memory type for general images setup");

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.pNext = nullptr;
	allocInfo.allocationSize = imageMemoryAllocationBytes;
	allocInfo.memoryTypeIndex = memTypeIndex;
	
	vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &generalImageMemory);
	vkDestroyImage(logicalDevice, setupImage, nullptr);
}

////- SHUTDOWN FUNCTION

void Graphics_Engine::shutdownEngine() {
	// wait for the device to finsh any current operations
	vkDeviceWaitIdle(logicalDevice);

	// destroy images
	for (auto& img : images) {
		vkDestroyImage(logicalDevice, img.second.textureImage, nullptr);
		vkDestroyDescriptorPool(logicalDevice, img.second.descriptorPool, nullptr);
		vkDestroyImageView(logicalDevice, img.second.textureImageView, nullptr);
		boundMemory.erase(img.second.textureMemoryOffset);
	}
	images.clear();
	for (auto& img : imagesToDestroy) {
		vkDestroyImage(logicalDevice, img.second.first.textureImage, nullptr);
		vkDestroyDescriptorPool(logicalDevice, img.second.first.descriptorPool, nullptr);
		vkDestroyImageView(logicalDevice, img.second.first.textureImageView, nullptr);
	}
	imagesToDestroy.clear();

	// free images memory
	vkFreeMemory(logicalDevice, generalImageMemory, nullptr);
	
	// destroy pipelines
	for (auto& pipe : pipelines) {
		vkDestroyPipeline(logicalDevice, pipe.second.first, nullptr);
		vkDestroyPipelineLayout(logicalDevice, pipe.second.second, nullptr);
	}
	pipelines.clear();

	// destroy descriptor set layout
	vkDestroyDescriptorSetLayout(logicalDevice, generalImagesDescriptorSetLayout, nullptr);

	// destroy texture sampler
	vkDestroySampler(logicalDevice, textureSampler, nullptr);

	// destroy fences & semaphores
	for (size_t i = 0; i < maxFramesInFlight; ++i) {
		vkDestroySemaphore(logicalDevice, imageAvailableSemaphores[i], nullptr);
		vkDestroySemaphore(logicalDevice, renderFinishedSemaphores[i], nullptr);
		vkDestroyFence(logicalDevice, inFlightFences[i], nullptr);
	}
	vkDestroyFence(logicalDevice, singleCommandsFence, nullptr);

	// free draw command pool
	vkDestroyCommandPool(logicalDevice, drawCommandPool, nullptr);

	// free helper command pool
	vkDestroyCommandPool(logicalDevice, helperCommandPool, nullptr);

	// destroy framebuffers
	for (auto framebuffer : framebuffers) {
		vkDestroyFramebuffer(logicalDevice, framebuffer, nullptr);
	}

	// destroy depthimage
	vkDestroyImageView(logicalDevice, depthImageView, nullptr);
	vkDestroyImage(logicalDevice, depthImage, nullptr);
	vkFreeMemory(logicalDevice, depthImageMemory, nullptr);
	// destroy renderpass
	vkDestroyRenderPass(logicalDevice, renderPass, nullptr);

	// destroy swapchain
	for (auto swapImageView : swapChainImageViews) {
		vkDestroyImageView(logicalDevice, swapImageView, nullptr);
	}
	vkDestroySwapchainKHR(logicalDevice, swapChain, nullptr);

	// cleanup devices
	vkDestroyDevice(logicalDevice, nullptr);

	// destroy draw surface
	vkDestroySurfaceKHR(vulkanInstance, surface, nullptr);

	// destroy validation layers
	vulkan_validation.shutdownDebugMessenger(vulkanInstance);
	
	//shutdown vulkan
	vkDestroyInstance(vulkanInstance, nullptr);
}

////- WINDOW RESIZE

void Graphics_Engine::rebuild(GLFWwindow* window) {
	// wait for the device to finish any current operations
	vkDeviceWaitIdle(logicalDevice);

	// destroy framebuffers
	for (size_t i = 0; i < framebuffers.size(); ++i) {
		vkDestroyFramebuffer(logicalDevice, framebuffers[i], nullptr);
	}

	// destroy depth image
	vkDestroyImageView(logicalDevice, depthImageView, nullptr);
	vkDestroyImage(logicalDevice, depthImage, nullptr);
	vkFreeMemory(logicalDevice, depthImageMemory, nullptr);
	
	// destroy images graphics pipelines
	for (auto& pipe : pipelines) {
		vkDestroyPipeline(logicalDevice, pipe.second.first, nullptr);
		vkDestroyPipelineLayout(logicalDevice, pipe.second.second, nullptr);
	}
	pipelines.clear();

	// destroy render pass
	vkDestroyRenderPass(logicalDevice, renderPass, nullptr);

	// destroy swapchain
	for (size_t i = 0; i < swapChainImageViews.size(); ++i) {
		vkDestroyImageView(logicalDevice, swapChainImageViews[i], nullptr);
	}
	vkDestroySwapchainKHR(logicalDevice, swapChain, nullptr);
	

	// recreate swapchain
	createSwapChain(window);
	createSwapChainImageViews();

	// recreate renderpass
	createRenderPass();

	// recreate graphics pipelines
	for (auto& img : images) {
		if (!pipelines.count(img.second.graphicsPipeline)) {
			createGraphicsPipeline(img.second);
		}
	}

	// recreate depth image
	createDepthImage();

	// recreate framebuffers
	createFramebuffers();
}

////- GRAPHICS_IMAGE FUNCTIONS

void Graphics_Engine::createGraphicsPipeline(Graphics_Image& buildImage) {
	
	VkShaderModule vertShaderModule;
	VkShaderModule fragShaderModule;
	
	std::ifstream vertFile;
	vertFile.open(buildImage.vertShaderPath, std::ios::ate | std::ios::binary);
	if (!vertFile.is_open()) {
		std::string err = "failed to open vertex shader file: " + buildImage.vertShaderPath + "\n";
		printf(err.c_str());
		return;
	}
	size_t filesize = (size_t)vertFile.tellg();
	std::vector<char> buffer(filesize);

	vertFile.seekg(0);
	vertFile.read(buffer.data(), filesize);
	vertFile.close();

	VkShaderModuleCreateInfo vertShaderInfo{};
	vertShaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	vertShaderInfo.codeSize = filesize;
	vertShaderInfo.pCode = (uint32_t*)buffer.data();

	if (vkCreateShaderModule(logicalDevice, &vertShaderInfo, nullptr, &vertShaderModule) != VK_SUCCESS) {
		throw std::runtime_error("failed to create vertex shader: " + buildImage.vertShaderPath);
	}

	std::ifstream fragFile;
	fragFile.open(buildImage.fragShaderPath, std::ios::ate | std::ios::binary);
	if (!fragFile.is_open()) throw std::runtime_error("failed to open fragment shader file: " + buildImage.fragShaderPath);

	filesize = (size_t)fragFile.tellg();
	buffer = std::vector<char>(filesize);

	fragFile.seekg(0);
	fragFile.read(buffer.data(), filesize);
	fragFile.close();

	VkShaderModuleCreateInfo fragShaderInfo{};
	fragShaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	fragShaderInfo.codeSize = filesize;
	fragShaderInfo.pCode = (uint32_t*)buffer.data();

	if (vkCreateShaderModule(logicalDevice, &fragShaderInfo, nullptr, &fragShaderModule) != VK_SUCCESS) {
		throw std::runtime_error("failed to create fragment shader: " + buildImage.fragShaderPath);
	}

	VkPipelineShaderStageCreateInfo vertStageInfo{};
	vertStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertStageInfo.module = vertShaderModule;
	vertStageInfo.pName = "main"; // the name of the function in the shader

	VkPipelineShaderStageCreateInfo fragStageInfo{};
	fragStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragStageInfo.module = fragShaderModule;
	fragStageInfo.pName = "main";
	
	VkPipelineShaderStageCreateInfo shaderStageInfos[2] = {vertStageInfo, fragStageInfo};
	

	// input info
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 0;
	vertexInputInfo.vertexAttributeDescriptionCount = 0;

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
	inputAssembly.primitiveRestartEnable = false;

	// viewport & scissors
	VkViewport viewport{};
	viewport.x = 0;
	viewport.y = 0;
	viewport.width = (float)swapChainExtent.width;
	viewport.height = (float)swapChainExtent.height;
	viewport.minDepth = 0;
	viewport.maxDepth = 1;

	VkRect2D scissor{};
	scissor.offset = { 0,0 };
	scissor.extent = swapChainExtent;

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	// rasterizer
	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.lineWidth = 1;
	rasterizer.cullMode = VK_CULL_MODE_NONE; // do not cull either face
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;

	// multisampling
	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	// color blend
	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_A_BIT;

	VkPipelineColorBlendStateCreateInfo colorBlendState{};
	colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendState.logicOpEnable = VK_FALSE;
	colorBlendState.attachmentCount = 1;
	colorBlendState.pAttachments = &colorBlendAttachment;

	// depth stencil
	VkPipelineDepthStencilStateCreateInfo depthStencil{};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS; // lower = closer
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.stencilTestEnable = VK_FALSE; // will not be using a stencil component

	// pipeline layout (push constants)
	VkPushConstantRange pushRange{};
	pushRange.offset = 0;
	pushRange.size = buildImage.pushConstantsSizeBytes;
	pushRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkPipelineLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	layoutInfo.pushConstantRangeCount = 1;
	layoutInfo.pPushConstantRanges = &pushRange;
	layoutInfo.setLayoutCount = 1;
	layoutInfo.pSetLayouts = &generalImagesDescriptorSetLayout;

	VkPipelineLayout layout;
	vkCreatePipelineLayout(logicalDevice, &layoutInfo, nullptr, &layout);
	
	// pipeline create info
	
	VkGraphicsPipelineCreateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	info.stageCount = 2;
	info.pStages = shaderStageInfos;

	info.pVertexInputState = &vertexInputInfo;
	info.pInputAssemblyState = &inputAssembly;
	info.pViewportState = &viewportState;
	info.pRasterizationState = &rasterizer;
	info.pMultisampleState = &multisampling;
	info.pColorBlendState = &colorBlendState;
	info.pDepthStencilState = &depthStencil;
		
	info.layout = layout;
	info.renderPass = renderPass;
	info.subpass = 0;

	// create pipeline
	VkPipeline pipe;
	if (vkCreateGraphicsPipelines(logicalDevice, nullptr, 1, &info, nullptr, &pipe) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline");
	}

	// cleanup shaders
	vkDestroyShaderModule(logicalDevice, vertShaderModule, nullptr);
	vkDestroyShaderModule(logicalDevice, fragShaderModule, nullptr);
	
	// store pipeline
	pipelines.insert({ buildImage.graphicsPipeline, {pipe, layout} });
}

//-

std::string Graphics_Engine::queueAddImage(image_create_info& imageInfo) {
	std::string identifier = imageInfo.identifier;
	while ((bool)images.count(identifier) || imageCreateQueue.count(identifier)) {
		// if the identifier is already in use, modify it before adding the image to the queue
		identifier += std::to_string(rand() % 10);
	}

	image_create_info& new_info = imageCreateQueue[identifier];
	new_info.identifier = imageInfo.identifier;
	new_info.graphicsPipeline = imageInfo.graphicsPipeline;
	new_info.vertShaderPath = imageInfo.vertShaderPath;
	new_info.fragShaderPath = imageInfo.fragShaderPath;
	new_info.pushConstantsSizeBytes = imageInfo.pushConstantsSizeBytes;
	new_info.texture_rows = imageInfo.texture_rows;
	new_info.texture_columns = imageInfo.texture_columns;
	new_info.pixels = imageInfo.pixels;
	new_info.initalPushConstantsValues = imageInfo.initalPushConstantsValues;
	
	return identifier;
}

void Graphics_Engine::queueDestroyImage(std::string imageIdentifier) {
	imageDestroyQueue.insert(imageIdentifier);
}

void Graphics_Engine::readyImageDestructionQueue() {
	for (auto& imgIdentifier : imageDestroyQueue) {
		if (!images.count(imgIdentifier)) continue;
		Graphics_Image& image = images[imgIdentifier];
		imagesToDestroy.insert({ imgIdentifier, {image, maxFramesInFlight} });
		boundMemory.erase(image.textureMemoryOffset);
		images.erase(imgIdentifier);
	}
	imageDestroyQueue.clear();
}

void Graphics_Engine::destroyQueuedImages() {
	std::vector<std::string> destroyed;
	for (auto& image : imagesToDestroy) {
		image.second.second -= 1;
		if (image.second.second <= 0) {
			vkDestroyDescriptorPool(logicalDevice, image.second.first.descriptorPool, nullptr);
			vkDestroyImageView(logicalDevice, image.second.first.textureImageView, nullptr);
			vkDestroyImage(logicalDevice, image.second.first.textureImage, nullptr);
			destroyed.push_back(image.first);
		}
	}
	for (auto& img : destroyed) {
		imagesToDestroy.erase(img);
	}
	
	/* ORIGINAL FUNCTION, SPLIT UP INTO TWO FUNCTIONS TO AVOID DESTROYING BOUND DATA BY CREATING A ONE-FRAME DELAY
	for (auto& imgIdentifier : imageDestroyQueue) {
		if (!images.count(imgIdentifier)) continue;
		Graphics_Image& image = images[imgIdentifier];
		vkDestroyDescriptorPool(logicalDevice, image.descriptorPool, nullptr);
		vkDestroyImageView(logicalDevice, image.textureImageView, nullptr);
		vkDestroyImage(logicalDevice, image.textureImage, nullptr);
		boundMemory.erase(image.textureMemoryOffset);
		images.erase(imgIdentifier);
	}
	imageDestroyQueue.clear();
	*/
}

void Graphics_Engine::createQueuedImages() {
	if (imageCreateQueue.size() == 0) return;
	VkBuffer pixelStagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	VkDeviceSize stagingBufferSize = 0;
	std::unordered_map<std::string/*image*/, VkDeviceSize/*offset*/> stagingBufferOffsets;

	for (auto& queuedImg : imageCreateQueue) {
		// create Graphics_Image instance
		auto& image = images.insert({ queuedImg.first, Graphics_Image() }).first->second;

		// fill image values
		image.vertShaderPath = queuedImg.second.vertShaderPath;
		image.fragShaderPath = queuedImg.second.fragShaderPath;
		image.graphicsPipeline = queuedImg.second.graphicsPipeline;
		image.pushConstantsSizeBytes = queuedImg.second.pushConstantsSizeBytes;	
		image.pushConstants = std::move(queuedImg.second.initalPushConstantsValues);

		// create graphics pipeline if it doesn't already exist
		if (!pipelines.count(image.graphicsPipeline)) {
			createGraphicsPipeline(image);
		}

		// create texture image
		createVulkanImage(queuedImg.second.texture_columns, queuedImg.second.texture_rows, VK_FORMAT_R8G8B8A8_SRGB,
			VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, image.textureImage);

		// bind image memory
		VkDeviceSize imgSize = (VkDeviceSize)queuedImg.second.texture_columns * queuedImg.second.texture_rows * 4;
		bindImageMemoryAllocation(image, imgSize);

		// create texture imageView
		createVulkanImageView(image.textureImage, image.textureImageView, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);

		// create descriptor pool & sets
		createDescriptors(image);

		// increase size of staging buffer before buffer creation
		stagingBufferSize += imgSize;
	}
	
	// create staging buffer
	createVulkanBuffer(stagingBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, pixelStagingBuffer, stagingBufferMemory);

	
	VkDeviceSize offset = 0;
	// copy pixel data to staging buffer
	for (auto& queuedImg : imageCreateQueue) {
		VkDeviceSize imgSize = (VkDeviceSize)queuedImg.second.texture_columns * queuedImg.second.texture_rows * 4;
		void* data;
		vkMapMemory(logicalDevice, stagingBufferMemory, offset, imgSize, 0, &data);
		memcpy(data, queuedImg.second.pixels.data(), imgSize);
		vkUnmapMemory(logicalDevice, stagingBufferMemory);
		stagingBufferOffsets[queuedImg.first] = offset;
		offset += imgSize;
	}

	// transition image layouts
	for (auto& queuedImg : imageCreateQueue) {
		transitionVulkanImageLayoutCreateFirst(images[queuedImg.first].textureImage);
	}

	// copy pixel data to images
	VkCommandBuffer imgCopyCmdBuffer = startVulkanImmediateCommands();
	for (auto& imageOffset : stagingBufferOffsets) {
		VkBufferImageCopy region{};
		region.bufferOffset = imageOffset.second;
		region.bufferRowLength = 0; // zero padding
		region.bufferImageHeight = 0; // zero padding
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;
		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = {
			imageCreateQueue[imageOffset.first].texture_columns, // width
			imageCreateQueue[imageOffset.first].texture_rows, // height
			1 // depth
		};

		vkCmdCopyBufferToImage(imgCopyCmdBuffer, pixelStagingBuffer, images[imageOffset.first].textureImage,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
	}
	submitVulkanImmediateCommands(imgCopyCmdBuffer);
	
	// transition image layouts
	for (auto& queuedimg : imageCreateQueue) {
		transitionVulkanImageLayoutCreateSecond(images[queuedimg.first].textureImage);
	}

	// free staging buffer
	vkDestroyBuffer(logicalDevice, pixelStagingBuffer, nullptr);
	vkFreeMemory(logicalDevice, stagingBufferMemory, nullptr);

	// clear image create queue
	imageCreateQueue.clear();
}

//-

void Graphics_Engine::createVulkanBuffer(VkDeviceSize size, VkBufferUsageFlags usage, 
	VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& memory) {

	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VkResult result = vkCreateBuffer(logicalDevice, &bufferInfo, nullptr, &buffer);
	if (result != VK_SUCCESS) {
		std::string err = "failed to create vulkan buffer, VkResult: " + std::to_string((int)result);
		throw std::runtime_error(err);
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(logicalDevice, buffer, &memRequirements);
	
	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = size;
	// buffer alignment correction
	if (memRequirements.alignment > 0) allocInfo.allocationSize += memRequirements.alignment;
	
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
	uint32_t memTypeIndex = -1;
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i) {
		if (memRequirements.memoryTypeBits & (1 << i)
			&& (memProperties.memoryTypes[i].propertyFlags & properties)) {
			memTypeIndex = i;
			break;
		}
	}
	if (memTypeIndex == -1) throw std::runtime_error("failed to find suitable memory type for buffer allocation");
	allocInfo.memoryTypeIndex = memTypeIndex;

	result = vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &memory);
	if (result != VK_SUCCESS) {
		std::string err = "failed to allocate memory for buffer, VkResult: " + std::to_string((int)result);
		throw std::runtime_error(err);
	}

	vkBindBufferMemory(logicalDevice, buffer, memory, 0);
}

//-

void Graphics_Engine::bindImageMemoryAllocation(Graphics_Image& image, VkDeviceSize imageSize) {
	if (imageSize == 0) {
		std::string warning = "attempted to bind image of size zero to memory\n";
		printf(warning.c_str());
		return;
	}
	if (boundMemory.empty() && imageSize <= imageMemoryAllocationBytes) {
		vkBindImageMemory(logicalDevice, image.textureImage, generalImageMemory, 0);
		boundMemory[0] = imageSize;
		image.textureMemoryOffset = 0;
		return;
	}
	else {
		for (auto it = boundMemory.begin(); it != boundMemory.end(); ++it) {
			VkDeviceSize test_pos = it->first + it->second; // offset + size
			// apply image alignment correction
			if (test_pos % imageMemoryAlignment > 0) test_pos += (imageMemoryAlignment - test_pos % imageMemoryAlignment);
			// check if fits
			if ((std::next(it) != boundMemory.end() && std::next(it)->first >= test_pos + imageSize)
				|| (std::next(it) == boundMemory.end() && test_pos + imageSize <= imageMemoryAllocationBytes)) {
				VkResult res = vkBindImageMemory(logicalDevice, image.textureImage, generalImageMemory, test_pos);
				if (res != VK_SUCCESS) {
					std::string err = "failed to bind image memory, VK_Result: " + std::to_string((int)res);
					printf(err.c_str());
					return;
				}
				boundMemory[test_pos] = imageSize;
				image.textureMemoryOffset = test_pos;
				return;
			}
		}
	}
	// not enough allocated memory remaining
	std::string err = "out of memory to allocate image\n";
	printf(err.c_str());
	// potentially add compacting method if there is extra time
}

//- 

VkCommandBuffer Graphics_Engine::startVulkanImmediateCommands() {
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = helperCommandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer cmdBuffer;
	vkAllocateCommandBuffers(logicalDevice, &allocInfo, &cmdBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(cmdBuffer, &beginInfo);
	return cmdBuffer;
}

void Graphics_Engine::submitVulkanImmediateCommands(VkCommandBuffer cmdBuffer) {
	vkEndCommandBuffer(cmdBuffer);

	VkSubmitInfo cmdSubmitInfo{};
	cmdSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	cmdSubmitInfo.commandBufferCount = 1;
	cmdSubmitInfo.pCommandBuffers = &cmdBuffer;

	vkResetFences(logicalDevice, 1, &singleCommandsFence);

	vkQueueSubmit(graphicsQueue, 1, &cmdSubmitInfo, singleCommandsFence);
	vkWaitForFences(logicalDevice, 1, &singleCommandsFence, VK_TRUE, UINT64_MAX);

	vkFreeCommandBuffers(logicalDevice, helperCommandPool, 1, &cmdBuffer);
}

//-

void Graphics_Engine::transitionVulkanImageLayoutCreateFirst(VkImage image) {
	VkCommandBuffer cmdBuffer = startVulkanImmediateCommands();

	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	barrier.srcAccessMask = 0;
	barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

	vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
		0, 0, nullptr, 0, nullptr, 1, &barrier);

	submitVulkanImmediateCommands(cmdBuffer);
}

void Graphics_Engine::transitionVulkanImageLayoutCreateSecond(VkImage image) {
	VkCommandBuffer cmdBuffer = startVulkanImmediateCommands();

	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		0, 0, nullptr, 0, nullptr, 1, &barrier);

	submitVulkanImmediateCommands(cmdBuffer);
}

//-

void Graphics_Engine::createDescriptors(Graphics_Image& image) {
	VkDescriptorPoolSize poolSize{};
	poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSize.descriptorCount = (uint32_t)maxFramesInFlight;

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;
	poolInfo.maxSets = (uint32_t)maxFramesInFlight;
	
	VkResult result = vkCreateDescriptorPool(logicalDevice, &poolInfo, nullptr, &image.descriptorPool);
	if (result != VK_SUCCESS) {
		std::string err = "failed to create descriptor pool, VkResult: " + std::to_string((int)result);
		throw std::runtime_error(err);
	}

	std::vector<VkDescriptorSetLayout> descriptorLayouts(maxFramesInFlight, generalImagesDescriptorSetLayout);
	VkDescriptorSetAllocateInfo setAllocInfo{};
	setAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	setAllocInfo.descriptorPool = image.descriptorPool;
	setAllocInfo.descriptorSetCount = (uint32_t)maxFramesInFlight;
	setAllocInfo.pSetLayouts = descriptorLayouts.data();

	image.descriptorSets.resize(maxFramesInFlight);
	result = vkAllocateDescriptorSets(logicalDevice, &setAllocInfo, image.descriptorSets.data());
	if (result != VK_SUCCESS) {
		std::string err = "failed to allocate descriptor sets, VkResult: " + std::to_string((int)result);
		throw std::runtime_error(err);
	}

	for (size_t i = 0; i < maxFramesInFlight; ++i) {
		VkDescriptorImageInfo descImageInfo{};
		descImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		descImageInfo.imageView = image.textureImageView;
		descImageInfo.sampler = textureSampler;

		VkWriteDescriptorSet descWrite{};
		descWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descWrite.dstSet = image.descriptorSets[i];
		descWrite.dstBinding = 0;
		descWrite.dstArrayElement = 0;
		descWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descWrite.descriptorCount = 1;
		descWrite.pImageInfo = &descImageInfo;

		vkUpdateDescriptorSets(logicalDevice, 1, &descWrite, 0, nullptr);
	}
}

//-

void Graphics_Engine::queuePushConstantsUpdate(std::string imgIdentifier, std::vector<unsigned char>& data) {
	pushConstantUpdateQueue[imgIdentifier] = data;
}

void Graphics_Engine::updatePushConstants() {
	for (auto& push : pushConstantUpdateQueue) {
		if (!images.count(push.first)) continue;
		Graphics_Image& image = images[push.first];

		image.pushConstants = std::move(push.second);
	}
	pushConstantUpdateQueue.clear();
}

////- DRAW

void Graphics_Engine::drawFrame(GLFWwindow* window) {
	vkWaitForFences(logicalDevice, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(logicalDevice, swapChain, 1000000, imageAvailableSemaphores[currentFrame],
		nullptr, &imageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || framebufferResized || result == VK_TIMEOUT) {
		int width, height = 0;
		glfwGetFramebufferSize(window, &width, &height);
		if ((width & height) == 0) return; // skip frame without rebuiling the swapchain, minimized window
		framebufferResized = false;
		rebuild(window);
		return; // rebuild swapchain and skip frame
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		std::string err = "failed to get the swapchain image, VkResult: " + std::to_string((int)result);
		throw std::runtime_error(err);
	}
	vkResetFences(logicalDevice, 1, &inFlightFences[currentFrame]);

	vkResetCommandBuffer(drawCommandBuffers[currentFrame], 0);
	recordDrawCommandBuffer(imageIndex);

	VkSubmitInfo queueSubmitInfo{};
	queueSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	queueSubmitInfo.waitSemaphoreCount = 1;
	queueSubmitInfo.pWaitSemaphores = &imageAvailableSemaphores[currentFrame];
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	queueSubmitInfo.pWaitDstStageMask = waitStages;
	
	queueSubmitInfo.commandBufferCount = 1;
	queueSubmitInfo.pCommandBuffers = &drawCommandBuffers[currentFrame];

	queueSubmitInfo.signalSemaphoreCount = 1;
	queueSubmitInfo.pSignalSemaphores = &renderFinishedSemaphores[currentFrame];

	result = vkQueueSubmit(graphicsQueue, 1, &queueSubmitInfo, inFlightFences[currentFrame]);
	if (result != VK_SUCCESS) {
		std::string err = "failed to submit draw commands to graphics queue, VkResult: " + std::to_string((int)result);
		throw std::runtime_error(err);
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &renderFinishedSemaphores[currentFrame];

	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &swapChain;
	presentInfo.pImageIndices = &imageIndex;

	result = vkQueuePresentKHR(presentQueue, &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || framebufferResized) {
		int width, height = 0;
		glfwGetFramebufferSize(window, &width, &height);
		if ((width & height) == 0) return; // skip frame without rebuiling the swapchain, minimized window
		framebufferResized = false;
		rebuild(window);
		return; // rebuild swapchain and skip frame
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		std::string err = "failed to present the swapchain image, VkResult: " + std::to_string((int)result);
		throw std::runtime_error(err);
	}

	currentFrame = (currentFrame + 1) % maxFramesInFlight;
}

void Graphics_Engine::recordDrawCommandBuffer(uint32_t swapImageIndex) {
	VkCommandBufferBeginInfo drawBeginInfo{};
	drawBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	VkResult result = vkBeginCommandBuffer(drawCommandBuffers[currentFrame], &drawBeginInfo);
	if (result != VK_SUCCESS) {
		std::string err = "failed to begin draw command buffer, VkResult: " + std::to_string((int)result);
		throw std::runtime_error(err);
	}

	VkClearValue clearValues[2] = {};
	clearValues[0].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
	clearValues[1].depthStencil = { 1.0f, 0 };

	VkRenderPassBeginInfo renderPassBeginInfo{};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.renderPass = renderPass;
	renderPassBeginInfo.framebuffer = framebuffers[swapImageIndex];
	renderPassBeginInfo.renderArea.offset = { 0,0 };
	renderPassBeginInfo.renderArea.extent = swapChainExtent;
	renderPassBeginInfo.clearValueCount = 2;
	renderPassBeginInfo.pClearValues = clearValues;

	vkCmdBeginRenderPass(drawCommandBuffers[currentFrame], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

	std::string boundPipeline = "";
	for (auto& image : images) {
		// only change bound pipeline if image's pipeline is different
		if (boundPipeline != image.second.graphicsPipeline) { 
			boundPipeline = image.second.graphicsPipeline;
			vkCmdBindPipeline(drawCommandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS,
				pipelines[boundPipeline].first);
		}
		// bind descriptor sets
		vkCmdBindDescriptorSets(drawCommandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, 
			pipelines[boundPipeline].second, 0, 1, &image.second.descriptorSets[currentFrame], 0, nullptr);
		// bind push constants
		if (image.second.pushConstantsSizeBytes != 0) {
			vkCmdPushConstants(drawCommandBuffers[currentFrame], pipelines[boundPipeline].second,
				VK_SHADER_STAGE_VERTEX_BIT, 0, image.second.pushConstantsSizeBytes, image.second.pushConstants.data());
		}
		vkCmdDraw(drawCommandBuffers[currentFrame], 4, 1, 0, 0); // 4 vertices with VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP
	}
	vkCmdEndRenderPass(drawCommandBuffers[currentFrame]);

	result = vkEndCommandBuffer(drawCommandBuffers[currentFrame]);
	if (result != VK_SUCCESS) {
		std::string err = "failed to submit draw command buffer, VkResult: " + std::to_string((int)result);
		throw std::runtime_error(err);
	}
}