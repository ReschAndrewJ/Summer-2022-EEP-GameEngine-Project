#ifndef OBJECT_BASE
// (const char[12])"Object_Base"
#define OBJECT_BASE OBJECT_BASE 

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <tuple>

#include "../../Attribute/Attribute.h"

#include "../../Engines/Main/PTR_IDENTIFIER_ENUMS.h"

const auto OBJECT_BASE = "Object_Base";

class Object
{
	friend class Main_Engine; // the main engine owns and manages the objects
public:
	Object();
private:
	// the member attributes of the object 
	std::unordered_map<std::string, Attribute> attributes;
	// the object's string identifier
	std::string identifier;
	// the string identifier of the parent object
	std::string parent;
	// string identifiers of the children objects
	std::unordered_set<std::string> children;

	std::unordered_set<void(*)(Object*, float)> process_functions;
	std::unordered_set<void(*)(Object*)> afterCreation_functions;
	std::unordered_set<void(*)(Object*)> beforeDestruction_functions;
	std::unordered_set<std::string> class_identifiers;
	

	std::unordered_map<std::string, Object*>* objectContainerPtr;
	/* points to the object creation queue in the main engine
	<string filepath, string objectName, string objectParent>*/
	std::vector<std::tuple<std::string, std::string, std::string>>* objectCreationQueue;
	/* points to the object destruction queue in the main engine */
	std::unordered_set<std::string>* objectDestructionQueue;

	std::unordered_map<PTR_IDENTIFIER, void*> requestedPointers;

protected:
	// insert a new attribute into the attributes map
	void createAttribute(std::string attributeName, Attribute::types attribute_type);

	/* assigns an identification string for the object class,
	should be called in the class constructor */
	void addClassIdentifier(std::string identifier);

	/* adds a pointer to be requested from the main engine when the object is created,
	should be called in the class constructor */
	void addRequestedPointer(PTR_IDENTIFIER, void*);

public:
	
	Attribute getAttribute(std::string);
	void setAttribute(std::string, Attribute);
	
	// returns true is the object has the given class identifier
	bool is_class(std::string class_identifier) const;

	// returns the object's string identifier
	std::string getIdentifier() const;

	// returns the string identifier of the object's parent object
	std::string getParentIdentifier() const;

	// returns the set of string identifiers of the object's child objects
	std::unordered_set<std::string>& getChildrenIdentifiers();

	// returns a reference to the requested object in the objects container
	Object& getObject(std::string objectName);

	// adds an object to the object creation queue in the main engine
	void queueCreateObject(std::string filepath, std::string objectName, std::string objectParent);

	// adds an object to the object destructino queue in the main engine
	void queueDestroyObject(std::string objectIdentifier);

};

#endif //OBJECT_BASE