#version 450 core

layout(location = 0) out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoords;
in vec3 TangentLightDir;
in vec3 TangentViewPos;
in vec3 TangentFragPos;

uniform sampler2D Diffuse1;
uniform sampler2D Specular1;
uniform sampler2D Normal1;

uniform vec3 viewPos;

const float ambient_strengh = 0.1;
const float specular_strength = 0.5;
const float specular_pow = 256.0;
const vec3 light_color = vec3(1.0, 1.0, 1.0);

void main()
{
    // obtain normal from normal map in range [0,1]
    vec3 normal = texture(Normal1, TexCoords).rgb;
    // transform normal vector to range [-1,1]
    normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space

    // get diffuse color
    vec3 color = texture(Diffuse1, TexCoords).rgb;

    //get specular color
    float colorSpec = texture(Specular1, TexCoords).r;

    // ambiant
    vec3 ambient = ambient_strengh * light_color * color;

    // diffuse
    vec3 lightDir = TangentLightDir;
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;

    //specular
    vec3 viewDir = normalize(TangentViewPos - TangentFragPos);
    vec3 reflectDir = reflect(lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), specular_pow);
    vec3 specular = vec3(specular_strength) * spec * colorSpec;

    // total light
    FragColor = vec4(ambient + diffuse + specular, 1.0);
}