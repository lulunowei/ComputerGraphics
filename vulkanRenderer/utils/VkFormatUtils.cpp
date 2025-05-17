#include "VkFormatUtils.h"
#include <stdexcept>

/**
 * @descrip 查找支持的格式，从最期望到最不期望的数组中找到期望的格式
 * 遍历 candidates 列表，查找 在指定 tiling 模式下，支持要求功能的第一个格式
 * 
 * @functionName:  findSupportedFormat
 * @functionType:    VkFormat
 * @param candidates 格式列表
 * @param tiling 图像的布局模式
 * @param features 格式支持的功能
 * @return    
 */
VkFormat VkFormatUtils::findSupportedFormat(
	VkPhysicalDevice physicalDevice,
	const std::vector<VkFormat>& candidates, 
	VkImageTiling tiling,
	VkFormatFeatureFlags features)
{
	for (auto format : candidates) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR &&
			(props.linearTilingFeatures & features) ==
			features) {
			return format;
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL &&
			(props.optimalTilingFeatures & features) ==
			features) {
			return format;
		}
	}
	throw std::runtime_error("failed to find supported format!");
}

/**
 * @descrip 选择深度组件格式
 * 
 * @functionName:  findDepthFormat
 * @functionType:    VkFormat
 * @return    
 */
VkFormat VkFormatUtils::findDepthFormat(VkPhysicalDevice physicalDevice)
{
	return findSupportedFormat(
		physicalDevice,
		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },//格式列表
		VK_IMAGE_TILING_OPTIMAL,//最适合 GPU 访问的图像布局模式
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT//用作深度/模板附件
	);
}

/**
 * @descrip 检查深度格式包含模板组件
 * 
 * @functionName:  hasStencilComponent
 * @functionType:    bool
 * @param format
 * @return    
 */
bool VkFormatUtils::hasStencilComponent(VkFormat format)
{
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;

}
