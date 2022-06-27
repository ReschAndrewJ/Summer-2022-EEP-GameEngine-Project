#ifndef OBJECT_CAMERA
#define OBJECT_CAMERA OBJECT_CAMERA

#include "../Spatial/Object_Spatial.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

const auto OBJECT_CLASS_CAMERA = "Object_Camera";

const auto ATTRIBUTE_CAMERA_DEPTH_MIN = "MinCameraDepth";
const auto ATTRIBUTE_CAMERA_DEPTH_MAX = "MaxCameraDepth";
const auto ATTRIBUTE_CAMERA_FOV = "CameraFOV";

class Object_Camera : public Object_Spatial {
protected:
	GLFWwindow** windowPtr;

public:
	Object_Camera();
	virtual ~Object_Camera();

	/*
	* the camera conditionally needs to pass its tranformation matrix on to child nodes,
	* (i.e. just for nodes that are visual),
	* this distinction cannot be made inside the spatial transformation function and
	* so a seperate function is needed to be called directly by the descendant object when applicable
	*/

	// override the transformation matrix to not affect the passed matrix
	glm::mat4 getTransformationMatrix(glm::mat4 childTransform = glm::mat4(1.0f)) override;

	// provide the camera transformations, virtual allows for custom cameras
	virtual glm::mat4 getCameraTransformationMatrix();


};


#endif //OBJECT_CAMERA