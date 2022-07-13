#include "Main_Engine.h"

#include <chrono>
#include <functional>

void Main_Engine::start(engine_start_info startInfo) {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, false);

	displayWindow = glfwCreateWindow(startInfo.windowWidth, startInfo.windowHeight, startInfo.windowTitle.data(), NULL, NULL);
	if (startInfo.enforceAspectRatio) {
		glfwSetWindowAspectRatio(displayWindow, startInfo.aspectRatioWidth, startInfo.aspectRatioHeight);
	}

	inputHandler.initializeInput_Handler(displayWindow);
	for (auto& key : startInfo.keysToPoll) {
		inputHandler.setupKeyInput(key.first, key.second);
	}

	graphicsEngine.startupEngine(displayWindow);

	objectLoader.setupBuiltInClasses();
	for (auto& func : startInfo.AddedObjectCreateFunctions) {
		objectLoader.addClassCreatorFunction(func.first, func.second);
	}
	
	objectCreationQueue.push_back({ "", "ROOT", "", {} });
	createObjectsInQueue();
	
	if (!startInfo.rootObject_filePath.empty() && !startInfo.rootObject_objectName.empty()) {
		objectCreationQueue.push_back({ startInfo.rootObject_filePath, startInfo.rootObject_objectName, "ROOT", {} });
	}

	shortestFrameLength = 1.0f / startInfo.maxFramerate;
	endLoop = false;

	std::thread secondaryThread(&graphicsLoop, this);
	objectLoop();
	secondaryThread.join();
	end();
}


void Main_Engine::end() {
	soundEngine.clearEngine();
	graphicsEngine.shutdownEngine();
	glfwTerminate();

	// cleanup objects
	for (auto& obj : objectsContainer) {
		delete obj.second;
	}
	objectsContainer.clear();

	// cleanup queues
	objectCreationQueue.clear();
	objectDestructionQueue.clear();
	imageCreationQueue.clear();
	pushConstantsUpdateQueue.clear();
	imageDestructionQueue.clear();

}


void Main_Engine::semaphore::arriveAtSemaphore(std::atomic<bool>* signal = nullptr) {
	lock.lock();
	if (signal != nullptr) *signal = false;
	signals.push_back(signal);
	--reverseThreadCount;
	if (!reverseThreadCount) { // reverseThreadCount == 0
		for (size_t i = 0; i < signals.size(); ++i) if (signals[i] != nullptr) *signals[i] = true;
		reverseThreadCount = threadsNeededToPass; // reset reverseThreadCount
	}
	lock.unlock();
}


Main_Engine::semaphore::semaphore(int threads) {
	threadsNeededToPass = threads;
	reverseThreadCount = threads;
}


void Main_Engine::objectLoop() {
	delta = 0; // time between frames in seconds
	auto endOfLastFrame = std::chrono::steady_clock::now();

	while (!glfwWindowShouldClose(displayWindow)) {
		if (delta < shortestFrameLength) {
			std::this_thread::sleep_for(std::chrono::milliseconds(long long((shortestFrameLength - delta)*1000)));
			auto time = std::chrono::steady_clock::now();
			delta = std::chrono::duration<float>(time - endOfLastFrame).count();
		}

		glfwPollEvents();
		inputHandler.updateInputStates();

		// queue objects' processes
		std::map<int, std::vector < std::function<void()> >> sortedAndCurried;
		float deltaAsVariable = delta;
		for (auto& obj : objectsContainer) {
			for (const auto& func : obj.second->process_functions) {
				sortedAndCurried[func.first].push_back([&func, &obj, deltaAsVariable]() { func.second(obj.second, deltaAsVariable); });
			}
		}

		// run objects' proceses
		for (auto& priority : sortedAndCurried) {
			for (auto& curriedFunc : priority.second) {
				curriedFunc();
			}
		}

		// mid-point block, wait for the secondary thread to clear its queues
		std::atomic<bool> midpointSignal;
		midFrameSemaphore.arriveAtSemaphore(&midpointSignal);
		while (!midpointSignal) std::this_thread::sleep_for(std::chrono::milliseconds(10));


		// process queues
		// 
		// object create queue
		createObjectsInQueue();
		// object destroy queue
		destroyObjectsInQueue();
		// image create queue
		sendImageCreationQueue();
		// image destroy queue
		sendImageDestructionQueue();
		// push constant update queue
		sendPushConstantsUpdateQueue();


		// end-point block, both threads wait for synchronicity
		std::atomic<bool> endpointSignal;
		endFrameSemaphore.arriveAtSemaphore(&endpointSignal);
		while (!endpointSignal) std::this_thread::sleep_for(std::chrono::milliseconds(10));

		auto time = std::chrono::steady_clock::now();
		delta = std::chrono::duration<float>(time - endOfLastFrame).count();
		endOfLastFrame = time;

		// tell the secondary thread that delta has been updated
		deltaUpdateSemaphore.arriveAtSemaphore();
	}

	endLoop = true;
}



