#define TEST_RUNFULLENGINE
#ifdef TEST_RUNFULLENGINE


#include "../Engines/Main/Main_Engine.h"



const auto PLAYERCAM = "Object_PlayerCamera";
const auto PLAYER = "Object_Player";
const auto REACT = "Object_React";


#include <math.h>
#include "glm/gtc/matrix_transform.hpp"
#include "../Objects/Camera/Object_Camera.h"

class playerCamera : virtual public Object_Camera {
	Input_Handler* inputHandlerPtr;
public:
	virtual ~playerCamera() {}

	static void afterCreateFunc(Object* selfPtr) {
		playerCamera& self = *dynamic_cast<playerCamera*>(selfPtr);

		self.inputHandlerPtr->setupKeyInput("W", GLFW_KEY_W);
		self.inputHandlerPtr->setupKeyInput("A", GLFW_KEY_A);
		self.inputHandlerPtr->setupKeyInput("S", GLFW_KEY_S);
		self.inputHandlerPtr->setupKeyInput("D", GLFW_KEY_D);
		self.inputHandlerPtr->setupKeyInput("Z", GLFW_KEY_Z);
		self.inputHandlerPtr->setupKeyInput("X", GLFW_KEY_X);
		self.inputHandlerPtr->setupKeyInput("Q", GLFW_KEY_Q);
		self.inputHandlerPtr->setupKeyInput("E", GLFW_KEY_E);
		self.inputHandlerPtr->setMouseCursorLocked();
	}

	static void processFunc(Object* selfPtr, float delta) {
		playerCamera& self = *dynamic_cast<playerCamera*>(selfPtr);
		
		glm::vec3 pos = { (float)self.getAttribute(ATTRIBUTE_SPATIAL_POSITION_X),
			(float)self.getAttribute(ATTRIBUTE_SPATIAL_POSITION_Y),
			(float)self.getAttribute(ATTRIBUTE_SPATIAL_POSITION_Z) };

		glm::mat4 rotMat = glm::mat4(1);
		rotMat = glm::rotate(glm::mat4(1.0f), glm::radians((float)self.getAttribute(ATTRIBUTE_SPATIAL_ROTATE_NOD)), {1,0,0}) * rotMat;
		rotMat = glm::rotate(glm::mat4(1.0f), glm::radians((float)self.getAttribute(ATTRIBUTE_SPATIAL_ROTATE_TURN)), {0,1,0}) * rotMat;
		glm::vec3 viewAxis = rotMat * glm::vec4{ 0,0,1,0 };
		rotMat = glm::rotate(glm::mat4(1.0f), glm::radians((float)self.getAttribute(ATTRIBUTE_SPATIAL_ROTATE_TILT)), viewAxis) * rotMat;
		glm::vec3 facing = glm::normalize(rotMat * glm::vec4{ 0,0,-1,1 });
		glm::vec3 right = glm::normalize(rotMat * glm::vec4{ 1,0,0,1 });
		glm::vec3 up = glm::normalize(rotMat * glm::vec4{ 0,-1,0,1 }); // y-axis inversion

		if (self.inputHandlerPtr->getKeyState("W", input_states::PRESSED)) pos += (20 * delta * facing);
		if (self.inputHandlerPtr->getKeyState("S", input_states::PRESSED)) pos -= (20 * delta * facing);
		if (self.inputHandlerPtr->getKeyState("A", input_states::PRESSED)) pos -= (20 * delta * right);
		if (self.inputHandlerPtr->getKeyState("D", input_states::PRESSED)) pos += (20 * delta * right);
		if (self.inputHandlerPtr->getKeyState("Z", input_states::PRESSED)) pos += (20 * delta * up);
		if (self.inputHandlerPtr->getKeyState("X", input_states::PRESSED)) pos -= (20 * delta * up);

		self.setAttribute(ATTRIBUTE_SPATIAL_POSITION_X, pos.x);
		self.setAttribute(ATTRIBUTE_SPATIAL_POSITION_Y, pos.y);
		self.setAttribute(ATTRIBUTE_SPATIAL_POSITION_Z, pos.z);

		float nod = (float)self.getAttribute(ATTRIBUTE_SPATIAL_ROTATE_NOD) + (float)self.inputHandlerPtr->getMouseCursorPosition().second * delta * 20;
		if (nod > 90) nod = 90; if (nod < -90) nod = -90;
		self.setAttribute(ATTRIBUTE_SPATIAL_ROTATE_NOD, nod);
		self.setAttribute(ATTRIBUTE_SPATIAL_ROTATE_TURN,
			(float)self.getAttribute(ATTRIBUTE_SPATIAL_ROTATE_TURN) - self.inputHandlerPtr->getMouseCursorPosition().first * delta * 20);
		float rot = 0;
		if (self.inputHandlerPtr->getKeyState("Q", input_states::PRESSED)) rot -= (20 * delta);
		if (self.inputHandlerPtr->getKeyState("E", input_states::PRESSED)) rot += (20 * delta);
		self.setAttribute(ATTRIBUTE_SPATIAL_ROTATE_TILT, (float)self.getAttribute(ATTRIBUTE_SPATIAL_ROTATE_TILT) + rot);

	}

