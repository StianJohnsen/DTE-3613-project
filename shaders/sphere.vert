#version 330 core
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord;

out vec3 fragPos;
out vec3 normal;
out vec2 texcoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(void)
{

    vec4 worldPos = model * vec4(in_position,1.0);
    fragPos = worldPos.xyz;
    normal = normalize(mat3(transpose(inverse(model))) * in_normal);

    texcoord = in_texcoord;

    gl_Position = projection * view * worldPos;

}
