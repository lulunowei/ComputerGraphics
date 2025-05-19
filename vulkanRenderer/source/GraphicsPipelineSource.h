#pragma once
#include<vulkan/vulkan.h>
struct GraphicPipeLineCreateInfo {
	VkDevice device;
	VkSampleCountFlagBits rasterizationSamples;
	VkDescriptorSetLayout descriptorSetLayout;
	VkRenderPass          renderPass;

};
class GraphicsPipelineSource
{
public:
	GraphicsPipelineSource() = default;
	~GraphicsPipelineSource() = default;
	void initGraphicsPipeline(const GraphicPipeLineCreateInfo& graphicpipelineInfo);
	VkPipeline &getGraphicsPipeline() { return m_graphicsPipeline; }
	VkPipelineLayout &getPipelineLayout() { return m_pipelineLayout; }
	
private:
	void createGraphicsPipeline(const GraphicPipeLineCreateInfo& graphicpipelineInfo);//创建图形管道
private:
	VkPipelineLayout m_pipelineLayout;//图形管道布局
	VkPipeline m_graphicsPipeline;//图形管道,用于被继承
};