	playerCamera() {
		addClassIdentifier(PLAYERCAM);
		createAttribute("TargetPlayer", Attribute::types::STRING);
		addRequestedPointer(PTR_IDENTIFIER::INPUT_HANDLER_PTR, &inputHandlerPtr);
		addProcessFunction(&processFunc, 10);
		addAfterCreationFunction(&afterCreateFunc);
	}
};


#include "../Objects/Collision/Object_Collision.h"
#include "../Objects/SoundOutput/Object_SoundOutput.h"
#include "../Objects/Sprite/Object_Sprite.h"
class reactToPlayer : virtual public Object_Collision, virtual public Object_SoundOutput, virtual public Object_Sprite {
public:
	virtual ~reactToPlayer() {}
	
	float delay = 0;
	void reaction() {
		if (delay <= 0) {
			play();
			delay = getAttribute("delay");
		}
	}
	
	static void processFunc(Object* self, float delta) {
		if (dynamic_cast<reactToPlayer*>(self)->delay > 0) dynamic_cast<reactToPlayer*>(self)->delay -= delta;
	}

	reactToPlayer() {
		addClassIdentifier(REACT);
		createAttribute("delay", Attribute::types::DOUBLE);
		setAttribute("delay", 0.5);
		addProcessFunction(&processFunc, 5);
	}

};


#include "../Objects/ActiveCollision/Object_ActiveCollision.h"
#include "../Objects/AnimatedSprite/Object_AnimatedSprite.h"
class player : virtual public Object_ActiveCollision, virtual public Object_AnimatedSprite {
	Input_Handler* inputHandlerPtr;
public:
	virtual ~player() {}

	static void afterCreateFunc(Object* selfPtr) {
		player& self = *dynamic_cast<player*>(selfPtr);

		self.inputHandlerPtr->setupKeyInput("UP", GLFW_KEY_UP);
		self.inputHandlerPtr->setupKeyInput("DOWN", GLFW_KEY_DOWN);
		self.inputHandlerPtr->setupKeyInput("LEFT", GLFW_KEY_LEFT);
		self.inputHandlerPtr->setupKeyInput("RIGHT", GLFW_KEY_RIGHT);
		self.inputHandlerPtr->setupKeyInput("ENTER", GLFW_KEY_ENTER);
		self.inputHandlerPtr->setupKeyInput("RSHIFT", GLFW_KEY_RIGHT_SHIFT);
	}

	static void processFunc(Object* selfPtr, float delta) {
		player& self = *dynamic_cast<player*>(selfPtr);

		glm::vec3 mov = { 0,0,0 };

		if (self.inputHandlerPtr->getKeyState("UP", input_states::PRESSED)) mov.z -= 20;
		if (self.inputHandlerPtr->getKeyState("DOWN", input_states::PRESSED)) mov.z += +20;
		if (self.inputHandlerPtr->getKeyState("LEFT", input_states::PRESSED)) mov.x -= 20;
		if (self.inputHandlerPtr->getKeyState("RIGHT", input_states::PRESSED)) mov.x += 20;
		if (self.inputHandlerPtr->getKeyState("ENTER", input_states::PRESSED)) mov.y -= 20;
		if (self.inputHandlerPtr->getKeyState("RSHIFT", input_states::PRESSED)) mov.y += 20;

		if (mov != glm::vec3{ 0,0,0 }) {
			auto collisions = self.move(mov.x, mov.y, mov.z, delta, 2);
			for (auto& collider : collisions) if (self.getObject(collider)->is_class(REACT)) {
				dynamic_cast<reactToPlayer*>(self.getObject(collider))->reaction();
			}
		}
	}

	player() {
		addClassIdentifier(PLAYER);
		addRequestedPointer(PTR_IDENTIFIER::INPUT_HANDLER_PTR, &inputHandlerPtr);
		addProcessFunction(&processFunc, 10);
		addAfterCreationFunction(&afterCreateFunc);
	}
};








int main() {

	Main_Engine engine;
	engine_start_info startInfo;

	startInfo.rootObject_filePath = "./test_runFullEngine/test_runFullEngineInst1.inst";
	startInfo.rootObject_objectName = "PlayerCamera";

	startInfo.AddedObjectCreateFunctions.push_back({ PLAYERCAM, [] { return (Object*)new playerCamera(); } });
	startInfo.AddedObjectCreateFunctions.push_back({ PLAYER, [] { return (Object*)new player(); } });
	startInfo.AddedObjectCreateFunctions.push_back({ REACT, [] { return (Object*)new reactToPlayer(); } });

	engine.start(startInfo);

}


#endif