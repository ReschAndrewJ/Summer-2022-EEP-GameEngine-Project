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
	int is_visible;
} pushConstants;

layout (location = 0) out vec2 fragTexCoord;
layout (location = 1) out int is_visible;

void main() {
	gl_Position = pushConstants.transformation_matrix * vec4(vertices[gl_VertexIndex], 0.0, 1.0);
	fragTexCoord = texture_coordinates[gl_VertexIndex];
	is_visible = pushConstants.is_visible;
}