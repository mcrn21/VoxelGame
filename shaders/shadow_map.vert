#version 430 core

layout (location = 0) in vec3 v_position;
layout (location = 1) in vec3 v_normal;
layout (location = 2) in vec4 v_color;
layout (location = 3) in vec2 v_texCoord;

uniform mat4 u_model;
uniform mat4 u_lightSpaceMat;

void main(){
    gl_Position = u_lightSpaceMat * (u_model * vec4(v_position, 1.0f));
}
