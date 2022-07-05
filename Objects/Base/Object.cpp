#include "Object.h"



Object::Object() {
	addClassIdentifier(OBJECT_CLASS_BASE);

	addRequestedPointer(PTR_IDENTIFIER::OBJ_CONTAINER_PTR, &objectContainerPtr);
	addRequestedPointer(PTR_IDENTIFIER::OBJ_CREATION_QUEUE_PTR, &objectCreationQueuePtr);
	addRequestedPointer(PTR_IDENTIFIER::OBJ_DESTRUCTION_QUEUE_PTR, &objectDestructionQueuePtr);

	identifier = "unnamed_object";
	parent = "";
}
Object::~Object() {}


void Object::createAttribute(std::string attributeName, Attribute::types type) {
	attributes.insert({ attributeName, Attribute(type) });
}


void Object::addClassIdentifier(std::string identifier) {
	class_identifiers.insert(identifier);
}


void Object::addRequestedPointer(PTR_IDENTIFIER ptrEnum, void* ptr) {
	requestedPointers.push_back({ ptrEnum, ptr });
}


void Object::addProcessFunction(void(*func)(Object*, float), int priority) {
	process_functions.insert({ priority, func });
}


void Object::addAfterCreationFunction(void(*func)(Object*)) {
	afterCreation_functions.insert(func);
}


void Object::addBeforeDestructionFunction(void(*func)(Object*)) {
	beforeDestruction_functions.insert(func);
}


std::string Object::getIdentifier() const {
	return identifier;
}


std::string Object::getParentIdentifier() const {
	return parent;
}


std::unordered_set<std::string>& Object::getChildrenIdentifiers() {
	return children;
}


Object* Object::getObject(std::string identifier) {
	if (identifier.empty()) return nullptr;
	return objectContainerPtr->at(identifier);
}


void Object::queueCreateObject(std::string filepath, std::string objectName, std::string objectParent, std::vector<std::pair<std::string, Attribute>> modifiedAttributes) {
	objectCreationQueuePtr->push_back({ filepath, objectName, objectParent, modifiedAttributes });
}


void Object::queueDestroyObject(std::string objectIdentifier) {
	objectDestructionQueuePtr->insert(objectIdentifier);
}


Attribute Object::getAttribute(std::string attr) {
	return attributes.at(attr);
}


void Object::setAttribute(std::string attr, Attribute val) {
	attributes.at(attr) = val;
}


bool Object::is_class(std::string identifier) const {
	return class_identifiers.count(identifier);
}


