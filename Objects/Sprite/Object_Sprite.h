#ifndef OBJECT_SPRITE
#define OBJECT_SPRITE OBJECT_SPRITE

#include "../Spatial/Object_Spatial.h"
#include "../../Engines/Graphics/image_create_info.h"

#include <map>
#include <set>

const auto OBJECT_CLASS_SPRITE = "Object_Sprite";

class Object_Sprite : public Object_Spatial
{

	// shortcut constants for getting and setting Sprite attributes
public:
	const char* ATTRIBUTE_SPRITE_IMG_FILEPATH = "SpriteFilepath";
	const char* ATTRIBUTE_SPRITE_WIDTH = "SpriteWidth";
	const char* ATTRIBUTE_SPRITE_HEIGHT = "SpriteHeight";

	const char* ATTRIBUTE_SPRITE_VISIBLE = "SpriteIsVisible";
	// const char* ATTRIBUTE_SPRITE_OPACITY = "SpriteOpacity", 
	// non-opaque objects would require an additional rendering sub-pass, 
	// which is outside the scope of this project

	const char* ATTRIBUTE_SPRITE_IMG_IDENTIFIER = "SpriteImageIdentifier";

protected:
	// <{owner object identifier, image identifier attribute name}, imageCreateInfo>
	std::map<std::pair<std::string, std::string>, image_create_info>* imageCreationQueuePtr;
	// <owner object identifier, image identifier attribute name>
	std::set<std::pair<std::string, std::string>>* imageDestructionQueuePtr;
	// <{owner object identifier, image identifier attribute name}, pushConstantsValues>
	std::map<std::pair<std::string, std::string>, std::vector<char>>* pushConstantsUpdateQueuePtr;


public:
	Object_Sprite();
	virtual ~Object_Sprite();

protected:
	virtual void loadImage();
	virtual void updatePushConstants();
	virtual void unloadImage();

public:
	static void processFunction(Object*, float);
	static void afterCreationFunction(Object*);
	static void beforeDestructionFunction(Object*);


};

#endif //OBJECT_SPRITE