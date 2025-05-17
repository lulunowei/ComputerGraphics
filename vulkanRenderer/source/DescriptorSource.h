#pragma once
#include<vulkan/vulkan.h>
#include<vector>
//创建framebuffer的参数传递
struct DescriptorSetsCreateInfo {
	VkDevice device = VK_NULL_HANDLE;
	std::vector<VkBuffer> UBO{};
	VkSampler textureSampler;
	VkImageView textureImageView;
};


class DescriptorSource
{
public:
	DescriptorSource() = default;
	~DescriptorSource() = default;
private:
	VkDescriptorSetLayout m_descriptorSetLayout;//描述符绑定，指定管线访问的资源
	VkDescriptorPool m_descriptorPool;//描述符集池
	std::vector<VkDescriptorSet> m_descriptorSets;//描述符集合
private:
	void createDescriptorSetLayout(VkDevice device = VK_NULL_HANDLE);//创建描述符布局
	void createDescriptorPool(VkDevice device = VK_NULL_HANDLE);//创建描述集池
	void createDescriptorSets(const DescriptorSetsCreateInfo& descriptorSetsInfo);//创建描述符集


};

