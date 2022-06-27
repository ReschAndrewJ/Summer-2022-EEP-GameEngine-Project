#ifndef IMAGE_CREATE_INFO

#include <string>
#include <stdint.h>
#include <vector>


struct image_create_info {
	std::string identifier;
	std::string graphicsPipeline;

	std::string vertShaderPath;
	std::string fragShaderPath;

	uint32_t pushConstantsSizeBytes = 0;
	std::vector<unsigned char> initalPushConstantsValues;

	unsigned int texture_rows = 0;
	unsigned int texture_columns = 0;
	std::vector<unsigned char> pixels;
};

#endif //IMAGE_CREATE_INFO