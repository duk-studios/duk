#version 450
#extension GL_ARB_separate_shader_objects : enable

#include "duk/camera.glsl"
#include "duk/instance.glsl"
#include "duk/phong_lighting.glsl"

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexCoord;
layout(location = 3) in flat int vInstanceIndex;

layout(location = 0) out vec4 oColor;

struct Properties {
    vec4 color;
    vec3 emissiveColor;
    float shininess;
    vec2 uvScale;
};

readonly buffer PropertiesUBO {
    DUK_INSTANCE_DECLARE(Properties);
} uProperties;

uniform sampler2D uBaseColor;
uniform sampler2D uSpecular;
uniform sampler2D uEmissive;

void main() {
    vec3 view = normalize(duk_camera_position() - vPosition);
    vec3 normal = normalize(vNormal);
    Properties properties = DUK_INSTANCE_GET(uProperties, vInstanceIndex);
    vec2 texCoord = vTexCoord * properties.uvScale;

    float shininess = max(1, (properties.shininess * texture(uSpecular, texCoord).r));

    vec3 lighting = duk_calculate_phong_lights(shininess, vPosition, normal, view);

    vec4 color = texture(uBaseColor, texCoord) * properties.color;

    vec3 emissive = texture(uEmissive, texCoord).rgb;

    // prevent lighting from affecting the emissive color
    lighting *= vec3(1.0) - emissive;

    oColor = color * vec4(lighting, 1.0) + vec4(emissive * properties.emissiveColor, 0.0);
}
