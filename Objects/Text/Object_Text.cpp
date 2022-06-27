#include "Object_Text.h"

#include "../../FontManager/FontManager.h"

#include <deque>

Object_Text::Object_Text() {
	addClassIdentifier(OBJECT_CLASS_TEXT);

	createAttribute(ATTRIBUTE_TEXT_STRING, Attribute::types::STRING);
	createAttribute(ATTRIBUTE_TEXT_DISPLAY_COUNT, Attribute::types::INTEGER);
	setAttribute(ATTRIBUTE_TEXT_DISPLAY_COUNT, -1);

	createAttribute(ATTRIBUTE_TEXT_ROW_LENGTH, Attribute::types::INTEGER);
	createAttribute(ATTRIBUTE_TEXT_ROW_COUNT, Attribute::types::INTEGER);
	setAttribute(ATTRIBUTE_TEXT_ROW_LENGTH, -1);
	setAttribute(ATTRIBUTE_TEXT_ROW_COUNT, 1);
}

Object_Text::~Object_Text() {}


struct txtPush_struct {
	glm::mat4 transformationMatrix;  // *-------*
	float boundX;					 // |   1---* <- yInner
	float boundYInner;				 // *---2	  <- yOuter
	float boundYOuter;				 //     ^ X
	int isVisible;
};


