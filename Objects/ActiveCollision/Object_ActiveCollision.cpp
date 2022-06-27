#include "Object_ActiveCollision.h"


Object_ActiveCollision::Object_ActiveCollision() {
	addClassIdentifier(OBJECT_CLASS_ACTIVECOLLISION);
}
Object_ActiveCollision::~Object_ActiveCollision() {}


std::vector<std::string> Object_ActiveCollision::move(double movX, double movY, double movZ, float delta, double magnitude) {

	// save current position
	double currentX = getAttribute(ATTRIBUTE_SPATIAL_POSITION_X);
	double currentY = getAttribute(ATTRIBUTE_SPATIAL_POSITION_Y);
	double currentZ = getAttribute(ATTRIBUTE_SPATIAL_POSITION_Z);

	// set target position
	double targetX = movX * magnitude * delta;
	double targetY = movX * magnitude * delta;
	double targetZ = movZ * magnitude * delta;

	setAttribute(ATTRIBUTE_SPATIAL_POSITION_X, targetX);
	setAttribute(ATTRIBUTE_SPATIAL_POSITION_Y, targetY);
	setAttribute(ATTRIBUTE_SPATIAL_POSITION_Z, targetZ);

	// check collisions and possible rollback
	auto collisions = getCollidingObjects();
	if (collisions.size() > 0) {
		setAttribute(ATTRIBUTE_SPATIAL_POSITION_X, currentX);
		setAttribute(ATTRIBUTE_SPATIAL_POSITION_Y, currentY);
		setAttribute(ATTRIBUTE_SPATIAL_POSITION_Z, currentZ);
	}
	return collisions;
}