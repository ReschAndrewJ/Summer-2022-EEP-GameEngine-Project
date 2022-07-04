#include "Object_AnimatedSprite.h"

#include "stb_image.h"

Object_AnimatedSprite::Object_AnimatedSprite() {
	addClassIdentifier(OBJECT_CLASS_ANIMATEDSPRITE);

	createAttribute(ATTRIBUTE_ANIMATEDSPRITE_NUM_ROWS, Attribute::types::INTEGER);
	createAttribute(ATTRIBUTE_ANIMATEDSPRITE_NUM_COLUMNS, Attribute::types::INTEGER);
	setAttribute(ATTRIBUTE_ANIMATEDSPRITE_NUM_ROWS, 1);
	setAttribute(ATTRIBUTE_ANIMATEDSPRITE_NUM_COLUMNS, 1);

	createAttribute(ATTRIBUTE_ANIMATEDSPRITE_FIRST_INDEX, Attribute::types::INTEGER);
	createAttribute(ATTRIBUTE_ANIMATEDSPRITE_LAST_INDEX, Attribute::types::INTEGER);
	createAttribute(ATTRIBUTE_ANIMATEDSPRITE_CURRENT_INDEX, Attribute::types::INTEGER);

	createAttribute(ATTRIBUTE_ANIMATEDSPRITE_TIME_BETWEEN_IMAGES, Attribute::types::DOUBLE);
	createAttribute(ATTRIBUTE_ANIMATEDSPRITE_TIME_TO_NEXT_IMAGE, Attribute::types::DOUBLE);
	
	createAttribute(ATTRIBUTE_ANIMATEDSPRITE_ISPLAYING, Attribute::types::BOOLEAN);
	createAttribute(ATTRIBUTE_ANIMATEDSPRITE_LOOP, Attribute::types::BOOLEAN);

	addProcessFunction(&processFunction, 19);
}
Object_AnimatedSprite::~Object_AnimatedSprite() {}


struct animatedSpritePush_struct {
	glm::mat4 transformationMatrix;
	glm::vec2 cellCoords[4];
	int isVisible;
};


