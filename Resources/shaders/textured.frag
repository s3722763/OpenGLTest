#version 330 core
#define MAXLIGHTS 10
#define AMBIENT vec3(0.1, 0.1, 0.1)

in vec2 out_texture_pos;
in vec3 vNormal;
in vec3 vPos;

out vec4 fragment;

struct Light {
    vec4 position;
    vec4 color;
};

uniform uint uLightCount;
uniform Light uLights[MAXLIGHTS];

uniform vec4 eyePosition;

uniform sampler2D textureSampler;

vec3 lit(vec3 light, vec3 normal, vec3 vector) {
    vec3 reflected_light = reflect(-light, normal);
    return reflected_light;
}

void main() {
    vec3 color = texture(textureSampler, out_texture_pos).rgb;
    
    vec3 norm = normalize(vNormal);
    vec3 lightDir = normalize(uLights[0].position.xyz - vPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * uLights[0].color.rgb;

    vec3 result = (AMBIENT + diffuse) * color;
    fragment = vec4(result, 1.0);
}