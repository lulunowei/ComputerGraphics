#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    float time;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {
    float jitterStrength = 0.02;

    float offsetX = sin(ubo.time * 30.0 + inPosition.y * 10.0) * jitterStrength;
    float offsetY = cos(ubo.time * 35.0 + inPosition.x * 10.0) * jitterStrength;
    float offsetZ = sin(ubo.time * 40.0 + inPosition.z * 10.0) * jitterStrength;

    vec3 jitteredPosition = inPosition + vec3(offsetX, offsetY, offsetZ);

    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(jitteredPosition, 1.0);
    //fragColor = inColor;
    //fragColor = inColor * (0.5 + 0.5 * sin(ubo.time + inPosition.x * 10.0));
    fragColor = inColor;
    fragColor.g = sin(ubo.time) / 2.0f + 0.5f;
    fragTexCoord = inTexCoord;
}