#include "Object_Loader.h"

#include <fstream>


#ifndef UNIT_TESTS
// include derived object classes
#include "../Objects/Spatial/Object_Spatial.h"
#include "../Objects/Sprite/Object_Sprite.h"
#include "../Objects/Camera/Object_Camera.h"
#include "../Objects/Collision/Object_Collision.h"


#else
#include "../Objects/Test/Object_Test1.h"
#endif
//


std::pair<Object*, std::vector<std::pair<std::string, std::string>>> Object_Loader::createInstance(std::string filepath, std::string identifier) {
	if (!loadedFiles.count(filepath)) {
		loadDataFromInstFile(filepath);
	}
	if (!loadedObjectInfos.count({ filepath + '/' + identifier})) {
		std::string err = "No instance data: " + filepath + '/' + identifier;
		printf(err.c_str());
		throw std::runtime_error(err);
	}
	
	std::pair<Object*, std::vector<std::pair<std::string, std::string>>> returnData;

	Object_Info& ObjectInfo = loadedObjectInfos.at(filepath + '/' + identifier);
	Object* newObject = nullptr;

	if (ObjectInfo.class_identifier == OBJECT_CLASS_BASE) newObject = new Object();
#ifndef UNIT_TESTS
	// else if derived classes
	else if (ObjectInfo.class_identifier == OBJECT_CLASS_SPATIAL) newObject = new Object_Spatial();
	else if (ObjectInfo.class_identifier == OBJECT_CLASS_SPRITE) newObject = new Object_Sprite();
	else if (ObjectInfo.class_identifier == OBJECT_CLASS_CAMERA) newObject = new Object_Camera();
	else if (ObjectInfo.class_identifier == OBJECT_CLASS_COLLISION) newObject = new Object_Collision();


#else
	else  if (ObjectInfo.class_identifier == OBJECT_CLASS_TEST1) newObject = new Object_Test1();
#endif
	//
	if (newObject == nullptr) {
		std::string err = "attempted to load object with unmanaged object class: " + ObjectInfo.class_identifier;
		printf(err.c_str());
		throw std::runtime_error(err);
	}

	for (const auto& attr : ObjectInfo.attributes) {
		newObject->setAttribute(attr.first, attr.second);
	}
	returnData.first = newObject;
	returnData.second = std::move(ObjectInfo.children);

	return returnData;
}


