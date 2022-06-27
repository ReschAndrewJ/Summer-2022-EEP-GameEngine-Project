#ifndef FONTMANAGER
#define FONTMANAGER

#include "ft2build.h"
#include "freetype/freetype.h"

#include <vector>
#include <string>

#include <unordered_map>

class FontManager
{
private:
	static FT_Library freetypeLib;
	static std::unordered_map<std::string, FT_Face> fonts;

public:
	void closeFreetype();

public:
	/* returns the pixel image of the requested character from the given font
	<bitmap pixels, <bitmap pitch, bitmap heightAboveBaseline>>*/
	static std::pair<std::vector<unsigned char>, std::pair<int, int>> getCharacterImage(std::string fontFilepath, char32_t character);

	// unloads the given font if it is currently loaded
	static void unloadFont(std::string filepath);

private:
	static void loadFont(std::string filepath);
	

};



#endif