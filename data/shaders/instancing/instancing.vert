#version 450 core

layout(location = 1) in vec3 aPos;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec2 aTex;
layout(location = 6) in mat4 aInstancingMatrix;

uniform mat4 view;
uniform mat4 projection;

out vec4 FragPos;
out vec2 TexCoords;
out vec3 Normal;

void main()
{
    FragPos = aInstancingMatrix * vec4(aPos, 1.0);
    TexCoords = aTex;
    Normal = transpose(inverse(mat3(aInstancingMatrix))) * aNormal;
    mat4 pvm = projection * view * aInstancingMatrix;
    gl_Position = pvm * vec4(aPos, 1.0);
}