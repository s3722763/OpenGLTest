#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texture_pos;

out vec2 out_texture_pos;
out vec3 vNormal;
out vec3 vPos;

uniform mat4 model;
uniform mat4 viewProj;

void main() {
    vec4 worldPosition = model * vec4(position, 1.0);
    
    vNormal = (model * vec4(normal, 1.0)).xyz;
    out_texture_pos = texture_pos;
    vPos = position;

    gl_Position = viewProj * worldPosition;
}