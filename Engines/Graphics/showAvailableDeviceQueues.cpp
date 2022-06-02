//#define SHOWAVAILABLEDEVICEQUEUES
#ifdef SHOWAVAILABLEDEVICEQUEUES

#include "vulkan/vulkan.h"
#include <vector>
#include <stdexcept>
#include <string>

// output all available physical devices and queues
int main() {

	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "";
	appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 0, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_API_VERSION(0, 0, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_3;

	VkInstanceCreateInfo instCreateInfo{};
	instCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instCreateInfo.pApplicationInfo = &appInfo;
	instCreateInfo.enabledExtensionCount = 0;
	instCreateInfo.ppEnabledExtensionNames = nullptr;
	instCreateInfo.enabledLayerCount = 0;
	instCreateInfo.pNext = nullptr;

	VkInstance instance;
	VkResult result = vkCreateInstance(&instCreateInfo, nullptr, &instance);
	if (result != VK_SUCCESS) {
		std::string err = "failed to create Vulkan instance, VkResult: " + std::to_string((int)result);
		throw std::runtime_error(err);
	}

	uint32_t physicalDeviceCount;
	vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
	std::vector<VkPhysicalDevice> devices(physicalDeviceCount);
	vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, devices.data());

	for (int i = 1; i <= physicalDeviceCount; ++i) {
		printf("device: %i\n", i);
		auto physicalDevice = devices[i - 1];

		uint32_t queueFamilyCount;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
		std::vector<VkQueueFamilyProperties> families(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, families.data());

		for (int k = 1; k <= families.size(); ++k) {
			printf("queue family: %i\n", k);
			auto& queueFamily = families[k - 1];

			printf("queue count: %i\n", queueFamily.queueCount);
			
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) printf("Graphics\n");
			if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) printf("Compute\n");
			if (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT) printf("Transfer\n");
			printf("\n");
		}


	}



}


#endif // SHOWAVAILABLEDEVICEQUEUES
