#version 460

vec2 vertices[4] = vec2[](
	vec2(-1.0, -1.0), vec2(1.0, -1.0), 
	vec2(-1.0, 1.0), vec2(1.0, 1.0)
);

layout (push_constant) uniform pushConstants_block {
	mat4 transformation_matrix;
	vec2[4] texture_coordinates;
} pushConstants;

layout (location = 0) out vec2 fragTexCoord;

void main() {
	gl_Position = pushConstants.transformation_matrix * vec4(vertices[gl_VertexIndex], 0.0, 1.0);
	fragTexCoord = pushConstants.texture_coordinates[gl_VertexIndex];
}