void Object_Text::loadImage() {

	std::string text = getAttribute(ATTRIBUTE_TEXT_STRING);
	std::string font = getAttribute(ATTRIBUTE_TEXT_FONT);
	long long charRows = getAttribute(ATTRIBUTE_TEXT_ROW_COUNT);
	long long charCols = getAttribute(ATTRIBUTE_TEXT_ROW_LENGTH);
	if (charCols == -1) {
		charCols = text.size();
		charRows = 1;
	}
	
	// construct image from string and font
	std::vector<std::vector<std::deque<unsigned char>>> characterImgRows;
	for (size_t row = 0; row < charRows; ++row) {
		std::vector<std::deque<unsigned char>> pixelColumnsInTextRow;

		int baseline = 0;

		for (size_t column = 0; column < charCols; ++column) {
			auto charImg = FontManager::getCharacterImage(font, text[row * charCols + column]);
			
			int charWidth = abs(charImg.second.first);
			int distAboveBaseline = charImg.second.second;
			int distBelowBaseline = charImg.first.size() / charWidth - distAboveBaseline;

			// resize pixelColumns vector
			size_t prevWidth = pixelColumnsInTextRow.size();
			size_t prevHeight = pixelColumnsInTextRow.size() != 0 ? pixelColumnsInTextRow[0].size() : 0;
			size_t newWidth = prevWidth + charWidth;
			pixelColumnsInTextRow.resize(newWidth); // columns
			for (size_t i = prevWidth; i < newWidth; ++i) {
				pixelColumnsInTextRow[i].resize(prevHeight);
			}
			if (distAboveBaseline > baseline) { // add space above
				int diff = distAboveBaseline - baseline;
				for (size_t i = 0; i < newWidth; ++i) {
					for (size_t j = 0; j < diff; ++j) {
						pixelColumnsInTextRow[i].push_front(0);
					}
				}
				baseline = distAboveBaseline;
			}
			if (distBelowBaseline > pixelColumnsInTextRow[0].size() - baseline) { // add space below
				int diff = distBelowBaseline - (pixelColumnsInTextRow[0].size() - baseline);
				for (size_t i = 0; i < newWidth; ++i) {
					for (size_t j = 0; j < diff; ++j) {
						pixelColumnsInTextRow[i].push_back(0);
					}
				}
			}
			int newHeight = pixelColumnsInTextRow[0].size();

			// copy pixel data to pixelColumns vector
			int verticalStartOffset = baseline - distAboveBaseline;
			for (size_t col = 0; col < charWidth; ++col) {
				for (size_t row = 0; row < (size_t)distAboveBaseline + distBelowBaseline; ++row) {
					pixelColumnsInTextRow[col + prevWidth][row + verticalStartOffset] = charImg.first[row * charWidth + col];
				}
			}
			xBounds.push_back(newWidth);
		}
		size_t newLast = yBounds.size();
		yBounds.push_back({ 0, pixelColumnsInTextRow.size() != 0 ? pixelColumnsInTextRow[0].size() : 0 });
		if (newLast > 0) {
			yBounds[newLast].first = yBounds[newLast - 1].first + yBounds[newLast - 1].second;
		}

		// copy pixel data from pixelColumns to characterImgRows
		characterImgRows.push_back(std::move(pixelColumnsInTextRow));
	}

	// combine text rows pixel data
	size_t maxWidth = characterImgRows[0].size();
	for (auto& vec : characterImgRows) {
		maxWidth = vec.size() > maxWidth ? vec.size() : maxWidth;
	}
	for (auto& vec : characterImgRows) { // add end-of-line padding
		size_t lastColInVec = vec.size() - 1;
		vec.resize(maxWidth);
		for (size_t pad = lastColInVec + 1; pad < maxWidth; ++pad) {
			vec[pad].resize(vec[0].size());
		}
	}

	std::vector<unsigned char> pixels;
	for (auto& line : characterImgRows) { // copy pixel data
		for (size_t row = 0; row < line[0].size(); ++row) {
			for (auto& pixColumn : line) {
				pixels.push_back(pixColumn[row]);
			}
		}
	}

	for (float& bound : xBounds) {
		bound = bound / maxWidth;
	}
	for (auto& bound : yBounds) {
		bound.first = bound.first / (pixels.size() / maxWidth);
		bound.second = bound.second / (pixels.size() / maxWidth);
	}
	
	/* DOES NOT APPLY VERTICAL PADDING
	std::vector<std::vector<unsigned char>> RowsTopToBottom(1);
	RowsTopToBottom[0].resize(1);

	// left->right, top->bottom
	size_t yOffset = 0;
	for (size_t row = 0; row < charRows; ++row) {
		
		size_t xIndent = 0;
		for (size_t character = 0; character < charCols; ++character) {
			// get character image
			auto charImg = FontManager::getCharacterImage(font, text[row * charCols + character]);
			int charWidth = abs(charImg.second.first);
			int charHeight = charImg.first.size() / charWidth;
			
			// resize 2D vector to fit
			if (yOffset + charHeight > RowsTopToBottom.size()) { // pixel rows
				RowsTopToBottom.resize(yOffset + charHeight);
				for (size_t i = yOffset; i < yOffset + charHeight; ++i) {
					RowsTopToBottom[i].resize(RowsTopToBottom[0].size());
				}
			}
			if (xIndent + charWidth > RowsTopToBottom[0].size()) { // pixel columns
				for (size_t i = 0; i < RowsTopToBottom.size(); ++i) RowsTopToBottom[i].resize(xIndent + charWidth);
			}
			
			// copy pixel data to 2D vector
			for (size_t yPos = 0; yPos < charHeight; ++yPos) {

				for (size_t xPos = 0; xPos < charWidth; ++xPos) {
					if (charImg.second.first > 0) { // positive pitch, top-to-bottom
						RowsTopToBottom[yOffset + yPos][xIndent + xPos] = charImg.first[yPos * charWidth + xPos];
					}
					else { // negative pitch, bottom-to-top
						RowsTopToBottom[yOffset + yPos][xIndent + xPos] = charImg.first[(charHeight - yPos - 1) * charWidth + xPos];
					}
				}
			}
			xIndent += charWidth;
		}
		yOffset = RowsTopToBottom.size();
	}
	
	// copy 2D vector to 1D vector
	size_t pHeight = RowsTopToBottom.size();
	size_t pWidth = RowsTopToBottom[0].size();
	std::vector<unsigned char> pixels(pWidth * pHeight);
	for (size_t pRow = 0; pRow < pHeight; ++pRow) {
		for (size_t pCol = 0; pCol < pWidth; ++pCol) {
			pixels[pRow * pWidth + pCol] = RowsTopToBottom[pRow][pCol];
		}
	}*/

	image_create_info imgInfo;

	imgInfo.vertShaderPath = "Objects/Text/textVert.spv";
	imgInfo.fragShaderPath = "Objects/Text/textFrag.spv";
	imgInfo.identifier = getIdentifier();
	imgInfo.graphicsPipeline = "Text_Pipeline";

	imgInfo.texture_columns = maxWidth / 4;
	imgInfo.texture_rows = pixels.size() / maxWidth;

	if (pixels.size() == 0) {
		std::string err = "failed to load text to pixels, text: " + (std::string)getAttribute(ATTRIBUTE_TEXT_STRING) + "\n" +
			"font filepath: " + (std::string)getAttribute(ATTRIBUTE_TEXT_FONT);
		printf(err.c_str());
		return;
	}
	imgInfo.pixels.resize(pixels.size());
	for (size_t i = 0; i < pixels.size(); ++i) {
		imgInfo.pixels[i] = pixels[i];
	}

	txtPush_struct initPushConstants{};
	initPushConstants.isVisible = 0;
	initPushConstants.transformationMatrix = glm::mat4(1.0f);

	long long count = getAttribute(ATTRIBUTE_TEXT_DISPLAY_COUNT);
	if (count > text.size()) {
		count = text.size();
		setAttribute(ATTRIBUTE_TEXT_DISPLAY_COUNT, count);
	}
	if (count == -1) {
		initPushConstants.boundX = 1;
		initPushConstants.boundYInner = 1;
		initPushConstants.boundYOuter = 1;
	}
	else if (count == 0) {
		initPushConstants.boundX = 0;
		initPushConstants.boundYInner = 0;
		initPushConstants.boundYOuter = 0;
	}
	else {
		int boundCol = (count - 1);
		initPushConstants.boundX = xBounds[boundCol];
		int boundRow = (count-1) / charCols;
		initPushConstants.boundYInner = yBounds[boundRow].first;
		initPushConstants.boundYOuter = initPushConstants.boundYInner + yBounds[boundRow].second;
	}

	imgInfo.pushConstantsSizeBytes = sizeof(txtPush_struct);
	imgInfo.initalPushConstantsValues.resize(imgInfo.pushConstantsSizeBytes);
	
	std::pair<std::string, std::string> key = { getIdentifier(), ATTRIBUTE_SPRITE_IMG_IDENTIFIER };
	(*imageCreationQueuePtr).insert({ key, imgInfo });
}


