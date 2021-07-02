#version 330 core
#define MAXLIGHTS 10

in vec4 out_color;
in vec3 vNormal;
in vec3 vColor;
in vec3 vPos;

out vec4 fragment;

struct Light {
    vec4 position;
    vec4 color;
};

uniform uint uLightCount;
uniform LightUBlock {
    Light uLights[MAXLIGHTS];
};

uniform vec4 eyePosition;

vec3 lit(vec3 light, vec3 normal, vec3 vector) {
    vec3 reflected_light = reflect(-light, normal);
    return reflected_light;
}

void main() {
    vec3 normal = normalize(vNormal);
    vec3 vector = normalize(eyePosition.xyz - vPos); 

    fragment = vec4(0.0);

    for (uint i = 0u; i < uLightCount; i++) {
        vec3 light = normalize(uLights[i].position.xyz - vPos);
        float NdL = clamp(dot(normal, light), 0.0, 1.0);
        fragment.rgb += NdL * uLights[i].color.rgb * lit(light, normal, vector);
    }
}