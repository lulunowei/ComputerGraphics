#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model[10];
    mat4 view;
    mat4 proj;
    float time;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in uint instanceID;


layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;



void main() {
    mat4 model = ubo.model[gl_InstanceIndex];
    gl_Position = ubo.proj * ubo.view * model* vec4(inPosition, 1.0);
    fragColor = inColor;
    fragTexCoord = inTexCoord;
}