#include "Input_Handler.h"

std::set<std::pair<int, int>> Input_Handler::current_pressed_keys {};
uint16_t Input_Handler::connected_controllers_bitmask {};
input_states::states Input_Handler::controller_button_states[16][GLFW_GAMEPAD_BUTTON_LAST] {};
float Input_Handler::controller_axes_values[16][GLFW_GAMEPAD_AXIS_LAST] {};
double Input_Handler::mouse_scroll_offsets[4]{};


void Input_Handler::initializeInput_Handler(GLFWwindow* window) {
	input_window = window;
	glfwSetKeyCallback(window, key_callback);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, true); // glfw_sticky_keys reduces the likelihood of inputs being dropped
	glfwSetJoystickCallback(joystick_callback);
	glfwSetScrollCallback(window, scroll_callback);

	for (uint16_t c = 0; c <= GLFW_JOYSTICK_LAST; ++c) {
		if (glfwJoystickPresent(c)) { joystick_callback(c, GLFW_CONNECTED); }
	}
}


void Input_Handler::updateInputStates() {
	for (auto& key : states_by_GLFWkey) {
		auto& key_state = key.second;

		if (glfwGetKey(input_window, key.first) == GLFW_PRESS) {	
			key_state = key_state & input_states::PRESSED ? input_states::PRESSED : input_states::JUST_PRESSED;
		}
		else {
			key_state = key_state & input_states::NOT_PRESSED ? input_states::NOT_PRESSED : input_states::JUST_RELEASED;
		}
	}
	for (int controller_i = 0; controller_i < 16; ++controller_i) {
		if (!isControllerConnected(controller_i)) break;
		GLFWgamepadstate gamepadState;
		glfwGetGamepadState(controller_i, &gamepadState);
		for (int button_i = 0; button_i <= GLFW_GAMEPAD_BUTTON_LAST; ++button_i) {
			if (gamepadState.buttons[button_i]) {
				controller_button_states[controller_i][button_i] = controller_button_states[controller_i][button_i] & input_states::PRESSED ? input_states::PRESSED : input_states::JUST_PRESSED;
			}
			else {
				controller_button_states[controller_i][button_i] = controller_button_states[controller_i][button_i] & input_states::NOT_PRESSED ? input_states::NOT_PRESSED : input_states::JUST_RELEASED;
			}
		}
		for (int axis_i = 0; axis_i <= GLFW_GAMEPAD_AXIS_LAST; ++axis_i) {
			controller_axes_values[controller_i][axis_i] = gamepadState.axes[axis_i];
		}
	}
	glfwGetCursorPos(input_window, &mouse_cursor_position[0], &mouse_cursor_position[1]);
	for (int button_i = 0; button_i <= GLFW_MOUSE_BUTTON_LAST; ++button_i) {
		if (glfwGetMouseButton(input_window, button_i) == GLFW_PRESS) {
			mouse_button_states[button_i] = mouse_button_states[button_i] & input_states::PRESSED ? input_states::PRESSED : input_states::JUST_PRESSED;
		}
		else {
			mouse_button_states[button_i] = mouse_button_states[button_i] & input_states::NOT_PRESSED ? input_states::NOT_PRESSED : input_states::JUST_RELEASED;
		}
	}
	mouse_cursor_in_window = glfwGetWindowAttrib(input_window, GLFW_HOVERED);
	mouse_scroll_offsets[0] = mouse_scroll_offsets[2];
	mouse_scroll_offsets[1] = mouse_scroll_offsets[3];
	mouse_scroll_offsets[2] = 0;
	mouse_scroll_offsets[3] = 0;

}


// ---- Keyboard

void Input_Handler::setupKeyInput(std::string key_name, int GLFW_key) {
	states_by_GLFWkey[GLFW_key] = input_states::NOT_PRESSED;
	states_by_keyname[key_name] = &states_by_GLFWkey[GLFW_key];
}


