#include "Object_Collision.h"

#include "glm/glm.hpp"

Object_Collision::Object_Collision() {
	addClassIdentifier(OBJECT_CLASS_COLLISION);

	createAttribute(ATTRIBUTE_COLLIDER_WIDTH, Attribute::types::DOUBLE);
	createAttribute(ATTRIBUTE_COLLIDER_DEPTH, Attribute::types::DOUBLE);
	createAttribute(ATTRIBUTE_COLLIDER_HEIGHT, Attribute::types::DOUBLE);

	createAttribute(ATTRIBUTE_COLLIDER_IGNORE_Z, Attribute::types::BOOLEAN);
	createAttribute(ATTRIBUTE_COLLIDER_IGNORE_Y, Attribute::types::BOOLEAN);
	createAttribute(ATTRIBUTE_COLLIDER_IGNORE_X, Attribute::types::BOOLEAN);

	
	addRequestedPointer(PTR_IDENTIFIER::OBJ_CONTAINER_PTR, &objectContainerPtr);
}

Object_Collision::~Object_Collision() {}


std::vector<std::string> Object_Collision::getCollidingObjects() {
	std::vector<std::string> collidingObjs;

	for (auto& obj : *objectContainerPtr) {
		if (obj.first == getIdentifier() || !obj.second->is_class(OBJECT_CLASS_COLLISION)) continue;

		if (checkIsColliding((Object_Collision*)obj.second)) collidingObjs.push_back(obj.first);
	}

	return collidingObjs;
}


bool Object_Collision::checkIsColliding(Object_Collision* collider) {
	// if the aligned bounding boxes collide, check if the actual collision boxes collide
	return checkforAlignedBBColision(collider) && checkforUnalignedBBCollision(collider);
}


bool Object_Collision::checkforAlignedBBColision(Object_Collision* collider) {
	float ownBoundingPoints[6]{};
	float colliderBoundingPoints[6]{};

	getBoundingPoints(ownBoundingPoints);
	collider->getBoundingPoints(colliderBoundingPoints);

	// x-seperation axis
	if (!(bool)getAttribute(ATTRIBUTE_COLLIDER_IGNORE_X) 
		&& !(bool)collider->getAttribute(ATTRIBUTE_COLLIDER_IGNORE_X)
		&& ownBoundingPoints[0] > colliderBoundingPoints[1]
		|| colliderBoundingPoints[0] > ownBoundingPoints[1]) return false;
	// y-seperation axis
	if (!(bool)getAttribute(ATTRIBUTE_COLLIDER_IGNORE_Y)
		&& !(bool)collider->getAttribute(ATTRIBUTE_COLLIDER_IGNORE_Y)
		&& ownBoundingPoints[2] > colliderBoundingPoints[3]
		|| colliderBoundingPoints[2] > ownBoundingPoints[3]) return false;
	// z-seperation axis
	if (!(bool)getAttribute(ATTRIBUTE_COLLIDER_IGNORE_Z)
		&& !(bool)collider->getAttribute(ATTRIBUTE_COLLIDER_IGNORE_Z)
		&& ownBoundingPoints[4] > colliderBoundingPoints[5]
		|| colliderBoundingPoints[4] > ownBoundingPoints[5]) return false;
	return true;
}


