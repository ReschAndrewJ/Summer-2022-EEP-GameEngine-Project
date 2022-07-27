#ifndef OBJECT_BASE
#define OBJECT_BASE OBJECT_BASE 

#include <string>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <memory>
#include <tuple>

#include "../../Attribute/Attribute.h"

#include "../../Engines/Main/PTR_IDENTIFIER_ENUMS.h"

const auto OBJECT_CLASS_BASE = "Object_Base";

class Object
{
	friend class Main_Engine; // the main engine owns and manages the objects
public:
	Object();
	virtual ~Object();
private:
	// the member attributes of the object 
	std::unordered_map<std::string, Attribute> attributes;
	// the object's string identifier
	std::string identifier;
	// the string identifier of the parent object
	std::string parent;
	// string identifiers of the children objects
	std::unordered_set<std::string> children;

	std::set<std::pair<int, void(*)(Object*, float)>> process_functions; // <priority, func>, lower values run first
	std::unordered_set<void(*)(Object*)> afterCreation_functions;
	std::unordered_set<void(*)(Object*)> beforeDestruction_functions;
	std::unordered_set<std::string> class_identifiers;
	

	std::unordered_map<std::string, Object*>* objectContainerPtr;
	/* points to the object creation queue in the main engine
	<string filepath, string objectName, string objectParent>*/
	std::vector<std::tuple<std::string, std::string, std::string, std::vector<std::pair<std::string, Attribute>>>>* objectCreationQueuePtr;
	/* points to the object destruction queue in the main engine */
	std::unordered_set<std::string>* objectDestructionQueuePtr;

	std::vector<std::pair<PTR_IDENTIFIER, void*>> requestedPointers;

protected:
	/* insert a new attribute into the attributes map,
	should be called in the class constructor */
	void createAttribute(std::string attributeName, Attribute::types attribute_type);

	/* assigns an identification string for the object class,
	should be called in the class constructor */
	void addClassIdentifier(std::string identifier);

	/* adds a pointer to be requested from the main engine when the object is created,
	addRequestedPonter(Identifier, T** ptr),
	should be called in the class constructor */
	void addRequestedPointer(PTR_IDENTIFIER, void*);

	/* adds a function to be called every frame,
	should be called in the class constructor,
	functions with lower priority values run first */
	void addProcessFunction(void(*func)(Object*, float), int priority);

	/* adds a function to be called after the creation step of the frame it is created during,
	should be called in the class constructor */
	void addAfterCreationFunction(void(*func)(Object*));

	/* adds a function to be called right before the destriction step of the frame it is destroyed during,
	should be called in the class constructor */
	void addBeforeDestructionFunction(void(*func)(Object*));

public:
	
	Attribute getAttribute(std::string) const;
	void setAttribute(std::string, Attribute);
	
	// returns true is the object has the given class identifier
	bool is_class(std::string class_identifier) const;

	// returns the object's string identifier
	std::string getIdentifier() const;

	// returns the string identifier of the object's parent object
	std::string getParentIdentifier() const;

	// returns the set of string identifiers of the object's child objects
	const std::unordered_set<std::string>& getChildrenIdentifiers() const;

	// returns a pointer to the requested object in the objects container
	Object* getObject(std::string objectName) const;

	// adds an object to the object creation queue in the main engine
	void queueCreateObject(std::string filepath, std::string objectName, std::string objectParent, std::vector<std::pair<std::string, Attribute>> modifiedAttributes = {});

	// adds an object to the object destruction queue in the main engine
	void queueDestroyObject(std::string objectIdentifier);

	// changes objects' identifiers such that newParentIdentifier becomes the object's parent
	void changeParentObject(std::string newParentIdentifier);

};

#endif //OBJECT_BASE