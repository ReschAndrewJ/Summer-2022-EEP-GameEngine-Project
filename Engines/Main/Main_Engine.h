#ifndef MAIN_ENGINE
#define MAIN_ENGINE


#include "../../Object_Loader/Object_Loader.h"
#include "../Graphics/Graphics_Engine.h"
//#include "../Sound/Sound_Engine.h" not implemented
#include "Input_Handler.h"

#include "PTR_IDENTIFIER_ENUMS.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"


#include <string>
#include <unordered_map>
#include <map>
#include <vector>

#include <thread>
#include <mutex>
#include <atomic>


struct engine_start_info {
	std::string windowTitle = "program";
	int windowWidth = 1280;
	int windowHeight = 720;
	bool enforceAspectRatio = true;
	int aspectRatioWidth = 16;
	int aspectRatioHeight = 9;
	unsigned int maxFramerate = 60;

	std::vector<std::pair<std::string, int>> keysToPoll;

	std::string rootObject_filePath;
	std::string rootObject_objectName;
};


class Main_Engine
{
	
private:
	GLFWwindow* displayWindow;

	Object_Loader objectLoader;
	Graphics_Engine graphicsEngine;
	//Sound_Engine soundEngine;
	Input_Handler inputHandler;

	
	std::unordered_map <std::string, Object*> objectsContainer;
	// Object Queues
	// 
	// <filepath, objectName, parentIdentifier>
	std::vector<std::tuple<std::string, std::string, std::string>> objectCreationQueue;
	// <objectIdentifier>
	std::unordered_set<std::string> objectDestructionQueue;
	
	// Graphics Queues
	// 
	// <{owner object identifier, image identifier attribute name}, imageCreateInfo>
	std::map<std::pair<std::string, std::string>, image_create_info> imageCreationQueue;
	// <image identifier>
	std::unordered_set<std::string> imageDestructionQueue;
	// <{owner object identifier, image identifier attribute name}, pushConstantsValues>
	std::map<std::pair<std::string, std::string>, std::vector<char>> pushConstantsUpdateQueue;


public:
	/* starts the game engine */
	void start(engine_start_info);

private:
	/* semaphore provides synchronization for the engine by setting the passed
	booleans to true when both threads have called the semaphore's arrive function */
	struct semaphore {
		std::mutex lock;
		int threadsNeededToPass = 2;
		int reverseThreadCount = threadsNeededToPass;
		std::vector<std::atomic<bool>*> signals;
		void arriveAtSemaphore(std::atomic<bool>* signal);
	};
	semaphore midFrameSemaphore;
	semaphore endFrameSemaphore;
	semaphore deltaUpdateSemaphore;

	float shortestFrameLength;
	std::atomic<bool> endLoop = false;
	std::atomic<float> delta = 0;
	void objectLoop(); // Main Thread
	static void graphicsLoop(Main_Engine* self); // Secondary Thread

	/* shuts down the game engine */
	void end();

	//

	void createObjectsInQueue();
	void destroyObjectsInQueue();

	void sendImageCreationQueue();
	void sendPushConstantsUpdateQueue();
	void sendImageDestructionQueue();


};


#endif //MAIN_ENGINE