bool Object_Collision::checkforUnalignedBBCollision(Object_Collision* collider) {
	// get object points
	glm::vec3 points[8]{};
	points[0] = {
		(float)getAttribute(ATTRIBUTE_COLLIDER_WIDTH) / 2,
		(float)getAttribute(ATTRIBUTE_COLLIDER_HEIGHT) / 2,
		(float)getAttribute(ATTRIBUTE_COLLIDER_DEPTH) / 2
	};
	for (int i = 1; i < 8; ++i) {
		points[i][0] = points[0][0] * (i % 2 * 2 - 1) * -1;
		points[i][1] = points[0][1] * (i / 2 % 2 * 2 - 1) * -1;
		points[i][2] = points[0][2] * (i / 4 % 2 * 2 - 1) * -1;
	}
	glm::mat4 transform = getTransformationMatrix();
	for (int i = 0; i < 8; ++i) points[i] = transform * glm::vec4{ points[i], 1.0f };

	// get collider points
	glm::vec3 cPoints[2]{};
	cPoints[0] = {
		-(float)collider->getAttribute(ATTRIBUTE_COLLIDER_WIDTH) / 2,
		-(float)collider->getAttribute(ATTRIBUTE_COLLIDER_HEIGHT) / 2,
		-(float)collider->getAttribute(ATTRIBUTE_COLLIDER_DEPTH) / 2
	};
	cPoints[1] = -cPoints[0];

	glm::mat4 cTransform = collider->getTransformationMatrix();
	glm::mat4 cInvTransform = glm::inverse(cTransform);

	// flatten ignored dimensions
	if ((bool)getAttribute(ATTRIBUTE_COLLIDER_IGNORE_X)
		|| (bool)collider->getAttribute(ATTRIBUTE_COLLIDER_IGNORE_X)
		|| (bool)getAttribute(ATTRIBUTE_COLLIDER_IGNORE_Y)
		|| (bool)collider->getAttribute(ATTRIBUTE_COLLIDER_IGNORE_Y)
		|| (bool)getAttribute(ATTRIBUTE_COLLIDER_IGNORE_Z)
		|| (bool)collider->getAttribute(ATTRIBUTE_COLLIDER_IGNORE_Z)
		) {

		// align cPoints to global
		for (int i = 0; i < 2; ++i) cPoints[i] = cTransform * glm::vec4{ cPoints[i], 1.0f };

		if ((bool)getAttribute(ATTRIBUTE_COLLIDER_IGNORE_X)
			|| (bool)collider->getAttribute(ATTRIBUTE_COLLIDER_IGNORE_X)) {
			for (int i = 0; i < 8; ++i) points[i][0] = 0.0f;
			for (int i = 0; i < 2; ++i) cPoints[i][0] = 0.0f;
		}
		if ((bool)getAttribute(ATTRIBUTE_COLLIDER_IGNORE_Y)
			|| (bool)collider->getAttribute(ATTRIBUTE_COLLIDER_IGNORE_Y)) {
			for (int i = 0; i < 8; ++i) points[i][1] = 0.0f;
			for (int i = 0; i < 2; ++i) cPoints[i][1] = 0.0f;
		}if ((bool)getAttribute(ATTRIBUTE_COLLIDER_IGNORE_Z)
			|| (bool)collider->getAttribute(ATTRIBUTE_COLLIDER_IGNORE_Z)) {
			for (int i = 0; i < 8; ++i) points[i][2] = 0.0f;
			for (int i = 0; i < 2; ++i) cPoints[i][2] = 0.0f;
		}

		// return cPoints to collider alignment
		for (int i = 0; i < 2; ++i) cPoints[i] = cInvTransform * glm::vec4{ cPoints[i], 1.0f };
	}

	// align points to collider
	for (int i = 0; i < 8; ++i) points[i] = cInvTransform * glm::vec4{ points[i], 1.0f };

	// check if any point in object is inside collider's space
	for (int i = 0; i < 8; ++i) {
		if (points[i][0] >= cPoints[0][0] && points[i][0] <= cPoints[1][0]
			&& points[i][1] >= cPoints[0][1] && points[i][1] <= cPoints[1][1]
			&& points[i][2] >= cPoints[0][2] && points[i][2] <= cPoints[1][2]) return true;
	}
	return false;
}



void Object_Collision::getBoundingPoints(float returnArray[6]) {
	glm::mat4 transform = getTransformationMatrix();
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

	point1 = transform * glm::vec4{ point1, 1.0f };
	for (int i = 0; i < 7; ++i) {
		p[i] = transform * glm::vec4{ p[i], 1.0f };
	}

	for (int i = 0; i < 3; ++i) {
		returnArray[2*i] = point1[i];
		returnArray[2*i + 1] = point1[i];
		for (int j = 0; j < 7; ++j) {
			if (p[j][i] < returnArray[2*i]) returnArray[2*i] = p[j][i];
			else if (p[j][i] > returnArray[2*i + 1]) returnArray[2*i + 1] = p[i][j];
		}
	}
}