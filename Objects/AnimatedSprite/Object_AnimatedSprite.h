#ifndef OBJECT_ANIMATEDSPRITE
#define OBJECT_ANIMATEDSPRITE

#include "../Sprite/Object_Sprite.h"

const auto OBJECT_CLASS_ANIMATEDSPRITE = "Object_AnimatedSprite";

// alias for sprite filepath
const auto ATTRIBUTE_ANIMATEDSPRITE_SPRITESHEET_FILEPATH = ATTRIBUTE_SPRITE_IMG_FILEPATH;

// the number of sprite cell rows in the sprite sheet
const auto ATTRIBUTE_ANIMATEDSPRITE_NUM_ROWS = "AnimatedSpriteRowCnt";
// the number of sprite cell columns in the sprite sheet
const auto ATTRIBUTE_ANIMATEDSPRITE_NUM_COLUMNS = "AnimatedSpriteColCnt";

// the index of the first sprite cell in the animation
const auto ATTRIBUTE_ANIMATEDSPRITE_FIRST_INDEX = "AnimatedSpriteFirstIndex";
// the index of the last sprite cell in the animation
const auto ATTRIBUTE_ANIMATEDSPRITE_LAST_INDEX = "AnimatedSpriteLastIndex";
// the index of the current frame sprite cell in the animation
const auto ATTRIBUTE_ANIMATEDSPRITE_CURRENT_INDEX = "AnimatedSpriteCurrIndex";

// the time between one animation frame and the next
const auto ATTRIBUTE_ANIMATEDSPRITE_TIME_BETWEEN_IMAGES = "AnimatedSpriteDelay";
// the remaining time until the next frame
const auto ATTRIBUTE_ANIMATEDSPRITE_TIME_TO_NEXT_IMAGE = "AnimatedSpriteNextImgTime";

// true if the animation is playing, false if paused
const auto ATTRIBUTE_ANIMATEDSPRITE_ISPLAYING = "AnimatedSpriteIsPlaying";

// true if animation is to loop, else animation stops on last frame
const auto ATTRIBUTE_ANIMATEDSPRITE_LOOP = "AnimatedSpriteDoesLoop";


class Object_AnimatedSprite : virtual public Object_Sprite
{
public:
	Object_AnimatedSprite();
	virtual ~Object_AnimatedSprite();


	void loadImage() override;
	void updatePushConstants() override;

	static void processFunction(Object*, float);

};


#endif