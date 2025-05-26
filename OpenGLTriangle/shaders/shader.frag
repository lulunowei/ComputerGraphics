#version 330 core
out vec4 FragColor;
in vec3 BlendColor;
in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float time;
void main()
{
    
    //FragColor = texture(ourTexture,TexCoord)*vec4(ourColor,1.0);//采样器和采样坐标
    vec4 texColor=mix(texture(texture1,TexCoord),texture(texture2,TexCoord),0.2);//混合80%，20%



    //FragColor=texColor*vec4(BlendColor,1.0);//混合80%，20%
    FragColor=texColor;//混合80%，20%
} 