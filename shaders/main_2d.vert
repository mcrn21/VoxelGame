#version 330 core

layout (location = 0) in vec3 v_position;
layout (location = 1) in vec4 v_color;
layout (location = 2) in vec2 v_texCoord;

uniform mat4 u_model;
uniform mat4 u_projection;

out vec4 a_color;
out vec2 a_texCoord;

void main(){
    a_color = v_color;
    a_texCoord = v_texCoord;

    gl_Position = u_projection * (u_model * vec4(v_position, 1.0f));
}
