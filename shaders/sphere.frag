#version 330 core
#define MAX_LIGHTS 4

in vec3 fragPos;
in vec3 normal;
in vec2 texcoord;

out vec4 FragColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform int numLights;

uniform vec3 viewPos;
uniform Material material;
uniform Light lights[MAX_LIGHTS];

uniform sampler2D texture_diffuse;
uniform bool useTexture;


void main(void)
{

    // normalize
    vec3 norm = normalize(normal);
    // vec3 lightDir = normalize(light.position - fragPos);

    vec3 result = vec3(0.0);

    // vec3 result = vec3(0.4) * material.diffuse; // base ambient floor

    for(int i = 0; i < numLights; ++i){

        // vec3 lightDir = normalize(lights[i].position - fragPos);
        vec3 lightDir = normalize(-lights[i].position);

        // ambient
        vec3 ambient = lights[i].ambient * material.ambient;

        // diffuse
        float diff = max(dot(norm,lightDir),0.0);
        vec3 diffuse = lights[i].diffuse * (diff * material.diffuse);

        // specular
        vec3 viewDir = normalize(viewPos - fragPos);
        vec3 reflectDir = reflect(-lightDir, norm);

        float spec = pow(max(dot(viewDir,reflectDir),0.0),material.shininess);
        // vec3 specular = vec3(0.0);
        vec3 specular = lights[i].specular * (spec * material.specular);

        result += ambient + diffuse + specular;

    }

    if(useTexture){
        vec3 texColor = texture(texture_diffuse,texcoord).rgb;
        result = (result * texColor);
    }

        FragColor = vec4(result,1.0);

}
