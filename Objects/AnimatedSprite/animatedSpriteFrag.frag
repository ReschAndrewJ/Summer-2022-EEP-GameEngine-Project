#version 460


layout (location = 0) in vec2 texCoord;
layout (location = 1) flat in int is_visible;

layout (binding = 0) uniform sampler2D texSampler;

layout (location = 0) out vec4 outColor;

void main() {
	outColor = texture(texSampler, texCoord);
	if (is_visible == 0 || outColor.a < 0.1) {
		discard;
	}
}