void Object_Loader::loadDataFromInstFile(std::string filepath) {
	if (loadedFiles.count(filepath)) return; // file already loaded, method is private/line should never be called

	std::fstream fileStream;
	fileStream.open(filepath);
	if (!fileStream.is_open()) {
		std::string err = "failed to open file: " + filepath;
		printf(err.c_str());
		throw std::runtime_error(err);
	}

	Object_Info currentInfo;
	bool firstInfo = true;
	size_t lineNumber = 0;
	while (!fileStream.eof()) {
		++lineNumber;
		std::string line;
		char currentChar;
		while ((currentChar = fileStream.get()) != '\n' && !fileStream.eof()) { // get current line
			line.push_back(currentChar);
		}

		// skip empty linespace & commented lines
		bool skipline = true;
		size_t startPoint = 0;
		for (size_t i = 0; i < line.size(); ++i) {
			if (line[i] == '#') {
				break;
			}
			if (line[i] != ' ') {
				startPoint = i;
				skipline = false;
				break;
			}
		}
		if (skipline) continue;

		// Object keyword
		if (line.size() - startPoint >= 7 && line.compare(startPoint, 7, "Object ", 0, 7) == 0) {
			// create new currentInfo
			if (!firstInfo) {
				loadedObjectInfos.insert({ filepath + '/' + currentInfo.name_id, currentInfo });
				currentInfo = Object_Info(); 
			}
			else firstInfo = false;

			for (size_t i = startPoint+7; i < line.size(); ++i) {
				if (line[i] != ' ') {
					startPoint = i; 
					break;
				}
			}
			// get currentInfo name
			std::string name_id;
			for (size_t i = startPoint; i < line.size(); ++i) {
				if (line[i] != ' ') name_id.push_back(line[i]);
				else {
					startPoint = i;
					break;
				}
			}
			currentInfo.name_id = name_id;

			for (size_t i = startPoint + 1; i < line.size(); ++i) {
				if (line[i] != ' ') {
					startPoint = i;
					break;
				}
			}

			// set currentInfo class
			std::string class_id;
			for (size_t i = startPoint; i < line.size(); ++i) {
				if (line[i] == ' ') break;
				class_id.push_back(line[i]);
			}
			currentInfo.class_identifier = class_id;
			continue;
		} // Object

		// Child keyword
		if (line.size() - startPoint >= 6 && line.compare(startPoint, 6, "Child ", 0, 6) == 0) {

			for (size_t i = startPoint + 6; i < line.size(); ++i) {
				if (line[i] != ' ') {
					startPoint = i;
					break;
				}
			}

			// get child filepath
			std::string child_filepath;

			char stopChar = ' ';
			if (line[startPoint] == '"') { stopChar = '"'; ++startPoint; }
			for (size_t i = startPoint; i < line.size(); ++i) {
				if (line[i] != stopChar) child_filepath.push_back(line[i]);
				else {
					startPoint = i;
					break;
				}
			}
			if (child_filepath == "Here") child_filepath = filepath;

			
			for (size_t i = startPoint + 1; i < line.size(); ++i) {
				if (line[i] != ' ') {
					startPoint = i;
					break;
				}
			}

			// get child name_id
			std::string child_nameid;
			for (size_t i = startPoint; i < line.size(); ++i) {
				if (line[i] == ' ') break;
				child_nameid.push_back(line[i]);
			}
			currentInfo.children.push_back({ child_filepath, child_nameid });
			continue;
		} // Child

		// Attributes

		// Integer
		if (line.size() - startPoint >= 8 && line.compare(startPoint, 8, "Integer ", 0, 8) == 0) {
			
			for (size_t i = startPoint + 8; i < line.size(); ++i) {
				if (line[i] != ' ') {
					startPoint = i;
					break;
				}
			}

			// attribute name
			std::string attr_name;
			for (size_t i = startPoint; i < line.size(); ++i) {
				if (line[i] != ' ') attr_name.push_back(line[i]);
				else {
					startPoint = i;
					break;
				}
			}

			for (size_t i = startPoint + 1; i < line.size(); ++i) {
				if (line[i] != ' ') {
					startPoint = i;
					break;
				}
			}

			// attribute value
			std::string val_as_string;
			for (size_t i = startPoint; i < line.size(); ++i) {
				if (line[i] == ' ') break;
				val_as_string.push_back(line[i]);
			}
			Attribute attr_val = Attribute(std::stoll(val_as_string), Attribute::types::INTEGER);
			
			currentInfo.attributes.push_back({ attr_name, attr_val });
			continue;
		} // Integer

		// Double
		if (line.size() - startPoint >= 7 && line.compare(startPoint, 7, "Double ", 0, 7) == 0) {
			
			for (size_t i = startPoint + 7; i < line.size(); ++i) {
				if (line[i] != ' ') {
					startPoint = i;
					break;
				}
			}

			// attribute name
			std::string attr_name;
			for (size_t i = startPoint; i < line.size(); ++i) {
				if (line[i] != ' ') attr_name.push_back(line[i]);
				else {
					startPoint = i;
					break;
				}
			}

			for (size_t i = startPoint + 1; i < line.size(); ++i) {
				if (line[i] != ' ') {
					startPoint = i;
					break;
				}
			}

			// attribute value
			std::string val_as_string;
			for (size_t i = startPoint; i < line.size(); ++i) {
				if (line[i] == ' ') break;
				val_as_string.push_back(line[i]);
			}
			Attribute attr_val = Attribute(std::stod(val_as_string), Attribute::types::DOUBLE);
			
			currentInfo.attributes.push_back({ attr_name, attr_val });
			continue;
		} // Double

		// Boolean
		if (line.size() - startPoint >= 8 && line.compare(startPoint, 8, "Boolean ", 0, 8) == 0) {

			for (size_t i = startPoint + 8; i < line.size(); ++i) {
				if (line[i] != ' ') {
					startPoint = i;
					break;
				}
			}

			// attribute name
			std::string attr_name;
			for (size_t i = startPoint; i < line.size(); ++i) {
				if (line[i] != ' ') attr_name.push_back(line[i]);
				else {
					startPoint = i;
					break;
				}
			}

			for (size_t i = startPoint + 1; i < line.size(); ++i) {
				if (line[i] != ' ') {
					startPoint = i;
					break;
				}
			}

			// attribute value
			std::string val_as_string;
			for (size_t i = startPoint; i < line.size(); ++i) {
				if (line[i] == ' ') break;
				val_as_string.push_back(line[i]);
			}

			Attribute attr_val(Attribute::types::BOOLEAN);
			if (val_as_string == "true") attr_val = true;
			else if (val_as_string == "false") attr_val = false;
			else {
			std::string err = "invalid value in inst file: " + filepath + " for boolean attribute: " + attr_name;
			printf(err.c_str());
				throw std::runtime_error(err);
			}

			currentInfo.attributes.push_back({ attr_name, attr_val });
			continue;
		} // Boolean

		// Character
		if (line.size() - startPoint >= 10 && line.compare(startPoint, 10, "Character ", 0, 10) == 0) {

			for (size_t i = startPoint + 10; i < line.size(); ++i) {
				if (line[i] != ' ') {
					startPoint = i;
					break;
				}
			}

			// attribute name
			std::string attr_name;
			for (size_t i = startPoint; i < line.size(); ++i) {
				if (line[i] != ' ') attr_name.push_back(line[i]);
				else {
					startPoint = i;
					break;
				}
			}

			for (size_t i = startPoint + 1; i < line.size(); ++i) {
				if (line[i] != ' ') {
					startPoint = i;
					break;
				}
			}

			// attribute value
			char cVal = line[startPoint];

			Attribute attr_val = Attribute(cVal, Attribute::types::CHARACTER);
			currentInfo.attributes.push_back({ attr_name, attr_val });
			continue;
		} // Character

		// String
		if (line.size() - startPoint >= 7 && line.compare(startPoint, 7, "String ", 0, 7) == 0) {

			for (size_t i = startPoint + 7; i < line.size(); ++i) {
				if (line[i] != ' ') {
					startPoint = i;
					break;
				}
			}

			// attribute name
			std::string attr_name;
			for (size_t i = startPoint; i < line.size(); ++i) {
				if (line[i] != ' ') attr_name.push_back(line[i]);
				else {
					startPoint = i;
					break;
				}
			}

			for (size_t i = startPoint + 1; i < line.size(); ++i) {
				if (line[i] == '"') {
					startPoint = i+1;
					break;
				}
			}

			// attribute value
			std::string sVal;
			for (size_t i = startPoint; i < line.size(); ++i) {
				if (line[i] == '"') break;
				sVal.push_back(line[i]);
			}

			Attribute attr_val = Attribute(sVal, Attribute::types::STRING);
			currentInfo.attributes.push_back({ attr_name, attr_val });
			continue;
		} // String

		std::string err = "invalid line number " + std::to_string(lineNumber) + ": " + line;
		printf(err.c_str());
		throw std::runtime_error(err);
	}
	loadedObjectInfos.insert({ filepath + '/' + currentInfo.name_id, currentInfo });

	fileStream.close();
	loadedFiles.insert(filepath);
}