#include "glm/gtc/matrix_transform.hpp"
#include "../Camera/Object_Camera.h"

void Object_Text::updatePushConstants() {
	txtPush_struct pushValues{};
	pushValues.isVisible = (bool)getAttribute(ATTRIBUTE_SPRITE_VISIBLE) ? 1 : 0;
	glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), { getAttribute(ATTRIBUTE_SPRITE_WIDTH), getAttribute(ATTRIBUTE_SPRITE_HEIGHT), 0.0f });

	glm::mat4 cameraMat = glm::mat4(1.0);
	{
		auto parent = getParentIdentifier();
		while (parent != "ROOT") {
			auto obj = getObject(parent);
			if (obj->is_class(OBJECT_CLASS_CAMERA)) {
				cameraMat = ((Object_Camera*)obj)->getCameraTransformationMatrix();
				break;
			}
			parent = obj->getParentIdentifier();
		}
	}
	pushValues.transformationMatrix = cameraMat * getTransformationMatrix(scaleMat);
	
	long long count = getAttribute(ATTRIBUTE_TEXT_DISPLAY_COUNT);
	if (count > ((std::string)getAttribute(ATTRIBUTE_TEXT_STRING)).size()) {
		count = ((std::string)getAttribute(ATTRIBUTE_TEXT_STRING)).size();
		setAttribute(ATTRIBUTE_TEXT_DISPLAY_COUNT, count);
	}
	if (count == -1) {
		pushValues.boundX = 1;
		pushValues.boundYInner = 1;
		pushValues.boundYOuter = 1;
	}
	else if (count == 0) {
		pushValues.boundX = 0;
		pushValues.boundYInner = 0;
		pushValues.boundYOuter = 0;
	}
	else {
		int cols = getAttribute(ATTRIBUTE_TEXT_ROW_LENGTH);
		int rows = getAttribute(ATTRIBUTE_TEXT_ROW_COUNT);
		if (cols == -1) {
			cols = ((std::string)getAttribute(ATTRIBUTE_TEXT_STRING)).size();
			rows = 1;
		}
		int boundCol = (count - 1);
		pushValues.boundX = xBounds[boundCol];
		int boundRow = (count-1) / cols;
		pushValues.boundYInner = yBounds[boundRow].first;
		pushValues.boundYOuter = pushValues.boundYInner + yBounds[boundRow].second;
	}

	std::vector<unsigned char> pushValuesV(sizeof(txtPush_struct));
	for (size_t i = 0; i < sizeof(txtPush_struct); ++i) {
		pushValuesV[i] = ((unsigned char*)&pushValues)[i];
	}

	std::pair<std::string, std::string> key = { getIdentifier(), ATTRIBUTE_SPRITE_IMG_IDENTIFIER };
	(*pushConstantsUpdateQueuePtr).insert({ key, pushValuesV });

}