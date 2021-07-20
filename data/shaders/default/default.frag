#version 450 core

layout(location = 0) out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;

uniform sampler2D Diffuse1;
uniform sampler2D Specular1;

const vec3 lightDir = vec3(0.0, 0.0, 1.0);
uniform vec3 viewPos;

const float ambient_strengh = 0.1;
const float specular_strength = 0.5;
const float specular_pow = 256.0;
const vec3 light_color = vec3(1.0, 1.0, 1.0);

void main()
{
    // get diffuse color
    vec3 color = texture(Diffuse1, TexCoords).rgb;

    // get specular color
    float colorSpec = texture(Specular1, TexCoords).r;

    // ambiant
    vec3 ambient = ambient_strengh * light_color * color;

    // diffuse
    float diff = max(dot(lightDir, Normal), 0.0);
    vec3 diffuse = diff * color;

    //specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(lightDir, Normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(Normal, halfwayDir), 0.0), specular_pow);
    vec3 specular = vec3(specular_strength) * spec * colorSpec;

    // total light
    FragColor = vec4(ambient + diffuse + specular, 1.0);
}