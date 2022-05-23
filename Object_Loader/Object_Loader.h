#ifndef OBJECT_LOADER
#define OBJECT_LOADER

#include "../Attribute/Attribute.h"

#include <string>
#include <vector>


// struct with the information required to instance an object
struct Object_Info {
	std::string name_id; // the identifier for the object
	std::string class_identifier; // the identifier for the object's class
	std::vector<std::pair<std::string, Attribute>> attributes; // list of non-default attributes to fill the object when instanced
	std::vector<std::pair<std::string/*filepath*/, std::string/*child name_id*/>> children; // list of objects to instance as children of the object's instance
};




class Object_Loader
{






};



#endif // OBJECT_LOADER