#version 450 core

layout(location = 1) in vec3 aPos;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec2 aTex;
layout (location = 4) in vec3 aTangent;
layout (location = 5) in vec3 aBitangent;

out vec2 TexCoords;
out vec3 TangentLightDir;
out vec3 TangentViewPos;
out vec3 TangentFragPos;
out vec3 FragPos;

uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;
const vec3 lightDir = vec3(0.0, 0.0, 1.0);
uniform vec3 viewPos;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0)).xyz;
    TexCoords = aTex;

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    mat3 TBN = transpose(mat3(T, B, N));    
    TangentLightDir = TBN * lightDir;
    TangentViewPos  = TBN * viewPos;
    TangentFragPos  = TBN * FragPos;

    mat4 pvm = projection * view * model;

    gl_Position = pvm * vec4(aPos, 1.0);
}