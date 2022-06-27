#ifndef OBJECT_COLLISION
#define OBJECT_COLLISION


#include "../Spatial/Object_Spatial.h"



const auto OBJECT_CLASS_COLLISION = "Object_Collision";

// the explicit dimensions of the collision object, are influenced by SpatialScale

const auto ATTRIBUTE_COLLIDER_WIDTH = "ColliderWidth";
const auto ATTRIBUTE_COLLIDER_HEIGHT = "ColliderHeight";
const auto ATTRIBUTE_COLLIDER_DEPTH = "ColliderDepth";

// a dimension can be ignored, such that if two object overlap in all non-ignored dimension
// they are treated as colliding

const auto ATTRIBUTE_COLLIDER_IGNORE_Z = "ColliderIgnoreZ";
const auto ATTRIBUTE_COLLIDER_IGNORE_Y = "ColliderIgnoreY";
const auto ATTRIBUTE_COLLIDER_IGNORE_X = "ColliderIgnoreX";

class Object_Collision : public Object_Spatial {

protected:
	std::unordered_map<std::string, Object*>* objectContainerPtr;


public:
	Object_Collision();
	virtual ~Object_Collision();


	std::vector<std::string> getCollidingObjects();

protected:
	virtual bool checkIsColliding(Object_Collision*);

private:
	bool checkforAlignedBBColision(Object_Collision*);
	bool checkforUnalignedBBCollision(Object_Collision*);

	// returns the aligned bounding box of the object, minx maxx miny maxy minz maxz
	void getBoundingPoints(float returnArray[6]);


};


#endif //OBJECT_COLLISION
