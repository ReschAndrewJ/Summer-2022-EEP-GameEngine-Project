#include "Object_Spatial.h"

#include "glm/gtc/matrix_transform.hpp"


Object_Spatial::Object_Spatial() {
	addClassIdentifier(OBJECT_CLASS_SPATIAL);

	createAttribute(ATTRIBUTE_SPATIAL_POSITION_X, Attribute::types::DOUBLE);
	createAttribute(ATTRIBUTE_SPATIAL_POSITION_Y, Attribute::types::DOUBLE);
	createAttribute(ATTRIBUTE_SPATIAL_POSITION_Z, Attribute::types::DOUBLE);

	createAttribute(ATTRIBUTE_SPATIAL_ROTATE_NOD, Attribute::types::DOUBLE);
	createAttribute(ATTRIBUTE_SPATIAL_ROTATE_TURN, Attribute::types::DOUBLE);
	createAttribute(ATTRIBUTE_SPATIAL_ROTATE_TILT, Attribute::types::DOUBLE);

	createAttribute(ATTRIBUTE_SPATIAL_SCALE_X, Attribute::types::DOUBLE);
	createAttribute(ATTRIBUTE_SPATIAL_SCALE_Y, Attribute::types::DOUBLE);
	createAttribute(ATTRIBUTE_SPATIAL_SCALE_Z, Attribute::types::DOUBLE);
	setAttribute(ATTRIBUTE_SPATIAL_SCALE_X, 1);
	setAttribute(ATTRIBUTE_SPATIAL_SCALE_Y, 1);
	setAttribute(ATTRIBUTE_SPATIAL_SCALE_Z, 1);

	createAttribute(ATTRIBUTE_SPATIAL_PARENT_TRANSFORMATIONS_INHERIT, Attribute::types::BOOLEAN);
	setAttribute(ATTRIBUTE_SPATIAL_PARENT_TRANSFORMATIONS_INHERIT, true);
}
Object_Spatial::~Object_Spatial() {}



glm::mat4 Object_Spatial::getTransformationMatrix(glm::mat4 childTransform) {
	glm::vec3 posV = {
		(float)getAttribute(ATTRIBUTE_SPATIAL_POSITION_X),
		(float)getAttribute(ATTRIBUTE_SPATIAL_POSITION_Y),
		(float)getAttribute(ATTRIBUTE_SPATIAL_POSITION_Z) };
	
	glm::mat4 translationMat = glm::translate(glm::mat4(1.0), posV);
	
	// nod -> turn -> tilt
	glm::mat4 rotationMat = glm::mat4(1.0f);
	
	rotationMat = glm::rotate(glm::mat4(1), glm::radians((float)getAttribute(ATTRIBUTE_SPATIAL_ROTATE_NOD)), {1,0,0}) * rotationMat;
	rotationMat = glm::rotate(glm::mat4(1), glm::radians((float)getAttribute(ATTRIBUTE_SPATIAL_ROTATE_TURN)), {0,1,0}) * rotationMat;
	glm::vec3 viewAxis = rotationMat * glm::vec4{ 0,0,1,0 };
	rotationMat = glm::rotate(glm::mat4(1), glm::radians((float)getAttribute(ATTRIBUTE_SPATIAL_ROTATE_TILT)), viewAxis) * rotationMat;


	glm::mat4 scaleMat = glm::scale(glm::mat4(1.0), glm::vec3(
		(float)getAttribute(ATTRIBUTE_SPATIAL_SCALE_X),
		(float)getAttribute(ATTRIBUTE_SPATIAL_SCALE_Y),
		(float)getAttribute(ATTRIBUTE_SPATIAL_SCALE_Z)
	));
	glm::mat4 transformationMat = translationMat * rotationMat * scaleMat * childTransform;
	
	if ((bool)getAttribute(ATTRIBUTE_SPATIAL_PARENT_TRANSFORMATIONS_INHERIT) && !getParentIdentifier().empty()
		&& getObject(getParentIdentifier())->is_class(OBJECT_CLASS_SPATIAL)) {
		return (dynamic_cast<Object_Spatial*>(
			getObject(getParentIdentifier()))
			)->getTransformationMatrix(transformationMat);
	}
	return transformationMat;
}


glm::mat4 Object_Spatial::rotateToVec(glm::vec3 rotateFrom, glm::vec3 rotateTo) {
	
	// Rotation Matrix - https://math.stackexchange.com/a/897677
	//
	//  cos() -sin() 0
	//  sin()  cos() 0
	//    0      0   1
	// 
	//  A X B = ||A||*||B||*sin()*uv
	//  ||AxB|| = ||A||*||B||*sin()
	//  with normalized A&B
	//	sin() = ||AxB||
	// 
	//  A * B = ||A||*||B||*cos()
	//  with normalized A&B
	//  cos() = A*B
	//

	glm::mat4 rotationMat = glm::mat4(1.0f);

	rotateFrom = glm::normalize(rotateFrom);
	rotateTo = glm::normalize(rotateTo);

	float cosine = glm::dot(rotateFrom, rotateTo);
	if (cosine != 1 && cosine != -1) {

		float sine = glm::length(glm::cross(rotateFrom, rotateTo));

		glm::mat3 tempRotMat = { {cosine, sine, 0}, {-sine, cosine, 0}, {0, 0, 1} };

		// change of basis
		glm::mat3 FInv = { rotateFrom, (rotateTo - cosine * rotateFrom) / glm::length(rotateTo - cosine * rotateFrom), glm::cross(rotateTo, rotateFrom) };
		rotationMat = FInv * tempRotMat * glm::inverse(FInv);
	}
	else if (cosine == -1) {
		rotationMat = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), { 0.0f, 1.0f, 0.0f });
	}

	return rotationMat;
}