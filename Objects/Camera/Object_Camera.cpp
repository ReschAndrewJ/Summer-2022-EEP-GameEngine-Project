#include "Object_Camera.h"


#include "glm/gtc/matrix_transform.hpp"


Object_Camera::Object_Camera() {
	addClassIdentifier(OBJECT_CLASS_CAMERA);

	createAttribute(ATTRIBUTE_CAMERA_DEPTH_MIN, Attribute::types::DOUBLE);
	createAttribute(ATTRIBUTE_CAMERA_DEPTH_MAX, Attribute::types::DOUBLE);
	createAttribute(ATTRIBUTE_CAMERA_FOV, Attribute::types::DOUBLE);

	createAttribute(ATTRIBUTE_CAMERA_RATIO_WIDTH, Attribute::types::INTEGER);
	createAttribute(ATTRIBUTE_CAMERA_RATIO_HEIGHT, Attribute::types::INTEGER);

	setAttribute(ATTRIBUTE_CAMERA_FOV, 45);
	setAttribute(ATTRIBUTE_CAMERA_RATIO_WIDTH, 1);
	setAttribute(ATTRIBUTE_CAMERA_RATIO_HEIGHT, 1);

	addRequestedPointer(PTR_IDENTIFIER::WINDOW_PTR, &windowPtr);

}
Object_Camera::~Object_Camera() {}


glm::mat4 Object_Camera::getTransformationMatrix(glm::mat4 childTransform) {
	return childTransform;
}

glm::mat4 Object_Camera::getCameraTransformationMatrix() {
	glm::mat4 cameraTransformationMat;
	
	// world scale
	glm::mat4 scaleMat = glm::scale(glm::mat4(1.0), glm::vec3(
		1 / (float)getAttribute(ATTRIBUTE_SPATIAL_SCALE_X),
		1 / (float)getAttribute(ATTRIBUTE_SPATIAL_SCALE_Y),
		1 / (float)getAttribute(ATTRIBUTE_SPATIAL_SCALE_Z)
	));
	
	// camera view matrix
	glm::vec3 posV = {
		(float)getAttribute(ATTRIBUTE_SPATIAL_POSITION_X),
		(float)getAttribute(ATTRIBUTE_SPATIAL_POSITION_Y),
		(float)getAttribute(ATTRIBUTE_SPATIAL_POSITION_Z) };
	
	// create view matrix without glm::lookAt

	// translate world with opposite position of camera
	glm::mat4 viewTranslateMat = glm::translate(glm::mat4(1.0f), -posV); 

	// rotate world in opposite rotation of camera
	glm::mat4 viewRotateMat = glm::mat4(1.0f);
	viewRotateMat = glm::rotate(glm::mat4(1.0f), glm::radians((float)getAttribute(ATTRIBUTE_SPATIAL_ROTATE_NOD)), { 1,0,0 }) * viewRotateMat;
	viewRotateMat = glm::rotate(glm::mat4(1.0f), glm::radians((float)getAttribute(ATTRIBUTE_SPATIAL_ROTATE_TURN)), {0,1,0}) * viewRotateMat;
	glm::vec3 viewAxis = viewRotateMat * glm::vec4{ 0,0,1,0 };
	viewRotateMat = glm::rotate(glm::mat4(1.0f), glm::radians((float)getAttribute(ATTRIBUTE_SPATIAL_ROTATE_TILT)), viewAxis) * viewRotateMat;
	viewRotateMat = glm::inverse(viewRotateMat);

	glm::mat4 viewMat = viewRotateMat * viewTranslateMat;
	
	// projection onto window matrix
	glm::mat4 projectionMat = glm::mat4(1.0f);
	
	// perspective camera
	int fWidth = getAttribute(ATTRIBUTE_CAMERA_RATIO_WIDTH);
	int fHeight = getAttribute(ATTRIBUTE_CAMERA_RATIO_HEIGHT);
	if (!fWidth || !fHeight) glfwGetFramebufferSize(*windowPtr, &fWidth, &fHeight);
	if (fWidth != 0 && fHeight != 0) {
		projectionMat = glm::perspective(
			glm::radians((float)getAttribute(ATTRIBUTE_CAMERA_FOV)), (float)fWidth / fHeight,
			(float)getAttribute(ATTRIBUTE_CAMERA_DEPTH_MIN), (float)getAttribute(ATTRIBUTE_CAMERA_DEPTH_MAX)
		);
	}

	cameraTransformationMat = projectionMat * viewMat * scaleMat;
	return cameraTransformationMat;
}