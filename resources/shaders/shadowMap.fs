#version 330

in vec4 v_position;
out vec4 outColor;
	
// From http://fabiensanglard.net/shadowmappingVSM/index.php
void main() 
{
	float depth = v_position.z / v_position.w;
	depth = depth * 0.5 + 0.5;

	float moment1 = depth;
	float moment2 = depth * depth;

	float dx = dFdx(depth);
	float dy = dFdy(depth);
	moment2 += 0.25*(dx*dx+dy*dy);
	outColor = vec4( moment1, moment2, 0.0, 0.0);

};