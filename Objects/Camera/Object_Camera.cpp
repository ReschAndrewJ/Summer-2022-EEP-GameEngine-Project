#include "Object_Camera.h"


#include "glm/gtc/matrix_transform.hpp"


Object_Camera::Object_Camera() {
	addClassIdentifier(OBJECT_CLASS_CAMERA);

	createAttribute(ATTRIBUTE_CAMERA_DEPTH_MIN, Attribute::types::DOUBLE);
	createAttribute(ATTRIBUTE_CAMERA_DEPTH_MAX, Attribute::types::DOUBLE);
	createAttribute(ATTRIBUTE_CAMERA_WIDTH, Attribute::types::DOUBLE);
	createAttribute(ATTRIBUTE_CAMERA_HEIGHT, Attribute::types::DOUBLE);
	createAttribute(ATTRIBUTE_CAMERA_ORTHOGRAPHIC, Attribute::types::BOOLEAN);
	setAttribute(ATTRIBUTE_CAMERA_ORTHOGRAPHIC, false);
	createAttribute(ATTRIBUTE_CAMERA_FOV, Attribute::types::DOUBLE);

	addRequestedPointer(PTR_IDENTIFIER::WINDOW_PTR, &windowPtr);

	setAttribute(ATTRIBUTE_SPATIAL_FACING_Z, -1.0); // facing negative => lower values are farther from the camera
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
	glm::vec3 facingV = {
		(float)getAttribute(ATTRIBUTE_SPATIAL_FACING_X),
		(float)getAttribute(ATTRIBUTE_SPATIAL_FACING_Y),
		(float)getAttribute(ATTRIBUTE_SPATIAL_FACING_Z) };
	/*
	glm::vec3 upV = { 0.0f, 1.0f, 0.0f };
	// rotate the up vector about the facing vector by the set angle
	upV = glm::rotate(glm::mat4(1.0f), glm::radians((float)getAttribute(ATTRIBUTE_SPATIAL_FACING_ROTATION)), facingV) * glm::vec4{ upV, 0 };
	glm::mat4 viewMat = glm::lookAt(posV, { facingV.x + posV.x, facingV.y + posV.y, facingV.z + posV.z }, upV);
	*/
	// create view matrix without glm::lookAt

	// translate world with opposite position of camera
	glm::mat4 viewTranslateMat = glm::translate(glm::mat4(1.0f), -posV); 

	glm::vec3 rotateFrom = glm::vec3{ 0.0f, 0.0f, -1.0f }; // rotate from default orientation
	
	// rotate world in opposite rotation of camera
	glm::mat4 viewRotateMat = rotateToVec(facingV, rotateFrom); 
	viewRotateMat = glm::rotate(glm::mat4(1.0f), -glm::radians((float)getAttribute(ATTRIBUTE_SPATIAL_FACING_ROTATION)), facingV) * viewRotateMat;

	glm::mat4 viewMat = viewRotateMat * viewTranslateMat;
	
	// projection onto window matrix
	glm::mat4 projectionMat = glm::mat4(1.0f);
	if ((bool)getAttribute(ATTRIBUTE_CAMERA_ORTHOGRAPHIC)) { // orthographic camera
		// -width/2 => -1 width/2 => +1, scale by 2/width
		// depth near => 0 depth far => 1, translate by -mindepth, scale by 1/(maxdepth-mindepth)
		glm::mat4 orthoMat = glm::translate(glm::mat4(1.0f), { 0.0f, 0.0f, -(float)getAttribute(ATTRIBUTE_CAMERA_DEPTH_MIN) });
		glm::vec3 windowScale = {
			2/(float)getAttribute(ATTRIBUTE_CAMERA_WIDTH), 
			2/(float)getAttribute(ATTRIBUTE_CAMERA_HEIGHT),
			-1/((float)getAttribute(ATTRIBUTE_CAMERA_DEPTH_MAX)-(float)getAttribute(ATTRIBUTE_CAMERA_DEPTH_MIN))
		};
		orthoMat = glm::scale(glm::mat4(1.0f), windowScale) * orthoMat;
		

		projectionMat = orthoMat;

		// glm::ortho depth does not work with vulkan's coordinate system
		/*projectionMat = glm::ortho( 
			-(float)getAttribute(ATTRIBUTE_CAMERA_WIDTH) / 2, (float)getAttribute(ATTRIBUTE_CAMERA_WIDTH) / 2,
			-(float)getAttribute(ATTRIBUTE_CAMERA_HEIGHT) / 2, (float)getAttribute(ATTRIBUTE_CAMERA_HEIGHT) / 2,
			(float)getAttribute(ATTRIBUTE_CAMERA_DEPTH_MIN), (float)getAttribute(ATTRIBUTE_CAMERA_DEPTH_MAX)
		);*/
	}
	else { // perspective camera

		int fWidth, fHeight;
		glfwGetFramebufferSize(*windowPtr, &fWidth, &fHeight);
		projectionMat = glm::perspective(
			glm::radians((float)getAttribute(ATTRIBUTE_CAMERA_FOV)), (float)fWidth / fHeight,
			(float)getAttribute(ATTRIBUTE_CAMERA_DEPTH_MIN), (float)getAttribute(ATTRIBUTE_CAMERA_DEPTH_MAX)
		);		
	}

	cameraTransformationMat = projectionMat * viewMat * scaleMat;
	return cameraTransformationMat;
}