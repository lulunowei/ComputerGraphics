#include "Mesh.h"

Mesh::Mesh()
{
	initCube();
	//initTriangle();
	//initSquare();
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
void Mesh::initSquare()
{

	glGenVertexArrays(1, &squareVAO);//1代表生成几个ID，存储到VBO
	GLuint squareVBO;//顶点缓冲对象
	glGenBuffers(1, &squareVBO);
	GLuint squareEBO;//索引对象
	glGenBuffers(1, &squareEBO);

	glBindVertexArray(squareVAO);//切换状态机到VAO(全局状态机)
	glBindBuffer(GL_ARRAY_BUFFER, squareVBO);//切换状态机到VBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, squareEBO);//切换状态机到EBO

	//复制顶点数据到缓冲区的内存中
	glBufferData(GL_ARRAY_BUFFER,//绑定到缓冲区的对象
		sizeof(squareVertices),//大小，字节为单位
		squareVertices,//发送的实际数据
		GL_STATIC_DRAW); //显卡如何管理这些数据：数据只设置一次，多次使用
	//复制索引数据到缓冲区的内存中
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
		sizeof(squareIndices),
		squareIndices,
		GL_STATIC_DRAW);
	//配置顶点属性数据
	glVertexAttribPointer(
		0, //顶点位置(layout=0)
		3,//vec3
		GL_FLOAT,//数据类型
		GL_FALSE,//是否归一化
		8 * sizeof(float),//步长：每个顶点间隔
		(void*)0//偏移0开始
	);
	glEnableVertexAttribArray(0);//启用顶点属性

	// 配置颜色属性 layout(location = 1)
	glVertexAttribPointer(
		1,
		3, 
		GL_FLOAT,
		GL_FALSE,
		8 * sizeof(float),
		(void*)(3 * sizeof(float)) // 从颜色位置偏移开始
	);
	glEnableVertexAttribArray(1);


	//配置顶点纹理属性
	glVertexAttribPointer(
		2, //顶点位置(layout=2)
		2,//vec2
		GL_FLOAT,//数据类型
		GL_FALSE,//是否归一化
		8 * sizeof(float),//步长：每个顶点间隔
		(void*)(6 * sizeof(float))
	);
	glEnableVertexAttribArray(2);
}

void Mesh::initTriangle()
{

	glGenVertexArrays(1, &triangleVAO);//1代表生成几个ID，存储到VBO
	GLuint triangleVBO;//顶点缓冲对象
	glGenBuffers(1, &triangleVBO);
	GLuint triangleEBO;//索引对象
	glGenBuffers(1, &triangleEBO);

	glBindVertexArray(triangleVAO);//切换状态机到VAO(全局状态机)
	glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);//切换状态机到VBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleEBO);//切换状态机到EBO

	//复制顶点数据到缓冲区的内存中
	glBufferData(GL_ARRAY_BUFFER,//绑定到缓冲区的对象
		sizeof(triangleVertices),//大小，字节为单位
		triangleVertices,//发送的实际数据
		GL_STATIC_DRAW); //显卡如何管理这些数据：数据只设置一次，多次使用
	//复制索引数据到缓冲区的内存中
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		sizeof(triangleIndices),
		triangleIndices,
		GL_STATIC_DRAW);
	//配置顶点属性
	glVertexAttribPointer(
		0, //顶点位置(layout=0)
		3,//vec3
		GL_FLOAT,//数据类型
		GL_FALSE,//是否归一化
		8 * sizeof(float),//步长：每个顶点间隔
		(void*)0
	);
	glEnableVertexAttribArray(0);//启用顶点属性
	//配置顶点颜色属性
	glVertexAttribPointer(
		1,//颜色位置
		3,
		GL_FLOAT,
		GL_FALSE,
		8 * sizeof(float), 
		(void*)(3 * sizeof(float))
	);
	glEnableVertexAttribArray(1);//启用顶点属性

	//配置顶点纹理属性
	glVertexAttribPointer(
		2,
		2,//vec2
		GL_FLOAT,
		GL_FALSE,
		8 * sizeof(float),
		(void*)(6*sizeof(float))
	);
	glEnableVertexAttribArray(2);//启用顶点属性
}

void Mesh::initCube()
{
	glGenVertexArrays(1, &cubeVAO);//1代表生成几个ID，存储到VBO
	GLuint cubeVBO;//顶点缓冲对象
	glGenBuffers(1, &cubeVBO);
	GLuint cubeEBO;//索引对象
	glGenBuffers(1, &cubeEBO);

	glBindVertexArray(cubeVAO);//切换状态机到VAO(全局状态机)
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);//切换状态机到VBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);//切换状态机到EBO

	//复制顶点数据到缓冲区的内存中
	glBufferData(GL_ARRAY_BUFFER,//绑定到缓冲区的对象
		sizeof(cubeVertices),//大小，字节为单位
		cubeVertices,//发送的实际数据
		GL_STATIC_DRAW); //显卡如何管理这些数据：数据只设置一次，多次使用
	//配置顶点属性
	glVertexAttribPointer(
		0, //顶点位置(layout=0)
		3,//vec3
		GL_FLOAT,//数据类型
		GL_FALSE,//是否归一化
		5 * sizeof(float),//步长：每个顶点间隔
		(void*)0
	);
	glEnableVertexAttribArray(0);//启用顶点属性

	//配置顶点纹理属性
	glVertexAttribPointer(
		2,
		2,//vec2
		GL_FLOAT,
		GL_FALSE,
		5 * sizeof(float),
		(void*)(3 * sizeof(float))
	);
	glEnableVertexAttribArray(2);//启用顶点属性

}
