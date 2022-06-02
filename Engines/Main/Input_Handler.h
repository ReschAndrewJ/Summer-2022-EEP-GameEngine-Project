#ifndef INPUT_HANDLER
#define INPUT_HANDLER

#include "GLFW/glfw3.h"

#include <unordered_map>
#include <string>
#include <set>


/* input_states must be placed outside of the Input_Handler class to both 
	have a scoped name and be able to be implicitly cast to int */

/* NOT_PRESSED, JUST_RELEASED, PRESSED, JUST_PRESSED */
namespace input_states {
	enum states { NOT_PRESSED = 1, JUST_RELEASED = 1 | 2, PRESSED = 4, JUST_PRESSED = 4 | 8 };
}

namespace controller_buttons {
	enum buttons {
		XBOX_A = GLFW_GAMEPAD_BUTTON_A, XBOX_B = GLFW_GAMEPAD_BUTTON_B, 
		XBOX_X = GLFW_GAMEPAD_BUTTON_X, XBOX_Y = GLFW_GAMEPAD_BUTTON_Y,
		SWITCH_A = XBOX_B, SWITCH_B = XBOX_A, SWITCH_X = XBOX_Y, SWITCH_Y = XBOX_X,
		CROSS = GLFW_GAMEPAD_BUTTON_CROSS, CIRCLE = GLFW_GAMEPAD_BUTTON_CIRCLE,
		SQUARE = GLFW_GAMEPAD_BUTTON_SQUARE, TRIANGLE = GLFW_GAMEPAD_BUTTON_TRIANGLE,
		L1 = GLFW_GAMEPAD_BUTTON_LEFT_BUMPER, R1 = GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER,
		L3 = GLFW_GAMEPAD_BUTTON_LEFT_THUMB, R3 = GLFW_GAMEPAD_BUTTON_RIGHT_THUMB,
		DPAD_LEFT = GLFW_GAMEPAD_BUTTON_DPAD_LEFT, DPAD_RIGHT = GLFW_GAMEPAD_BUTTON_DPAD_RIGHT,
		DPAD_UP = GLFW_GAMEPAD_BUTTON_DPAD_UP, DPAD_DOWN = GLFW_GAMEPAD_BUTTON_DPAD_DOWN,
		START = GLFW_GAMEPAD_BUTTON_START, GUIDE = GLFW_GAMEPAD_BUTTON_GUIDE,
		BACK = GLFW_GAMEPAD_BUTTON_BACK
	};
}
namespace controlller_axes {
	enum axes {
		LEFT_STICK_X = GLFW_GAMEPAD_AXIS_LEFT_X, LEFT_STICK_Y = GLFW_GAMEPAD_AXIS_LEFT_Y,
		RIGHT_STICK_X = GLFW_GAMEPAD_AXIS_RIGHT_X, RIGHT_STICK_Y = GLFW_GAMEPAD_AXIS_RIGHT_Y,
		L2 = GLFW_GAMEPAD_AXIS_LEFT_TRIGGER, R2 = GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER
	};
}
namespace mouse_buttons {
	enum buttons {
		LEFT_MOUSE = GLFW_MOUSE_BUTTON_LEFT, RIGHT_MOUSE = GLFW_MOUSE_BUTTON_RIGHT,
		MIDDLE_MOUSE = GLFW_MOUSE_BUTTON_MIDDLE
	};
}


/* class to simplify processing inputs by managing the input states,
synchronizes input handling to the engine loop */
class Input_Handler
{	
private:
	GLFWwindow* input_window = nullptr;

	// holds the values of the key states
	std::unordered_map<int, input_states::states> states_by_GLFWkey;
	// holds pointers to the values of the key states
	std::unordered_map<std::string, input_states::states*> states_by_keyname;
	
	input_states::states mouse_button_states[GLFW_MOUSE_BUTTON_LAST] {};
	bool mouse_cursor_in_window = false;
	double mouse_cursor_position[2] {};


public:
// --- Main Engine Functions

