#version 430 core

layout (location = 0) in vec3 v_position;
layout (location = 1) in vec3 v_normal;
layout (location = 2) in vec4 v_color;
layout (location = 3) in vec2 v_texCoords;

uniform mat4 u_model;
uniform mat4 u_cameraProjMat;
uniform mat4 u_cameraViewMat;
uniform mat4 u_lightSpaceMat;

out VS_OUT {
	vec3 fragPos;
	vec3 normal;
	vec2 texCoords;
	vec4 fragPosLightSpace;
	vec4 color;
} vs_out;

void main(){
	vs_out.color = v_color;
	vs_out.normal = mat3(transpose(inverse(u_model))) * v_normal;
	vs_out.texCoords = v_texCoords;
	vs_out.fragPos = vec3(u_model * vec4(v_position, 1.0f));
	vs_out.fragPosLightSpace = u_lightSpaceMat * vec4(vs_out.fragPos, 1.0);

        gl_Position = u_cameraProjMat * u_cameraViewMat * (u_model * vec4(v_position, 1.0f));
}
