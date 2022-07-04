#ifdef UNIT_TESTS

#ifndef OBJECT_TEST1
#define OBJECT_TEST1 OBJECT_TEST1

const auto OBJECT_CLASS_TEST1 = "Object_Test1";

#include "../Base/Object.h"
class Object_Test1 : virtual public Object
{
public:
	Object_Test1();

};

#endif
#endif