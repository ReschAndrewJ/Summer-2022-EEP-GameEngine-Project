#ifndef OBJECT_SPRITE
#define OBJECT_SPRITE OBJECT_SPRITE

#include "../Spatial/Object_Spatial.h"
#include "../../Engines/Graphics/image_create_info.h"

#include <map>
#include <set>

const auto OBJECT_CLASS_SPRITE = "Object_Sprite";

// filepath to the image file from the application directory
const auto ATTRIBUTE_SPRITE_IMG_FILEPATH = "SpriteFilepath";
// width of the sprite, (not the image resolution width)
const auto ATTRIBUTE_SPRITE_WIDTH = "SpriteWidth";
// height of the sprite, (not the image resolution height)
const auto ATTRIBUTE_SPRITE_HEIGHT = "SpriteHeight";

// set to false to not render the sprite
const auto ATTRIBUTE_SPRITE_VISIBLE = "SpriteIsVisible";

// not supported
const auto ATTRIBUTE_SPRITE_OPACITY = "SpriteOpacity";
// non-opaque objects would require an additional rendering sub-pass, 
// which is outside the scope of this project

const auto ATTRIBUTE_SPRITE_IMG_IDENTIFIER = "SpriteImageIdentifier";

class Object_Sprite : public Object_Spatial {

protected:
	// <{owner object identifier, image identifier attribute name}, imageCreateInfo>
	std::map<std::pair<std::string, std::string>, image_create_info>* imageCreationQueuePtr;
	// <owner object identifier, image identifier attribute name>
	std::set<std::pair<std::string, std::string>>* imageDestructionQueuePtr;
	// <{owner object identifier, image identifier attribute name}, pushConstantsValues>
	std::map<std::pair<std::string, std::string>, std::vector<unsigned char>>* pushConstantsUpdateQueuePtr;


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