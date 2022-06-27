#version 460


layout (location = 0) in vec2 texCoord;
layout (location = 1) flat in int is_visible;

layout (location = 2) flat in int boundX;
layout (location = 3) flat in int boundYInner;
layout (location = 4) flat in int boundYOuter;

layout (binding = 0) uniform sampler2D texSampler;

layout (location = 0) out vec4 outColor;

void main() {
	outColor = texture(texSampler, texCoord);
	if (is_visible == 0 || outColor.a < 0.1 || texCoord.y > boundYOuter || (texCoord.x > boundX && texCoord.y > boundYInner)) {
		discard;
	}
}
