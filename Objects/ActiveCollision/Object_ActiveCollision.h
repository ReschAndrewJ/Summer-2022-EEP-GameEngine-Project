#ifndef OBJECT_ACTIVECOLLISION
#define OBJECT_ACTIVECOLLISION

#include "../Collision/Object_Collision.h"


const auto OBJECT_CLASS_ACTIVECOLLISION = "Object_ActiveCollision";


class Object_ActiveCollision : virtual public Object_Collision {
public:
	Object_ActiveCollision();
	virtual ~Object_ActiveCollision();


public:
	/* causes the activeCollision object to move in the direction of movXYZ,
	with a speed of ||XYZ|| * magnitude * delta, returns the vector of colliding objects, stops on collision */
	std::vector<std::string> move(double movX, double movY, double movZ, float delta, double magnitude=1);





};


#endif //OBJECT_ACTIVECOLLISION