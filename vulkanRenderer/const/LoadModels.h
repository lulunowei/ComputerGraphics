#pragma once
#include<vector>
#include"Data.h"
class LoadModels
{
public:
	static void loadObjModels(
		 std::vector<Vertex> & vertices,
		 std::vector<uint32_t>& indices
		);//加载obj类型数据
	static void loadRectangle(
		std::vector<Vertex>& vertices,
		std::vector<uint32_t>& indices
	);//加载四边形

};

