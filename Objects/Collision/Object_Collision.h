#ifndef OBJECT_COLLISION
#define OBJECT_COLLISION


#include "../Spatial/Object_Spatial.h"


const auto OBJECT_CLASS_COLLISION = "Object_Collision";

// the explicit dimensions of the collision object, are influenced by SpatialScale

const auto ATTRIBUTE_COLLIDER_WIDTH = "ColliderWidth";
const auto ATTRIBUTE_COLLIDER_HEIGHT = "ColliderHeight";
const auto ATTRIBUTE_COLLIDER_DEPTH = "ColliderDepth";

/* MEANING AMBIGUOUS, MEANING COULD CHANGE WITH PARENT OBJECT TRANSFORMATIONS
// a dimension can be ignored, such that if two object overlap in all non-ignored dimension
// they are treated as colliding
const auto ATTRIBUTE_COLLIDER_IGNORE_Z = "ColliderIgnoreZ";
const auto ATTRIBUTE_COLLIDER_IGNORE_Y = "ColliderIgnoreY";
const auto ATTRIBUTE_COLLIDER_IGNORE_X = "ColliderIgnoreX";
*/

// a non-detectable collision object will be excluded from the collision tests of 
// other collision objects

const auto ATTRIBUTE_COLLIDER_DETECTABLE = "ColliderDetectable";

class Object_Collision : virtual public Object_Spatial {
protected:
	std::unordered_map<std::string, Object*>* objectContainerPtr;


public:
	Object_Collision();
	virtual ~Object_Collision();


	std::vector<std::string> getCollidingObjects();

protected:
	virtual bool checkIsColliding(Object_Collision*);

private:
	bool halfCollisionCheck(Object_Collision*);

	bool checkforAlignedBBColision(Object_Collision* collider);
	void getBoundingPoints(float returnArray[6]);

	

};


#endif //OBJECT_COLLISION
