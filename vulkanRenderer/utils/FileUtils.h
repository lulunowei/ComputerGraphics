#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>
#include<vulkan/vulkan.h>
class FileUtils {
public:
    //读取文件
    static std::vector<char> readFile(const std::string& filename);
    //读取shader封装成字节码
    static VkShaderModule createShaderModule(const std::vector<char>& code,VkDevice device);//创建着色器模块

};

