#version 330 core

in vec2 out_texture_pos;

out vec4 fragment;

uniform sampler2D textureSampler;

void main() {
    fragment = texture(textureSampler, out_texture_pos);
}