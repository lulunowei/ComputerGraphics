#include "DescriptorSource.h"
#include <stdexcept>
#include<array>
#include"../const/Data.h"

/**
 * @descrip 创建描述符布局
 * 
 * @functionName:  createDescriptorSetLayout
 * @functionType:    void
 * @param device
 */
void DescriptorSource::createDescriptorSetLayout(VkDevice device)
{
	//ubo描述符布局
	VkDescriptorSetLayoutBinding uboLayoutBinding{};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;//只有一个uniform buffer
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;//顶点着色器阶段
	uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

	//sampler描述符布局
	VkDescriptorSetLayoutBinding samplerLayoutBinding{};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;//片段着色器阶段
	samplerLayoutBinding.pImmutableSamplers = nullptr;

	//布局存放到一个数组
	std::array<VkDescriptorSetLayoutBinding, 2> bindings{ uboLayoutBinding,samplerLayoutBinding };

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &m_descriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}

/**
 * @descrip 
 * 
 * @functionName:  createDescriptorPool
 * @functionType:    void
 */
void DescriptorSource::createDescriptorPool(VkDevice device)
{
	std::array<VkDescriptorPoolSize, 2> poolSizes{};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;//枚举值：6
	poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);//总共的槽位
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;//枚举值：1
	poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);//总共的槽位

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;//枚举值:33
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);//最大描述符集数量

	//创建描述符集池
	if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

/**
 * @descrip 创建描述符集
 * 
 * @functionName:  createDescriptorSets
 * @functionType:    void
 * @param descriptorSetsInfo
 */
void DescriptorSource::createDescriptorSets(const DescriptorSetsCreateInfo& descriptorSetsInfo)
{
	//一次性创建多个set,绑定到同个描述符布局
	std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, m_descriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = m_descriptorPool;//通过池分配set
	allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);//指定layouts长度
	allocInfo.pSetLayouts = layouts.data();//为每个分配的描述符集指明描述符布局

	m_descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
	//分配描述符集，设置句柄保留sets副本
	if (vkAllocateDescriptorSets(descriptorSetsInfo.device, &allocInfo, m_descriptorSets.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	//填充描述符，把哪个 VkBuffer 绑定到哪个 set 的哪个 binding 插槽
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {

		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = descriptorSetsInfo.UBO[i];//指定缓冲区
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);

		VkDescriptorImageInfo imageInfo{};
		imageInfo.sampler = descriptorSetsInfo.textureSampler;
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = descriptorSetsInfo.textureImageView;

		std::array<VkWriteDescriptorSet, 2> descriptorWrites{};
		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = m_descriptorSets[i];
		descriptorWrites[0].dstBinding = 0;//指定统一描述符索引是0
		descriptorWrites[0].dstArrayElement = 0;//没使用数组，从索引为0开始
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;//再次指定描述符类型
		descriptorWrites[0].descriptorCount = 1;//更新描述符数组元素数量
		descriptorWrites[0].pBufferInfo = &bufferInfo;

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = m_descriptorSets[i];
		descriptorWrites[1].dstBinding = 1;//指定统一描述符索引是0
		descriptorWrites[1].dstArrayElement = 0;//没使用数组，从索引为0开始
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;//再次指定描述符类型
		descriptorWrites[1].descriptorCount = 1;//更新描述符数组元素数量
		descriptorWrites[1].pImageInfo = &imageInfo;


		//vulkan进行绑定实际资源到set的binding槽上
		vkUpdateDescriptorSets(descriptorSetsInfo.device,
			static_cast<uint32_t>(descriptorWrites.size()),//传入更改的descriptorWrites数量
			descriptorWrites.data(),
			0,
			nullptr);
	}



}
