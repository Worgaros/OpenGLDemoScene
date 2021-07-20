#version 450 core

layout(location = 1) in vec3 aPos;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec2 aTex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    mat4 pvm = projection * view * model;
    gl_Position = pvm * vec4(aPos, 1.0);
}