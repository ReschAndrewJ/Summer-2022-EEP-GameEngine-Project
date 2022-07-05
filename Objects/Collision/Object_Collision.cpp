#include "Object_Collision.h"

#include "glm/glm.hpp"

Object_Collision::Object_Collision() {
	addClassIdentifier(OBJECT_CLASS_COLLISION);

	createAttribute(ATTRIBUTE_COLLIDER_WIDTH, Attribute::types::DOUBLE);
	createAttribute(ATTRIBUTE_COLLIDER_DEPTH, Attribute::types::DOUBLE);
	createAttribute(ATTRIBUTE_COLLIDER_HEIGHT, Attribute::types::DOUBLE);

	/*
	createAttribute(ATTRIBUTE_COLLIDER_IGNORE_Z, Attribute::types::BOOLEAN);
	createAttribute(ATTRIBUTE_COLLIDER_IGNORE_Y, Attribute::types::BOOLEAN);
	createAttribute(ATTRIBUTE_COLLIDER_IGNORE_X, Attribute::types::BOOLEAN);
	*/

	createAttribute(ATTRIBUTE_COLLIDER_DETECTABLE, Attribute::types::BOOLEAN);
	setAttribute(ATTRIBUTE_COLLIDER_DETECTABLE, true);
	
	addRequestedPointer(PTR_IDENTIFIER::OBJ_CONTAINER_PTR, &objectContainerPtr);
}

Object_Collision::~Object_Collision() {}


std::vector<std::string> Object_Collision::getCollidingObjects() {
	std::vector<std::string> collidingObjs;

	for (auto& obj : *objectContainerPtr) {
		if (!obj.second->is_class(OBJECT_CLASS_COLLISION) 
			|| !obj.second->getAttribute(ATTRIBUTE_COLLIDER_DETECTABLE)
			|| obj.first == getIdentifier()) continue;

		if (checkIsColliding(dynamic_cast<Object_Collision*>(obj.second))) collidingObjs.push_back(obj.first);
	}

	return collidingObjs;
}


bool Object_Collision::checkIsColliding(Object_Collision* collider) {
	// if the aligned bounding boxes collide, check if the actual collision boxes collide
	return checkforAlignedBBColision(collider) && halfCollisionCheck(collider) && collider->halfCollisionCheck(this);
}


bool Object_Collision::checkforAlignedBBColision(Object_Collision* collider) {
	float ownBoundingPoints[6]{};
	float colliderBoundingPoints[6]{};

	getBoundingPoints(ownBoundingPoints);
	collider->getBoundingPoints(colliderBoundingPoints);

	// x-seperation axis
	if (ownBoundingPoints[0] >= colliderBoundingPoints[1]
		|| colliderBoundingPoints[0] >= ownBoundingPoints[1]) return false;
	// y-seperation axis
	if (ownBoundingPoints[2] >= colliderBoundingPoints[3]
		|| colliderBoundingPoints[2] >= ownBoundingPoints[3]) return false;
	// z-seperation axis
	if (ownBoundingPoints[4] >= colliderBoundingPoints[5]
		|| colliderBoundingPoints[4] >= ownBoundingPoints[5]) return false;
	return true;
}


void Object_Collision::getBoundingPoints(float returnArray[6]) {
	glm::mat4 transform = getTransformationMatrix();

	// get collider points
	glm::vec3 point1 = {
		(float)getAttribute(ATTRIBUTE_COLLIDER_WIDTH) / 2,
		(float)getAttribute(ATTRIBUTE_COLLIDER_HEIGHT) / 2,
		(float)getAttribute(ATTRIBUTE_COLLIDER_DEPTH) / 2
	};
	glm::vec3 p[7] = {
		{point1.x, point1.y, -point1.z},
		{ point1.x, -point1.y, point1.z },
		{ point1.x, -point1.y, -point1.z },
		{ -point1.x, point1.y, point1.z },
		{ -point1.x, point1.y, -point1.z },
		{ -point1.x, -point1.y, point1.z },
		{ -point1.x, -point1.y, -point1.z }
	};

	// transform collider points
	point1 = transform * glm::vec4{ point1, 1.0f };
	for (int i = 0; i < 7; ++i) {
		p[i] = transform * glm::vec4{ p[i], 1.0f };
	}

	// find min and max bounds for collider points
	for (int i = 0; i < 3; ++i) {
		returnArray[2*i] = point1[i];
		returnArray[2*i + 1] = point1[i];
		for (int j = 0; j < 7; ++j) {
			if (p[j][i] < returnArray[2*i]) returnArray[2*i] = p[j][i];
			else if (p[j][i] > returnArray[2*i + 1]) returnArray[2*i + 1] = p[j][i];
		}
	}
}


bool Object_Collision::halfCollisionCheck(Object_Collision* collider) {
	glm::mat4 transform = glm::inverse(getTransformationMatrix()) * collider->getTransformationMatrix();
	
	glm::vec3 point1 = {
		(float)collider->getAttribute(ATTRIBUTE_COLLIDER_WIDTH) / 2,
		(float)collider->getAttribute(ATTRIBUTE_COLLIDER_HEIGHT) / 2,
		(float)collider->getAttribute(ATTRIBUTE_COLLIDER_DEPTH) / 2
	};
	glm::vec3 p[7] = {
		{point1.x, point1.y, -point1.z},
		{ point1.x, -point1.y, point1.z },
		{ point1.x, -point1.y, -point1.z },
		{ -point1.x, point1.y, point1.z },
		{ -point1.x, point1.y, -point1.z },
		{ -point1.x, -point1.y, point1.z },
		{ -point1.x, -point1.y, -point1.z }
	};
	point1 = transform * glm::vec4{ point1, 1.0f };
	for (int i = 0; i < 7; ++i) {
		p[i] = transform * glm::vec4{ p[i], 1.0f };
	}
	
	float colliderBounds[6]{ 0 };
	for (int i = 0; i < 3; ++i) {
		colliderBounds[2 * i] = point1[i];
		colliderBounds[2 * i + 1] = point1[i];
		for (int j = 0; j < 7; ++j) {
			if (p[j][i] < colliderBounds[2 * i]) colliderBounds[2 * i] = p[j][i];
			else if (p[j][i] > colliderBounds[2 * i + 1]) colliderBounds[2 * i + 1] = p[j][i];
		}
	}
	
	float ownX = getAttribute(ATTRIBUTE_COLLIDER_WIDTH);
	float ownY = getAttribute(ATTRIBUTE_COLLIDER_HEIGHT);
	float ownZ = getAttribute(ATTRIBUTE_COLLIDER_DEPTH);
	
	// x-seperation axis
	if (colliderBounds[0] >= ownX / 2
		|| colliderBounds[1] <= -ownX / 2) return false;
	// y-seperation axis
	if (colliderBounds[2] >= ownY / 2
		|| colliderBounds[3] <= -ownY / 2) return false;
	// z-seperation axis
	if (colliderBounds[4] >= ownZ / 2
		|| colliderBounds[5] <= -ownZ / 2) return false;
	return true;
}