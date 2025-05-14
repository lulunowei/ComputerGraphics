#include "Mesh.h"

Mesh::Mesh()
{
}

/**
 * @descrip 顶点缓冲对象在缓冲区中初始化了顶点数据，
 * 设置了一个顶点和片段着色器，
 * 将顶点数据链接到顶点着色器的顶点属性
 * 
 * 
 * @functionName:  initVBO
 * @functionType:    void
 */
void Mesh::initVBO()
{

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int VBO;//顶点缓冲对象
	glGenBuffers(1, &VBO);//生成带缓冲区类型的对象
	glBindBuffer(GL_ARRAY_BUFFER, VBO);//顶点缓冲区类型
	//复制到缓冲区的内存中
	glBufferData(GL_ARRAY_BUFFER,//绑定到缓冲区的对象
		sizeof(vertices),//大小，字节为单位
		vertices,//发送的实际数据
		GL_STATIC_DRAW); //显卡如何管理这些数据：数据只设置一次，多次使用
	//解释顶点数据
	glVertexAttribPointer(
		0, //顶点位置(layout=0)
		3,//vec3
		GL_FLOAT,//数据类型
		GL_FALSE,//是否归一化
		3 * sizeof(float),//步长：每个顶点间隔
		(void*)0
	);
	glEnableVertexAttribArray(0);//启用顶点属性
	//编译链接shader
	ShaderTool::compileShader();
}
