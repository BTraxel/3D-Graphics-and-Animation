#version 410 core

layout(location = 0) in vec3 position;

out vec3 TexCoords;

uniform mat4 projection; 
uniform mat4 view;

void main(){
    vec4 pos = projection*view*vec4(position, 1.0f);
    gl_Position = vec4(pos.x, pos.y, pos.w, pos.w);
    TexCoords = vec3(position.x, position.y, -position.z);
}