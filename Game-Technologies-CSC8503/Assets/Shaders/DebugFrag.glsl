#version 400 core

uniform sampler2D 	mainTex;

in Vertex
{
	vec4 colour;
	vec2 texCoord;
} IN;

out vec4 fragColor;

void main(void)
{
	if(textureSize(mainTex, 0).x < 10) {
		fragColor = IN.colour;
	}
	else {
		float alpha = texture(mainTex, IN.texCoord).r;
		
		if(alpha < 0.00001f) {
			discard;
		}
		
		fragColor = IN.colour * vec4(1,1,1,alpha);
	}
}