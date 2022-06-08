#ifndef OBJECT_LOADER
#define OBJECT_LOADER

#include "../Attribute/Attribute.h"
#include "../Objects/Base/Object.h"

#include <string>
#include <vector>
#include <unordered_map>


// struct with the information required to instance an object
struct Object_Info {
	std::string name_id; // the identifier for the object
	std::string class_identifier; // the identifier for the object's class
	std::vector<std::pair<std::string, Attribute>> attributes; // list of non-default attributes to fill the object when instanced
	std::vector<std::pair<std::string/*filepath*/, std::string/*child name_id*/>> children; // list of objects to instance as children of the object's instance
};




class Object_Loader
{
private:
	std::unordered_map<std::string/*filepath+/+identifier*/, Object_Info> loadedObjectInfos;
	std::unordered_set<std::string/*filepath*/> loadedFiles;

public:
	/* creates an object using the instancing data from a .inst file
	pair<Object* newObject, vector<pair<string filepath, string identifier>> children> createInstance(string filepath, string objectIdentifier) */
	std::pair<Object*, std::vector<std::pair<std::string, std::string>>> createInstance(std::string filepath, std::string objectIdentifier);


private:
	/* loads a .inst file */
	void loadDataFromInstFile(std::string filepath);

};



#endif // OBJECT_LOADER