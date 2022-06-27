#ifndef OBJECT_TEXT
#define OBJECT_TEXT

#include "../Sprite/Object_Sprite.h"

const auto OBJECT_CLASS_TEXT = "Object_Text";

// the filepath to the font from the application directory
const auto ATTRIBUTE_TEXT_FONT = ATTRIBUTE_SPRITE_IMG_FILEPATH;

// the string of text displayed by the object
const auto ATTRIBUTE_TEXT_STRING = "TextString";
// the number of characters to be displayed from the front of TextString, default=-1 for all
const auto ATTRIBUTE_TEXT_DISPLAY_COUNT = "TextCount";
// the number of characters in each row before line wrap, default=-1 for no wrap
const auto ATTRIBUTE_TEXT_ROW_LENGTH = "TextRowLen";
// the number of rows of text, default=1
const auto ATTRIBUTE_TEXT_ROW_COUNT = "TextRowCount";


class Object_Text : public Object_Sprite {

public:
	Object_Text();
	virtual ~Object_Text();

protected:
	void loadImage() override;
	void updatePushConstants() override;


	// helper variables, not permanent attributes
private:
	std::vector<float> xBounds; // per character
	std::vector<std::pair<float, float>> yBounds; // per row, <start,height>


};


#endif