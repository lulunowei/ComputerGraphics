#pragma once
#include <glad/glad.h> 
#include<string>
#include<vector>
class Texture
{
public:
	//texture ID
	std::vector<unsigned int> m_textures;
	//activate and bind
	void use();
	Texture();
	~Texture() = default;
private:
	//load texture
	void loadTexture(std::string path);
};

