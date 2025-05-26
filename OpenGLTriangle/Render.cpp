
#include "Render.h"
#include"shaders/Shader.h"

Render::Render()
{
	m_mesh =
		std::make_unique<Mesh>();
	m_texture =
		std::make_unique<Texture>();
}

void Render::render()
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
	Shader myshader(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);
	myshader.use();
	myshader.setInt("texture1", 0);//纹理一
	myshader.setInt("texture2", 1);//纹理二
	drawCubeArrarys(myshader);
}

void Render::drawTriangle(const unsigned int shaderProgram)
{

	//添加全局颜色变化
	float timeValue = glfwGetTime();
	float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
	int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");//获取ourColor索引
	glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
	glBindVertexArray(m_mesh->triangleVAO);
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
	
}


void Render::drawRectangle(unsigned int shaderProgram)
{
	//添加model变换
	float timeValue = glfwGetTime();


	float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
	int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");//获取ourColor索引
	glUniform4f(vertexColorLocation, 1.0f, greenValue, 1.0f, 1.0f);

	float angle = timeValue * glm::radians(45.0f); // 每秒旋转90度
	float scaleValue = sin(timeValue) / 2.0f + 0.5f;//缩放

	glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::rotate(trans, angle, glm::vec3(0.0, 0.0, 1.0));//沿Z轴旋转90度
	trans = glm::scale(trans, glm::vec3(scaleValue, scaleValue, scaleValue));//缩小
	unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");//获取shader中transform的ID
	glUniformMatrix4fv(transformLoc,1,GL_FALSE,glm::value_ptr(trans));


	glBindVertexArray(m_mesh->squareVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
void Render::drawCube(unsigned int shaderProgram)
{
	glm::mat4 model = glm::mat4(1.0f);//模型矩阵
	model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
	int modelLoc = glGetUniformLocation(shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glm::mat4 view = glm::mat4(1.0f);//视图矩阵
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));//移动整个场景
	int viewLoc = glGetUniformLocation(shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glm::mat4 projection;//投影矩阵
	projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f); 
	int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glBindVertexArray(m_mesh->cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);//绘制立方体

}
void Render::drawCubeArrarys(const Shader& myshader)
{
	glBindVertexArray(m_mesh->cubeVAO);
	//glm::mat4 view = glm::mat4(1.0f);//视图矩阵
		//创建视图矩阵
	const float radius = 10.0f;
	float camX = sin(glfwGetTime()) * radius;
	float camZ = cos(glfwGetTime()) * radius;
	glm::mat4 view;
	view = glm::lookAt(
		glm::vec3(camX, 0.0f, camZ),//镜头坐标
		glm::vec3(0.0f, 0.0f, 0.0f),//目标
		glm::vec3(0.0f, 1.0f, 0.0f));//上方向
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));//移动整个场景
	int viewLoc = glGetUniformLocation(myshader.ID, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glm::mat4 projection;//投影矩阵
	projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
	int projectionLoc = glGetUniformLocation(myshader.ID, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	for (unsigned int i = 0; i < 10; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, cubePositions[i]);
		//float angle = 20.0f * i;
		//model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f)*(i+1), glm::vec3(0.5f, 1.0f, 0.0f));
		//myshader.setMat4("model", model);
		int modelLoc = glGetUniformLocation(myshader.ID, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	myshader.setFloat("time", glfwGetTime()); // 设置给 shader
	//glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);//相机世界坐标位置
	//glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);//目标点
	////相机前方向
	//glm::vec3 cameraDirection = glm::normalize(cameraPos- cameraTarget);
	//glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	////相机右方向
	//glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));//up叉乘cameraDireaction
	////相机上方向
	//glm::vec3 cameraUp = glm::normalize(glm::cross(cameraDirection, cameraRight));


}
Render::~Render()
{
}
