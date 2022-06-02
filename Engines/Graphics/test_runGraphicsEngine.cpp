#define TEST_RUNGRAPHICSENGINE
#ifdef TEST_RUNGRAPHICSENGINE

#include "Graphics_Engine.h"

#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

void resizeCallback(GLFWwindow* window, int width, int height) {
	reinterpret_cast<Graphics_Engine*>(glfwGetWindowUserPointer(window))->framebufferResized = true;
}

struct test_image_pushConstants_block {
	glm::mat4 transformation;
	glm::vec2 texCoordinates[4];
};

int main() {

	try {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		Graphics_Engine engine;

		GLFWwindow* window = glfwCreateWindow(600, 600, "Test Graphics_Engine", NULL, NULL);
		glfwSetWindowUserPointer(window, &engine);
		glfwSetFramebufferSizeCallback(window, resizeCallback);

		engine.startupEngine(window);

		image_create_info test_image_info{};
		test_image_info.vertShaderPath = "./Engines/Graphics/testImage/testVert.spv";
		test_image_info.fragShaderPath = "./Engines/Graphics/testImage/testFrag.spv";
		test_image_info.graphicsPipeline = "test_pipeline";
		test_image_info.identifier = "test_image";
		test_image_info.pushConstantsSizeBytes = sizeof(test_image_pushConstants_block);
		test_image_info.initalPushConstantsValues.resize(test_image_info.pushConstantsSizeBytes);

		test_image_pushConstants_block initialPushValues{};
		glm::mat4 perspective = glm::perspective(45.0f, 1.0f, 0.1f, 10.0f);
		perspective[1][1] *= -1;
		initialPushValues.transformation = perspective * glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::vec2 texCoords[4] = { {0,0}, {1,0}, {0,1}, {1,1} };
		for (size_t i = 0; i < 4; ++i) initialPushValues.texCoordinates[i] = texCoords[i];
		for (size_t i = 0; i < test_image_info.pushConstantsSizeBytes; ++i) {
			test_image_info.initalPushConstantsValues[i] = reinterpret_cast<char*>(&initialPushValues)[i];
		}

		int texWidth, texHeight, texChannels;
		stbi_uc* pixels = stbi_load("./Engines/Graphics/testImage/test_image.jpg",
			&texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		if (!pixels) { throw std::runtime_error("failed to load image pixels"); }
		test_image_info.pixels.resize(texWidth * texHeight * 4);
		for (size_t i = 0; i < (size_t)texWidth * texHeight * 4; ++i) {
			test_image_info.pixels[i] = reinterpret_cast<char*>(pixels)[i];
		}
		test_image_info.texture_columns = texWidth;
		test_image_info.texture_rows = texHeight;
		stbi_image_free(pixels);

		engine.queueAddImage(test_image_info);

		image_create_info test_image_info2{};
		test_image_info2.vertShaderPath = "./Engines/Graphics/testImage/testVert.spv";
		test_image_info2.fragShaderPath = "./Engines/Graphics/testImage/testFrag.spv";
		test_image_info2.graphicsPipeline = "test_pipeline";
		test_image_info2.identifier = "test_image";
		test_image_info2.pushConstantsSizeBytes = sizeof(test_image_pushConstants_block);
		test_image_info2.initalPushConstantsValues.resize(test_image_info2.pushConstantsSizeBytes);

		test_image_pushConstants_block initialPushValues2{};
		glm::mat4 perspective2 = glm::perspective(45.0f, 1.0f, 0.1f, 10.0f);
		perspective2[1][1] *= -1;
		initialPushValues2.transformation = perspective2 * glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::translate(glm::mat4(1), glm::vec3(0.0f, 0.0f, -1.0f));
		glm::vec2 texCoords2[4] = { {0,0}, {1,0}, {0,1}, {1,1} };
		for (size_t i = 0; i < 4; ++i) initialPushValues2.texCoordinates[i] = texCoords2[i];
		for (size_t i = 0; i < test_image_info2.pushConstantsSizeBytes; ++i) {
			test_image_info2.initalPushConstantsValues[i] = reinterpret_cast<char*>(&initialPushValues2)[i];
		}

		int texWidth2, texHeight2, texChannels2;
		stbi_uc* pixels2 = stbi_load("./Engines/Graphics/testImage/test_image.jpg",
			&texWidth2, &texHeight2, &texChannels2, STBI_rgb_alpha);
		if (!pixels2) { throw std::runtime_error("failed to load image pixels"); }
		test_image_info2.pixels.resize(texWidth2 * texHeight2 * 4);
		for (size_t i = 0; i < (size_t)texWidth2 * texHeight2 * 4; ++i) {
			test_image_info2.pixels[i] = reinterpret_cast<char*>(pixels2)[i];
		}
		test_image_info2.texture_columns = texWidth2;
		test_image_info2.texture_rows = texHeight2;
		stbi_image_free(pixels2);

		engine.queueAddImage(test_image_info2);

		engine.createQueuedImages();

		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
			engine.drawFrame(window);
		}
		engine.shutdownEngine();
	}
	catch (std::runtime_error err) {
		std::cout << err.what() << std::endl;
	}



}


#endif