	/* tells the Input_Handler what GLFWWindow to poll input states from */
	void initializeInput_Handler(GLFWwindow*);

	/* updates the Input_Handler's internal input data */
	void updateInputStates();

// ---- Keyboard

	/* tells the Input_Handler to process inputs for the given GLFW_KEY value, using the given name as an alias
	setupInput(string key_name, int GLFW_Key) */
	void setupKeyInput(std::string, int);

	/* returns true if the tested state is contained in the key's input_states bitmask
	ex.: tested state - NOT_PRESSED, actual state - JUST_RELEASED, returns true,
	keys that have not been set up return false,
	getKeyState(string key_name, input_states::states state)
	getKeyState(int GLFW_KEY_value, input_states::states state) */
	bool getKeyState(std::string, input_states::states);
	bool getKeyState(int, input_states::states);

	/* returns all keys currently pressed, regardless of if setupKeyInput has been called for those keys,
	first value is the GLFW_KEY value, second value is the key's scancode */
	std::set<std::pair<int,int>> getCurrentKeysPressed();

// ---- Controller

	/* returns true if the tested state is contained in the button's input_states bitmask
	getControllerButtonState(int controllerIndex, controller_buttons::buttons button, input_states::states state) 
	getControllerButtonState(int controllerIndex, int GLFW_BUTTON_value, inputstates::states state) */
	bool getControllerButtonState(int, controller_buttons::buttons, input_states::states);
	bool getControllerButtonState(int, int, input_states::states);

	/* returns the magnitude of the axis [-1.0,1.0] 
	getControllerAxisValue(int controllerIndex, controller_axes::axes axis)
	getControllerAxisValue(int controllerIndex, int axis) */
	float getControllerAxisValue(int, controlller_axes::axes);
	float getControllerAxisValue(int, int);

	/* returns true if the requested controller is connected
	isControllerConnected(int controllerIndex) */
	bool isControllerConnected(int);

// ---- Mouse

	void setMouseCursorHidden(); // makes the cursor invisible when in the application window area
	void setMouseCursorLocked(); // hides the cursor and locks it to the application window
	void setMouseCursorNormal(); // makes the cursor behave normally

	/* returns true if the mouse cursor is in the screen area of the application window */
	bool isMouseCursorInWindow();

	/* returns the unnormalized coordinates of the mouse cursor
	<Position_X, Position_Y> getMouseCursorPosition() */
	std::pair<double, double> getMouseCursorPosition();

	/* returns true if the tested state is contained in the mouse button's input_states bitmask,
	* can take an integer to access additional buttons on the mouse (max: GLFW_MOUSE_BUTTON_LAST),
	getMouseButtonState(mouse_buttons::buttons button, input_states::states state)
	getMouseButtonState(int button, input_states::states state) */
	bool getMouseButtonState(mouse_buttons::buttons, input_states::states);
	bool getMouseButtonState(int, input_states::states);

	/* returns the offset of the scroll wheel between the two most recent calls to updateInputStates() */
	std::pair<double, double> getMouseScroll();

	
// ---- statics
private:
	static std::set<std::pair<int, int>> current_pressed_keys;
	static uint16_t connected_controllers_bitmask;

	// holds the states of the buttons for each of the connected controllers
	static input_states::states controller_button_states[16][GLFW_GAMEPAD_BUTTON_LAST];
	// holds the states of the axes for each controller
	static float controller_axes_values[16][GLFW_GAMEPAD_AXIS_LAST];

	static double mouse_scroll_offsets[4]; // x_pollable, y_pollable, x_increment, y_increment


public:
	static void key_callback(GLFWwindow*, int, int, int, int);
	static void joystick_callback(int, int);
	static void scroll_callback(GLFWwindow*, double, double);


};


#endif // INPUT_HANDLER