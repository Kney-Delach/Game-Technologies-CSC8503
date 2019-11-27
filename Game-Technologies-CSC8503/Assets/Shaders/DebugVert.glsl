#version 400 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 colour;
layout(location = 2) in vec2 texCoord;

uniform mat4 viewProjMatrix = mat4(1);

uniform int useMatrix = 0;

out Vertex
{
	vec4 colour;
	vec2 texCoord;
} OUT;

void main(void)
{
	vec4 clipPos = vec4(0,0,0,1);
	
	if(useMatrix > 0) {
		clipPos		= viewProjMatrix * vec4(position, 1.0);
	}
	else {
		clipPos		= vec4(position, 1.0);
	}

	gl_Position		= clipPos;
	OUT.texCoord	= texCoord;
	OUT.colour		= colour;
}