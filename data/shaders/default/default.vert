#version 450 core

layout(location = 1) in vec3 aPos;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec2 aTex;

out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;

uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0)).xyz;
    TexCoords = aTex;
    Normal = transpose(inverse(mat3(model))) * aNormal;
    mat4 pvm = projection * view * model;
    gl_Position = pvm * vec4(aPos, 1.0);
}