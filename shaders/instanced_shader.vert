#version 330 core

layout (location = 0) in vec3 position;

layout (location = 1) in vec3 color;

layout (location = 2) in vec2 texCoord;

layout (location = 3) in vec3 normal;

layout (location = 4) in vec3 offset;

out vec3 positionOut;
out vec3 colorOut;
out vec2 texCoordOut;
out vec3 normalOut;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	vec4 posv4 = model * vec4(position, 1.0) + vec4(offset, 1.0);

	positionOut = vec3(posv4);

	gl_Position = projection * view * posv4;
	
	colorOut = color;

	texCoordOut = texCoord;

	normalOut = mat3(transpose(inverse(model))) * normal;
}