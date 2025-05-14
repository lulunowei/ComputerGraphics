#pragma once
#include"const.h"
#include"Mesh.h"
class Render
{
public:
	Render();
	void render();

	virtual ~Render();
private:
	Mesh mesh;
};

