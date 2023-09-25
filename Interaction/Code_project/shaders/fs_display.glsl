#version 410 core

in vec2 TexCoords;
out vec4 color;
uniform sampler2D screenTexture;
uniform bool inversecolor;

void main()
{ 
	if(inversecolor){
		color = vec4(1.0 - vec3(texture(screenTexture, TexCoords).xyz), 1.0);
	}
	else{
		color = vec4(texture(screenTexture, TexCoords));
	}
}