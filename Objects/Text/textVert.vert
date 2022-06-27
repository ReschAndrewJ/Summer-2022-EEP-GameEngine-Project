#version 460

vec2 vertices[4] = vec2[](
	vec2(-1.0, -1.0), vec2(1.0, -1.0),
	vec2(-1.0, 1.0), vec2(1.0, 1.0)
);

vec2 texture_coordinates[4] = vec2[](
	vec2(0,0), vec2(1,0),
	vec2(0,1), vec2(1,1)
);

layout (push_constant) uniform pushConstants_block {
	mat4 transformation_matrix;
	int boundX;
	int boundYInner;
	int boundYOuter;
	int is_visible;
} pushConstants;

layout (location = 0) out vec2 fragTexCoord;
layout (location = 1) out int is_visible;
layout (location = 2) out int boundX;
layout (location = 3) out int boundYInner;
layout (location = 4) out int boundYOuter;

void main() {
	gl_Position = pushConstants.transformation_matrix * vec4(vertices[gl_VertexIndex], 0.0, 1.0);
	fragTexCoord = texture_coordinates[gl_VertexIndex];
	is_visible = pushConstants.is_visible;
	boundX = pushConstants.boundX;
	boundYInner = pushConstants.boundYInner;
	boundYOuter = pushConstants.boundYOuter;
}