#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexture;

out vec3 BlendColor;
out vec2 TexCoord;

uniform vec3 ourColor;
uniform mat4 transform;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos,1.0);
    BlendColor=ourColor;
    TexCoord = aTexture;
}