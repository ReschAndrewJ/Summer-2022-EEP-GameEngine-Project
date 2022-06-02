#ifndef VALIDATION_LAYERS
#define VALIDATION_LAYERS

#include"vulkan/vulkan.h"

#include <vector>
#include <cstring>
#include <iostream>



class Validation_Layers {
public:
	const static std::vector<const char*> validationLayers;
	const static bool enableValidationLayers;


private:
	VkDebugUtilsMessengerEXT debugMessenger;


public:
	static bool checkValidationLayersSupport();
	void setupDebugMessenger(VkInstance);
	void shutdownDebugMessenger(VkInstance);

public:
	static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT&);

public:
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT, VkDebugUtilsMessageTypeFlagsEXT,
		const VkDebugUtilsMessengerCallbackDataEXT*, void*);

private:
	VkResult CreateDebugUtilsMessengerEXT(
		VkInstance, const VkDebugUtilsMessengerCreateInfoEXT*,
		const VkAllocationCallbacks*, VkDebugUtilsMessengerEXT*);

};


#endif // VALIDATION_LAYERS