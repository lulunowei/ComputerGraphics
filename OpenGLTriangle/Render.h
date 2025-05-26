#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include"const.h"
#include"Mesh.h"
#include"source/Texture.h"
#include"shaders/Shader.h"
class Render
{
public:
	Render();
	void render();
	virtual ~Render();

	void drawTriangle(const unsigned int shaderProgram);
	void drawRectangle(unsigned int shaderProgram);
	void drawCube(unsigned int shaderProgram);
	void drawCubeArrarys(const Shader &myshader);
private:
	std::shared_ptr<Mesh> m_mesh;//顶点数据
	std::shared_ptr<Texture> m_texture;//纹理数据
};

