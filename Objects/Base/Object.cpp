#include "Object.h"



Object::Object() {
	addClassIdentifier(OBJECT_BASE);
	addRequestedPointer(PTR_IDENTIFIER::OBJ_CONTAINER_PTR, objectContainerPtr);
	addRequestedPointer(PTR_IDENTIFIER::OBJ_CREATION_QUEUE_PTR, objectCreationQueue);
	addRequestedPointer(PTR_IDENTIFIER::OBJ_DESTRUCTION_QUEUE_PTR, objectDestructionQueue);
	identifier = "unnamed_object";
	parent = "";
}


void Object::createAttribute(std::string attributeName, Attribute::types type) {
	attributes.insert({ attributeName, Attribute(type) });
}


void Object::addClassIdentifier(std::string identifier) {
	class_identifiers.insert(identifier);
}


void Object::addRequestedPointer(PTR_IDENTIFIER ptrEnum, void* ptr) {
	requestedPointers.insert({ ptrEnum, ptr });
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


Object& Object::getObject(std::string identifier) {
	return *objectContainerPtr->operator[](identifier);
}


void Object::queueCreateObject(std::string filepath, std::string objectName, std::string objectParent) {
	objectCreationQueue->push_back({ filepath, objectName, objectParent });
}


void Object::queueDestroyObject(std::string objectIdentifier) {
	objectDestructionQueue->insert(objectIdentifier);
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


