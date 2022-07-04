#include "FontManager.h"
#include "freetype/ftbitmap.h"

FT_Library FontManager::freetypeLib = nullptr;
std::unordered_map<std::string, FT_Face> FontManager::fonts{};

void FontManager::closeFreetype() {
	for (auto& font : fonts) {
		FT_Done_Face(font.second);
	}
	fonts.clear();
	if (freetypeLib!=nullptr) FT_Done_FreeType(freetypeLib);
}


void FontManager::loadFont(std::string filepath) {
	if (freetypeLib == nullptr) {
		auto error = FT_Init_FreeType(&freetypeLib);
		if (error) {
			std::string err = "failed to initialize FreeType library\n";
			printf(err.c_str());
			return;
		}
	}

	FT_Face newFont;
	auto error = FT_New_Face(freetypeLib, filepath.c_str(), 0, &newFont);
	if (error == FT_Err_Unknown_File_Format) {
		std::string err = "unkown font file format: " + filepath + "\n";
		printf(err.c_str());
		return;
	}
	else if (error) {
		std::string err = "failed to open font file: " + filepath + "\n"
			+ "error: " + std::to_string(error) + "\n";
		printf(err.c_str());
		return;
	}

	error = FT_Set_Pixel_Sizes(newFont, 0, 64);
	if (error) error = FT_Set_Pixel_Sizes(newFont, 0, 32);
	if (error) error = FT_Set_Pixel_Sizes(newFont, 0, 16);
	if (error && newFont->num_fixed_sizes != 0) {
		error = FT_Set_Pixel_Sizes(newFont, newFont->available_sizes[0].width, newFont->available_sizes[0].height);
	}
	if (error) {
		std::string err = "failed to set font pixel size, font: " + filepath + "\n";
		printf(err.c_str());
		return;
	}

	fonts.insert({ filepath, newFont });
}


void FontManager::unloadFont(std::string filepath) {
	if (fonts.count(filepath)) {
		FT_Done_Face(fonts.at(filepath));
		fonts.erase(filepath);
	}
}


std::pair<std::vector<unsigned char>, std::pair<int,int>> FontManager::getCharacterImage(std::string fontFilepath, char32_t character) {
	if (!fonts.count(fontFilepath)) {
		loadFont(fontFilepath);
	}
	if (!fonts.count(fontFilepath)) {
		std::string err = "failed to get character image, characer: " + std::string(1, (char)character) + "\n" +
			"font filepath: " + fontFilepath + "\n";
		printf(err.c_str());
		return { std::vector<unsigned char>{0,0,0,0},{0,0} };
	}

	FT_Face font = fonts.at(fontFilepath);
	auto glyphIndex = FT_Get_Char_Index(font, character);

	// note: FT_LOAD_COLOR loads glyphs in BGRA format, not RGBA
	bool format_color = true;
	auto error = FT_Load_Glyph(font, glyphIndex, FT_LOAD_COLOR);
	if (error) error = FT_Load_Glyph(font, glyphIndex, FT_LOAD_DEFAULT); format_color = false;
	if (error) {
		std::string err = "failed to load character glyph, character: " + std::string(1, (char)character) + "\n" +
			"font filepath: " + fontFilepath + "\n";
	}

	// convert to bitmap
	if (font->glyph->format != FT_GLYPH_FORMAT_BITMAP) {
		FT_Render_Glyph(font->glyph, FT_RENDER_MODE_NORMAL);
	}

	int charTop = font->glyph->bitmap_top;
	/* DOES NOT SUPPORT CORRECT SPACING
	int pitch = format_color ? font->glyph->bitmap.pitch : font->glyph->bitmap.pitch * 4;
	std::vector<unsigned char> pixels(font->glyph->bitmap.rows * abs(pitch));
	
	if (format_color) for (size_t i = 0; i < pixels.size(); ++i) pixels[i] = font->glyph->bitmap.buffer[i];
	else for (size_t i = 0; i < (size_t)font->glyph->bitmap.rows * font->glyph->bitmap.pitch; ++i) {
		pixels[4 * i] = font->glyph->bitmap.buffer[i];
		pixels[4 * i + 1] = font->glyph->bitmap.buffer[i];
		pixels[4 * i + 2] = font->glyph->bitmap.buffer[i];
		if (font->glyph->bitmap.buffer[i]) pixels[4 * i + 3] = 255;
		//pixels[4 * i + 3] = 255;
	}

	return { pixels, {pitch, charTop} };
	*/
	
	int pitch = (font->glyph->bitmap.pitch >= 0 ? font->glyph->advance.x / 64 : -font->glyph->advance.x / 64);
	pitch = format_color ? pitch : pitch * 4;
	std::vector<unsigned char> pixels(font->glyph->bitmap.rows * abs(pitch));

	size_t i = 0, j = 0;
	if (format_color) while (i < pixels.size()) {
		if (i % abs(pitch) < font->glyph->bitmap.pitch) {
			pixels[i] = font->glyph->bitmap.buffer[j];
			++j;
		}
		++i;
	} 
	else while (i < pixels.size() / 4) {
		if (i % abs(pitch/4) < (size_t)font->glyph->bitmap.pitch) {
			pixels[4 * i] = font->glyph->bitmap.buffer[j];
			pixels[4 * i + 1] = font->glyph->bitmap.buffer[j];
			pixels[4 * i + 2] = font->glyph->bitmap.buffer[j];
			if (font->glyph->bitmap.buffer[j]) pixels[4 * i + 3] = 255;
			++j;
		}
		++i; 
	}

	return { pixels, {pitch, charTop} };
}


