#pragma once
#include<vulkan/vulkan.h>
#include<vector>
class VkFormatUtils
{
public:
	static VkFormat findSupportedFormat(
        VkPhysicalDevice physicalDevice,
        const std::vector<VkFormat>& candidates,
        VkImageTiling tiling,
        VkFormatFeatureFlags features);//查找支持的格式

    static VkFormat findDepthFormat(VkPhysicalDevice physicalDevice);//选择深度组件格式

    static bool hasStencilComponent(VkFormat format);//检查深度格式包含模板组件
};

