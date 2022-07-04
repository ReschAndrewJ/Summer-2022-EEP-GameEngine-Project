#include "Object_Sprite.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


Object_Sprite::Object_Sprite() {
	addClassIdentifier(OBJECT_CLASS_SPRITE);

	createAttribute(ATTRIBUTE_SPRITE_IMG_FILEPATH, Attribute::types::STRING);
	createAttribute(ATTRIBUTE_SPRITE_WIDTH, Attribute::types::INTEGER);
	createAttribute(ATTRIBUTE_SPRITE_HEIGHT, Attribute::types::INTEGER);
	createAttribute(ATTRIBUTE_SPRITE_VISIBLE, Attribute::types::BOOLEAN);
	createAttribute(ATTRIBUTE_SPRITE_IMG_IDENTIFIER, Attribute::types::STRING);
	
	setAttribute(ATTRIBUTE_SPRITE_VISIBLE, true);

	addAfterCreationFunction(&afterCreationFunction);
	addBeforeDestructionFunction(&beforeDestructionFunction);
	addProcessFunction(&processFunction, 20);

	addRequestedPointer(PTR_IDENTIFIER::IMG_CREATION_QUEUE_PTR, &imageCreationQueuePtr);
	addRequestedPointer(PTR_IDENTIFIER::IMG_DESTRUCTION_QUEUE_PTR, &imageDestructionQueuePtr);
	addRequestedPointer(PTR_IDENTIFIER::PUSH_UPDATE_QUEUE_PTR, &pushConstantsUpdateQueuePtr);

}
Object_Sprite::~Object_Sprite() {}



void Object_Sprite::processFunction(Object* self, float delta) {
	dynamic_cast<Object_Sprite*>(self)->updatePushConstants();
}


void Object_Sprite::afterCreationFunction(Object* self) {
	dynamic_cast<Object_Sprite*>(self)->loadImage();
}


void Object_Sprite::beforeDestructionFunction(Object* self) {
	dynamic_cast<Object_Sprite*>(self)->unloadImage();
}

struct imgPush_struct {
	glm::mat4 transformationMatrix;
	int isVisible;
};

void Object_Sprite::loadImage() {
	image_create_info imgInfo;
	imgInfo.vertShaderPath = "Resources/Shaders/spriteVert.spv";
	imgInfo.fragShaderPath = "Resources/Shaders/spriteFrag.spv";
	imgInfo.identifier = getIdentifier();
	imgInfo.graphicsPipeline = "Sprite_Pipeline";

	int texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load(((std::string)getAttribute(ATTRIBUTE_SPRITE_IMG_FILEPATH)).c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
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
	
	imgPush_struct initPushConstants{};
	initPushConstants.isVisible = 0;
	initPushConstants.transformationMatrix = glm::mat4(1.0f);

	imgInfo.pushConstantsSizeBytes = sizeof(imgPush_struct);
	imgInfo.initalPushConstantsValues.resize(imgInfo.pushConstantsSizeBytes);

	for (size_t i = 0; i < imgInfo.pushConstantsSizeBytes; ++i) {
		imgInfo.initalPushConstantsValues[i] = ((unsigned char*)&initPushConstants)[i];
	}

	std::pair<std::string, std::string> key = { getIdentifier(), ATTRIBUTE_SPRITE_IMG_IDENTIFIER };
	(*imageCreationQueuePtr).insert({ key, imgInfo });

}


void Object_Sprite::unloadImage() {
	(*imageDestructionQueuePtr).insert({ getIdentifier(), ATTRIBUTE_SPRITE_IMG_IDENTIFIER});
}


#include "glm/gtc/matrix_transform.hpp"
#include "../Camera/Object_Camera.h"

void Object_Sprite::updatePushConstants() {
	imgPush_struct pushValues{};
	pushValues.isVisible = (bool)getAttribute(ATTRIBUTE_SPRITE_VISIBLE) ? 1 : 0;
	glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), { (float)getAttribute(ATTRIBUTE_SPRITE_WIDTH)/2, (float)getAttribute(ATTRIBUTE_SPRITE_HEIGHT)/2, 0.0f });
	// vulkan uses range [-1,1] so width and height should be halved
	
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

	std::vector<unsigned char> pushValuesV(sizeof(imgPush_struct));
	for (size_t i = 0; i < sizeof(imgPush_struct); ++i) {
		pushValuesV[i] = ((unsigned char*)&pushValues)[i];
	}

	std::pair<std::string, std::string> key = {getIdentifier(), ATTRIBUTE_SPRITE_IMG_IDENTIFIER};
	(*pushConstantsUpdateQueuePtr).insert({ key, pushValuesV });

}