bool Input_Handler::getKeyState(std::string key_name, input_states::states state) {
	return (*states_by_keyname.at(key_name) & state) == state;
	// if the key's state contains the requested state, bitwise '&' equals the requested state
}
bool Input_Handler::getKeyState(int GLFW_KEY_value, input_states::states state) {
	return (states_by_GLFWkey.at(GLFW_KEY_value) & state) == state;
}


std::set<std::pair<int,int>> Input_Handler::getCurrentKeysPressed() {
	return current_pressed_keys;
}


void Input_Handler::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		current_pressed_keys.insert({ key, scancode });
	}
	else if (action == GLFW_RELEASE) {
		current_pressed_keys.erase({ key, scancode });
	}
}


// ---- Controller

bool Input_Handler::getControllerButtonState(int controllerIndex, controller_buttons::buttons button, input_states::states state) {
	return (controller_button_states[controllerIndex][button] & state) == state;
}


float Input_Handler::getControllerAxisValue(int controllerIndex, controlller_axes::axes axis) {
	return controller_axes_values[controllerIndex][axis];
}


bool Input_Handler::isControllerConnected(int controllerIndex) {
	uint16_t bitmask = 1<<controllerIndex;
	return connected_controllers_bitmask & bitmask;
	// bitmask can only be a single bit, so '== bitmask' is not necessary
}

#include <iostream>
void Input_Handler::joystick_callback(int joystick, int event) {
	if (event == GLFW_DISCONNECTED) {
		connected_controllers_bitmask = connected_controllers_bitmask & ~(1<<joystick);
		for (int button_i = 0; button_i <= GLFW_GAMEPAD_BUTTON_LAST; ++button_i) {
			controller_button_states[joystick][button_i] = input_states::NOT_PRESSED;
		}
		for (int axis_i = 0; axis_i <= GLFW_GAMEPAD_AXIS_LAST; axis_i++) {
			controller_axes_values[joystick][axis_i] = 0;
		}
	} 
	else if (event == GLFW_CONNECTED && glfwJoystickIsGamepad(joystick)) {
		connected_controllers_bitmask = connected_controllers_bitmask | (1<<joystick);
		for (int button_i = 0; button_i <= GLFW_GAMEPAD_BUTTON_LAST; ++button_i) {
			controller_button_states[joystick][button_i] = input_states::NOT_PRESSED;
		}
		for (int axis_i = 0; axis_i<=GLFW_GAMEPAD_AXIS_LAST; axis_i++) {
			controller_axes_values[joystick][axis_i] = 0;
		}
	} 
	if (event == GLFW_CONNECTED) {
		std::cout << "joystick connected" << std::endl;
		if (glfwJoystickIsGamepad(joystick)) {
			std::cout << "joystick is gamepad" << std::endl;
		}
	}
}


// ---- Mouse

void Input_Handler::setMouseCursorHidden() { 
	glfwSetInputMode(input_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN); 
}
void Input_Handler::setMouseCursorLocked() {
	glfwSetInputMode(input_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}
void Input_Handler::setMouseCursorNormal() {
	glfwSetInputMode(input_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

bool Input_Handler::isMouseCursorInWindow() {
	return mouse_cursor_in_window;
}


std::pair<double, double> Input_Handler::getMouseCursorPosition() {
	return { mouse_cursor_position[0], mouse_cursor_position[1] };
}


bool Input_Handler::getMouseButtonState(mouse_buttons::buttons button, input_states::states state) {
	return (mouse_button_states[button] & state) == state;
}


bool Input_Handler::getMouseButtonState(int button, input_states::states state) {
	return (mouse_button_states[button] & state) == state;
}


std::pair<double, double> Input_Handler::getMouseScroll() {
	return { mouse_scroll_offsets[0], mouse_scroll_offsets[1] };
}


void Input_Handler::scroll_callback(GLFWwindow* window, double pos_x, double pos_y) {
	mouse_scroll_offsets[2] += pos_x;
	mouse_scroll_offsets[3] += pos_y;
}
