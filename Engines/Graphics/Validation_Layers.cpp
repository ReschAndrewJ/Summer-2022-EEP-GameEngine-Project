#include "Validation_Layers.h"


const std::vector<const char*> Validation_Layers::validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};
#ifdef NDEBUG
const bool Validation_Layers::enableValidationLayers = false;
#else
const bool Validation_Layers::enableValidationLayers = true;
#endif

void Validation_Layers::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageType = 0
		| VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
		;
	createInfo.messageSeverity = 0
		//| VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
		//| VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
		;
	createInfo.pfnUserCallback = debugCallback;
}


bool Validation_Layers::checkValidationLayersSupport() {
	uint32_t layercount;
	vkEnumerateInstanceLayerProperties(&layercount, nullptr);
	std::vector<VkLayerProperties> availableLayers(layercount);
	vkEnumerateInstanceLayerProperties(&layercount, availableLayers.data());

	for (const char* layerName : validationLayers) {
		bool layerfound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerfound = true;
				break;
			}
		}
		if (!layerfound) return false;
	}

	return true;
}


void Validation_Layers::setupDebugMessenger(VkInstance vulkanInstance) {
	if (!enableValidationLayers) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	populateDebugMessengerCreateInfo(createInfo);

	if (CreateDebugUtilsMessengerEXT(vulkanInstance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
		throw std::runtime_error("failed to set up debug messenger");
	}
}

void Validation_Layers::shutdownDebugMessenger(VkInstance vulkanInstance) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vulkanInstance,
		"vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(vulkanInstance, debugMessenger, nullptr);
	}
}



VKAPI_ATTR VkBool32 VKAPI_CALL Validation_Layers::debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messegeSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData) {
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl << std::endl;

	return VK_FALSE;
}


VkResult Validation_Layers::CreateDebugUtilsMessengerEXT(
	VkInstance vulkanInstance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {

	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vulkanInstance,
		"vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) return func(vulkanInstance, pCreateInfo, pAllocator, pDebugMessenger);
	else return VK_ERROR_EXTENSION_NOT_PRESENT;
}
