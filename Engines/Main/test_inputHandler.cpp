//#define TEST_INPUTHANDLER
#ifdef TEST_INPUTHANDLER

#include "Input_Handler.h"

#include <iostream>

#include "GLFW/glfw3.h"

int main() {
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(600, 600, "test_inputhandler", NULL, NULL);

	// my controller does not have input mappings in GLFW v3.3.4, bindings taken from steam auto bindings
	const char* mappings = "03000000d620000011a7000000000000,Switch,a:b1,b:b2,back:b8,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,guide:b12,leftshoulder:b4,leftstick:b10,lefttrigger:b6,leftx:a0,lefty:a1,rightshoulder:b5,rightstick:b11,righttrigger:b7,rightx:a2,righty:a3,start:b9,x:b0,y:b3,platform:Windows";
	glfwUpdateGamepadMappings(mappings);

	Input_Handler iHandler;
	iHandler.initializeInput_Handler(window);
	
	iHandler.setupKeyInput("Enter", GLFW_KEY_ENTER);

	std::cout << glfwGetJoystickName(0) << std::endl;

	while (!glfwWindowShouldClose(window)) {

		glfwPollEvents();
		iHandler.updateInputStates();

		// left-click normal mouse
		if (iHandler.getMouseButtonState(mouse_buttons::LEFT_MOUSE, input_states::JUST_PRESSED)
			&& iHandler.isMouseCursorInWindow()) {
			std::cout << iHandler.getMouseCursorPosition().first << " " << iHandler.getMouseCursorPosition().second << std::endl;
			iHandler.setMouseCursorNormal();
		}
		// right-click hidden mouse
		if (iHandler.getMouseButtonState(mouse_buttons::RIGHT_MOUSE, input_states::JUST_PRESSED)
			&& iHandler.isMouseCursorInWindow()) {
			std::cout << iHandler.getMouseCursorPosition().first << " " << iHandler.getMouseCursorPosition().second << std::endl;
			iHandler.setMouseCursorHidden();
		}
		// middle-click locked mouse
		if (iHandler.getMouseButtonState(mouse_buttons::MIDDLE_MOUSE, input_states::JUST_PRESSED)
			&& iHandler.isMouseCursorInWindow()) {
			std::cout << iHandler.getMouseCursorPosition().first << " " << iHandler.getMouseCursorPosition().second << std::endl;
			iHandler.setMouseCursorLocked();
		}

		// print scroll magnitude
		if (iHandler.getMouseScroll().second) {
			std::cout << iHandler.getMouseScroll().second << std::endl;
		}

		// print pressed keys
		auto keys = iHandler.getCurrentKeysPressed();
		if (!keys.empty()) {
			bool newline = false;
			for (const auto& key : keys) {
				if (glfwGetKeyName(key.first, key.second)) { // non-printable keys return null
					std::cout << (glfwGetKeyName(key.first, key.second));
					newline = true;
				}
			}
			if (newline) std::cout << std::endl;
		}

		// controller output
		if (iHandler.isControllerConnected(0)) {
			if (iHandler.getControllerButtonState(0, controller_buttons::SWITCH_A, input_states::JUST_PRESSED)) {
				std::cout << "controler: a" << std::endl;
			}
			if (iHandler.getControllerButtonState(0, controller_buttons::L1, input_states::JUST_PRESSED)) {
				std::cout << "controller: L1" << std::endl;
			}
			if (iHandler.getControllerAxisValue(0, controlller_axes::L2) > 0.5) {
				std::cout << "controller: L2" << std::endl;
			}
		}


		// enter to exit
		if (iHandler.getKeyState("Enter", input_states::PRESSED)) glfwSetWindowShouldClose(window, true);
	}

	glfwDestroyWindow(window);
	glfwTerminate();
}


#endif // TEST_INPUTHANDLER
