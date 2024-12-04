#version 430 core
      
layout(triangles, invocations = 5) in;
layout(triangle_strip, max_vertices = 3) out;

layout (std140, binding = 1) uniform ShadowLightSpacesData
{
    mat4 matrices[16];
    float planes[16];
    int planesCount;
} u_shadow_light_spaces_data;
    
void main()
{          
    for (int i = 0; i < 3; ++i) {
        gl_Position = 
            u_shadow_light_spaces_data.matrices[gl_InvocationID] * gl_in[i].gl_Position;
        gl_Layer = gl_InvocationID;
        EmitVertex();
    }
    EndPrimitive();
} 
