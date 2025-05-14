#pragma once
#include"const.h"
#include"Utils/ShaderTool.h"
namespace {
	float vertices[9] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f,  0.5f, 0.0f
	};
}

class Mesh
{
public:
	Mesh();
	void initVBO();


};

