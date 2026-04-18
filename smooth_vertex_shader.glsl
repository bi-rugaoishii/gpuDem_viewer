#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec4 instanceData; // xyz + radius

out vec3 FragPos;
out vec3 Normal;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec3 pos = aPos * instanceData.w + instanceData.xyz;

    FragPos = pos;
    Normal = aNormal;

    gl_Position = projection * view * vec4(pos,1.0);
}