void Main_Engine::graphicsLoop(Main_Engine* self) {
	while (!self->endLoop) {
		if (self->delta < self->shortestFrameLength) {
			std::this_thread::sleep_for(std::chrono::milliseconds(long long((self->shortestFrameLength - self->delta) * 1000)));
		}

		// create queued images
		self->graphicsEngine.createQueuedImages();
		// update push constants
		self->graphicsEngine.updatePushConstants();
		// destroy queued images
		self->graphicsEngine.destroyQueuedImages();
		// prepare image destruction for the next frame
		self->graphicsEngine.readyImageDestructionQueue();

		// mid-point block, let the main thread know the queues are safe to update
		self->midFrameSemaphore.arriveAtSemaphore();


		// draw processes
		self->graphicsEngine.drawFrame(self->displayWindow);


		// end-point block, both threads wait for synchronicity
		std::atomic<bool> endpointSignal;
		self->endFrameSemaphore.arriveAtSemaphore(&endpointSignal);
		while (!endpointSignal) std::this_thread::sleep_for(std::chrono::milliseconds(10));

		// wait for the main thread to update delta
		std::atomic<bool> deltaUpdateSignal;
		self->deltaUpdateSemaphore.arriveAtSemaphore(&deltaUpdateSignal);
		while (!deltaUpdateSignal) std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}



void Main_Engine::createObjectsInQueue() {

	// create objects
	std::vector<std::string> newIdentifiers;
	for (size_t i = 0; i < objectCreationQueue.size(); ++i) {
		auto& queuedObj = objectCreationQueue[i];
		
		std::string identifier;
		std::pair<Object*, std::vector<std::pair<std::string, std::string>>> newObj;
		if (std::get<1>(queuedObj) == "ROOT") {
			newObj.first = new Object();
			newObj.first->identifier = "ROOT";
			identifier = "ROOT";
		}
		else {
			if (!objectsContainer.count(std::get<2>(queuedObj))) continue; // skip object if assigned parent doesn't exist
			newObj = objectLoader.createInstance(std::get<0>(queuedObj), std::get<1>(queuedObj));

			identifier = std::get<1>(queuedObj);
			while (objectsContainer.count(identifier)) identifier += std::to_string(rand() % 10);
			newObj.first->identifier = identifier;
			newObj.first->parent = std::get<2>(queuedObj);
		}

		// handle requested pointers
		for (auto& ptrIt : newObj.first->requestedPointers) {
			void** pointer = reinterpret_cast<void**>(ptrIt.second);
			switch (ptrIt.first) {
				case PTR_IDENTIFIER::INPUT_HANDLER_PTR: *(pointer) = &inputHandler; break;
				case PTR_IDENTIFIER::OBJ_CONTAINER_PTR: *(pointer) = &objectsContainer; break;
				case PTR_IDENTIFIER::OBJ_CREATION_QUEUE_PTR: *(pointer) = &objectCreationQueue; break;
				case PTR_IDENTIFIER::OBJ_DESTRUCTION_QUEUE_PTR: *(pointer) = &objectDestructionQueue; break;
				case PTR_IDENTIFIER::IMG_CREATION_QUEUE_PTR: *(pointer) = &imageCreationQueue; break;
				case PTR_IDENTIFIER::IMG_DESTRUCTION_QUEUE_PTR: *(pointer) = &imageDestructionQueue; break;
				case PTR_IDENTIFIER::PUSH_UPDATE_QUEUE_PTR: *(pointer) = &pushConstantsUpdateQueue; break;
				case PTR_IDENTIFIER::WINDOW_PTR: *(pointer) = &displayWindow; break;
				case PTR_IDENTIFIER::SOUND_ENGINE_PTR: *(pointer) = &soundEngine; break;
				default: *(pointer) = nullptr; break;
			}
		}
		for (auto& attr : std::get<3>(queuedObj)) {
			newObj.first->setAttribute(attr.first, attr.second);
		}
		objectsContainer.insert({ identifier, newObj.first });
		
		for (size_t j = 0; j < newObj.second.size(); ++j) {
			objectCreationQueue.push_back({ newObj.second[j].first, newObj.second[j].second, identifier, {} });
		}
		newIdentifiers.push_back(identifier);
	}
	objectCreationQueue.clear();

	// run objects' afterCreation functions
	for (auto& newObj : newIdentifiers) {
		Object* objPtr = objectsContainer.at(newObj);
		for (auto& func : objPtr->afterCreation_functions) func(objPtr);
		// add objects to parents' children lists
		if (objPtr->identifier != "ROOT") {
			objectsContainer.at(objPtr->parent)->children.insert(objPtr->identifier);
		}
	}
}


void Main_Engine::destroyObjectsInQueue() {

	// move destruction queue to a vector
	std::vector<std::string> queueAsVector(objectDestructionQueue.begin(), objectDestructionQueue.end());

	// add descendant objects to destruction queue
	for (size_t i = 0; i < queueAsVector.size(); ++i) {
		if (!objectsContainer.count(queueAsVector[i])) continue; // skip objects that already don't exist
		for (const std::string& child : objectsContainer.at(queueAsVector[i])->children) {
			queueAsVector.push_back(child);
		}
	}
	// run objects' beforeDestruction functions
	for (auto& objToDestroy : queueAsVector) {
		if (!objectsContainer.count(objToDestroy)) continue; // skip objects that already don't exist
		Object* objPtr = objectsContainer.at(objToDestroy);
		for (auto& func : objPtr->beforeDestruction_functions) func(objPtr);
	}
	// remove objects from parents' children lists
	for (auto& objToDestroy : queueAsVector) {
		objectsContainer.at(objectsContainer.at(objToDestroy)->parent)->children.erase(objToDestroy);
	}
	// destroy objects
	for (auto& objToDestroy : queueAsVector) {
		if (!objectsContainer.count(objToDestroy)) continue; // skip objects that already don't exist
		delete objectsContainer.at(objToDestroy);
		objectsContainer.erase(objToDestroy);
	}
	// clear queue
	objectDestructionQueue.clear();
}



void Main_Engine::sendImageCreationQueue() {

	for (auto& img : imageCreationQueue) {
		if (!objectsContainer.count(img.first.first)) continue; // skip image if the owner object does not exists
	
		objectsContainer.at(img.first.first)->setAttribute(img.first.second, graphicsEngine.queueAddImage(img.second));
	}
	imageCreationQueue.clear();
}


void Main_Engine::sendImageDestructionQueue() {

	for (auto& img : imageDestructionQueue) {
		graphicsEngine.queueDestroyImage(img);
	}
	imageDestructionQueue.clear();
}


void Main_Engine::sendPushConstantsUpdateQueue() {

	for (auto& pushInfo : pushConstantsUpdateQueue) {
		if (!objectsContainer.count(pushInfo.first.first)) continue; // skip push update if the image owner object does not exist
		graphicsEngine.queuePushConstantsUpdate(objectsContainer.at(pushInfo.first.first)->getAttribute(pushInfo.first.second), pushInfo.second);
	}

	pushConstantsUpdateQueue.clear();
}

