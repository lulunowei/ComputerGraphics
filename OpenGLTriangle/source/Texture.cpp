#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "Texture.h"
#include<string>
#include"../const.h"

/**  
 * @descrip 加载纹理
 * 
 * @functionName:  loadTexture
 * @functionType:    void
 */
void Texture::loadTexture(std::string path)
{
	//加载纹理同时绑定纹理
	unsigned int textureID;
	glGenTextures(1,&textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	//uv 缩放模式
	//float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, Channels;
	unsigned char* data= stbi_load(
		path.c_str(),
		&width,
		&height,
		&Channels,
		0
	);
	if (data) {
		//绑定到当前纹理对象
		glTexImage2D(
			GL_TEXTURE_2D,//纹理目标设置2D
			0,//mipmap
			GL_RGB,
			width,
			height,
			0,
			GL_RGB,
			GL_UNSIGNED_BYTE,
			data//实际纹理资源
		);
		glGenerateMipmap(GL_TEXTURE_2D);//创建mipmaps
	}
	else
	{
		std::cout << "Failed to load texture,path:"+path << std::endl;
	}
	//stbi_set_flip_vertically_on_load(true);
	stbi_image_free(data);
	//存入句柄
	m_textures.push_back(textureID);
}

/**
 * @descrip 绑定纹理，设置纹理坐标和缩放
 * 
 * @functionName:  use
 * @functionType:    void
 */
void Texture::use() {
	for (int i = 0; i < m_textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i); // 激活纹理单元
		glBindTexture(GL_TEXTURE_2D, m_textures[i]); // 绑定对应纹理
	}
}

//构造函数
Texture::Texture():
	m_textures()
{
	loadTexture(TEXTURE_PATH_01);
	loadTexture(TEXTURE_PATH_02);
	use();
}





