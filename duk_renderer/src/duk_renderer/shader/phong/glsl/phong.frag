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
    float shininess;
    vec2 uvScale;
};

layout(binding = 3) buffer PropertiesUBO {
    DUK_INSTANCE_DECLARE(Properties);
} uProperties;

layout(binding = 4) uniform sampler2D uBaseColor;
layout(binding = 5) uniform sampler2D uSpecular;

void main() {
    vec3 view = normalize(duk_camera_position() - vPosition);
    vec3 normal = normalize(vNormal);

    Properties properties = DUK_INSTANCE_GET(uProperties, vInstanceIndex);

    float shininess = max(1, (properties.shininess * texture(uSpecular, vTexCoord * properties.uvScale).r));

    vec3 lighting = duk_calculate_phong_lights(shininess, vPosition, normal, view);

    vec4 color = texture(uBaseColor, vTexCoord * properties.uvScale) * properties.color;

    oColor = color * vec4(lighting, 1.0);
}
