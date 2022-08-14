#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

out vec4 v_position;

uniform mat4 cameraToShadowView;
uniform mat4 cameraToShadowProjector;
uniform mat4 model;

//uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() {
	//gl_Position = cameraToShadowProjector * vec4(position, 1.0);
	gl_Position = cameraToShadowProjector * model * vec4(position, 1);

	v_position  = gl_Position;
};