void Object_AnimatedSprite::loadImage() {
	image_create_info imgInfo;
	imgInfo.vertShaderPath = "Resources/Shaders/animatedSpriteVert.spv";
	imgInfo.fragShaderPath = "Resources/Shaders/animatedSpriteFrag.spv";
	imgInfo.identifier = getIdentifier();
	imgInfo.graphicsPipeline = "AnimatedSprite_Pipeline";

	int texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load(((std::string)getAttribute(ATTRIBUTE_ANIMATEDSPRITE_SPRITESHEET_FILEPATH)).c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	if (!pixels) {
		std::string err = "failed to load image pixels from file: " + (std::string)getAttribute(ATTRIBUTE_SPRITE_IMG_FILEPATH);
		printf(err.c_str());
		return;
	}
	imgInfo.pixels.resize((size_t)texWidth * texHeight * 4);
	for (size_t i = 0; i < (size_t)texWidth * texHeight * 4; ++i) {
		imgInfo.pixels[i] = ((unsigned char*)pixels)[i];
	}
	imgInfo.texture_columns = texWidth;
	imgInfo.texture_rows = texHeight;
	stbi_image_free(pixels);

	animatedSpritePush_struct initPushConstants{};
	initPushConstants.isVisible = 0;
	initPushConstants.transformationMatrix = glm::mat4(1.0f);
	for (size_t i = 0; i < 4; ++i) {
		initPushConstants.cellCoords[i] = { 0,0 };
	}

	imgInfo.pushConstantsSizeBytes = sizeof(animatedSpritePush_struct);
	imgInfo.initalPushConstantsValues.resize(imgInfo.pushConstantsSizeBytes);

	for (size_t i = 0; i < imgInfo.pushConstantsSizeBytes; ++i) {
		imgInfo.initalPushConstantsValues[i] = ((unsigned char*)&initPushConstants)[i];
	}

	std::pair<std::string, std::string> key = { getIdentifier(), ATTRIBUTE_SPRITE_IMG_IDENTIFIER };
	(*imageCreationQueuePtr).insert({ key, imgInfo });
}

#include "glm/gtc/matrix_transform.hpp"
#include "../Camera/Object_Camera.h"

void Object_AnimatedSprite::updatePushConstants() {
	animatedSpritePush_struct pushValues{};
	pushValues.isVisible = (bool)getAttribute(ATTRIBUTE_SPRITE_VISIBLE);

	glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), { (float)getAttribute(ATTRIBUTE_SPRITE_WIDTH)/2, (float)getAttribute(ATTRIBUTE_SPRITE_HEIGHT)/2, 0.0f });
	glm::mat4 cameraMat = glm::mat4(1.0);
	{
		auto parent = getParentIdentifier();
		while (parent != "ROOT") {
			auto obj = getObject(parent);
			if (obj->is_class(OBJECT_CLASS_CAMERA)) {
				cameraMat = dynamic_cast<Object_Camera*>(obj)->getCameraTransformationMatrix();
				break;
			}
			parent = obj->getParentIdentifier();
		}
	}
	pushValues.transformationMatrix = cameraMat * getTransformationMatrix(scaleMat);

	long long cellIndex = getAttribute(ATTRIBUTE_ANIMATEDSPRITE_CURRENT_INDEX);
	long long columnCount = getAttribute(ATTRIBUTE_ANIMATEDSPRITE_NUM_COLUMNS);
	long long column = cellIndex % columnCount;

	float left = (float)column / columnCount;
	float right = left + 1.0f / columnCount;

	long long rowCount = getAttribute(ATTRIBUTE_ANIMATEDSPRITE_NUM_ROWS);
	long long row = cellIndex / columnCount;

	float top = (float)row / rowCount;
	float bottom = top + 1.0f / rowCount;
	
	pushValues.cellCoords[0] = { left, top };
	pushValues.cellCoords[1] = { right, top };
	pushValues.cellCoords[2] = { left, bottom };
	pushValues.cellCoords[3] = { right, bottom };

	std::vector<unsigned char> pushValuesV(sizeof(animatedSpritePush_struct));
	for (size_t i = 0; i < sizeof(animatedSpritePush_struct); ++i) {
		pushValuesV[i] = ((unsigned char*)&pushValues)[i];
	}

	std::pair<std::string, std::string> key = { getIdentifier(), ATTRIBUTE_SPRITE_IMG_IDENTIFIER };
	(*pushConstantsUpdateQueuePtr).insert({ key, pushValuesV });
}


void Object_AnimatedSprite::processFunction(Object* self, float delta) {
	Object_AnimatedSprite* selfPtr = dynamic_cast<Object_AnimatedSprite*>(self);
	if (!selfPtr->getAttribute(ATTRIBUTE_ANIMATEDSPRITE_ISPLAYING)) return;

	double timeToNext = (double)selfPtr->getAttribute(ATTRIBUTE_ANIMATEDSPRITE_TIME_TO_NEXT_IMAGE) - delta;
	if (timeToNext <= 0) {
		long long nextIndex = (long long)selfPtr->getAttribute(ATTRIBUTE_ANIMATEDSPRITE_CURRENT_INDEX) + 1;
		if (nextIndex > (long long)selfPtr->getAttribute(ATTRIBUTE_ANIMATEDSPRITE_LAST_INDEX)) {
			if ((bool)selfPtr->getAttribute(ATTRIBUTE_ANIMATEDSPRITE_LOOP)) {
				nextIndex = (long long)selfPtr->getAttribute(ATTRIBUTE_ANIMATEDSPRITE_FIRST_INDEX);
			}
			else {
				--nextIndex;
				selfPtr->setAttribute(ATTRIBUTE_ANIMATEDSPRITE_ISPLAYING, false);
			}
		}
		selfPtr->setAttribute(ATTRIBUTE_ANIMATEDSPRITE_CURRENT_INDEX, nextIndex);
		timeToNext = (double)selfPtr->getAttribute(ATTRIBUTE_ANIMATEDSPRITE_TIME_BETWEEN_IMAGES);
	}
	selfPtr->setAttribute(ATTRIBUTE_ANIMATEDSPRITE_TIME_TO_NEXT_IMAGE, timeToNext);
}