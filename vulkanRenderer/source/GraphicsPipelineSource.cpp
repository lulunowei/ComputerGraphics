#include "GraphicsPipelineSource.h"
#include"../utils/FileUtils.h"
#include"../const/Data.h"

void GraphicsPipelineSource::initGraphicsPipeline(const GraphicPipeLineCreateInfo& graphicpipelineInfo)
{
	createGraphicsPipeline(graphicpipelineInfo);
}



/**
 * @descrip 创建图像管道
 * 
 * @functionName:  createGraphicsPipeline
 * @functionType:    void
 */
void GraphicsPipelineSource::createGraphicsPipeline(const GraphicPipeLineCreateInfo& graphicpipelineInfo)
{
	auto vertShaderCode = FileUtils::readFile("shaders/vert.spv");
	auto fragShaderCode = FileUtils::readFile("shaders/frag.spv");

	//封装字节码
	VkShaderModule vertShaderModule = FileUtils::createShaderModule(vertShaderCode, graphicpipelineInfo.device);
	VkShaderModule fragShaderModule = FileUtils::createShaderModule(fragShaderCode, graphicpipelineInfo.device);

	//创建vertext shader
	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;//着色阶段：顶点着色阶段
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	//创建fragment shader
	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };//保存到数组

	//vertex shader顶点的输入信息
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};

	auto bindingDescription = Vertex::getBindingDescription();
	auto attributeDescriptions = getAttributeDescriptions();

	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	//绑定描述和属性
	vertexInputInfo.vertexBindingDescriptionCount = 2;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());//顶点属性数量
	vertexInputInfo.pVertexBindingDescriptions = bindingDescription.data();//描述顶点结构
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();//描述结构体字段



	//顶点数据组织成几何图元
	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;//每三个顶点组成三角形
	//inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	//视口和裁剪
	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;

	//光栅化变成片元
	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_NONE;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;//
	rasterizer.depthBiasEnable = VK_FALSE;

	//多重采样
	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_TRUE;//单独计算每个样本，对整个像素样本的平均值进行处理
	multisampling.rasterizationSamples = graphicpipelineInfo.rasterizationSamples;//使用MSAA

	//颜色混合
	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
		VK_COLOR_COMPONENT_G_BIT |
		VK_COLOR_COMPONENT_B_BIT |
		VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;

	//全局配置颜色混合
	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;

	//声明视口和剪裁区域为动态状态
	std::vector<VkDynamicState> dynamicStates = {
		  VK_DYNAMIC_STATE_VIEWPORT,
		  VK_DYNAMIC_STATE_SCISSOR
	};
	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates = dynamicStates.data();

	//定义深度测试
	VkPipelineDepthStencilStateCreateInfo depthStencil{};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.minDepthBounds = 0.0f; // Optional
	depthStencil.maxDepthBounds = 1.0f; // Optional
	depthStencil.stencilTestEnable = VK_FALSE;
	depthStencil.front = {}; // Optional
	depthStencil.back = {}; // Optional

	//声明管线布局，用于访问外部资源
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &graphicpipelineInfo.descriptorSetLayout;//创建的管线绑定描述符布局
	//pipelineLayoutInfo.pushConstantRangeCount = 0;

	//创建管线布局
	if (vkCreatePipelineLayout(graphicpipelineInfo.device, &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}


	//定义管线
	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;

	pipelineInfo.pVertexInputState = &vertexInputInfo;//管线的输入
	pipelineInfo.pInputAssemblyState = &inputAssembly;//管线的顶点组成assmebler
	pipelineInfo.pViewportState = &viewportState;//视口
	pipelineInfo.pRasterizationState = &rasterizer;//光栅化
	pipelineInfo.pMultisampleState = &multisampling;//多采样
	pipelineInfo.pDepthStencilState = &depthStencil;//深度测试
	pipelineInfo.pColorBlendState = &colorBlending;//颜色混合
	pipelineInfo.pDynamicState = &dynamicState;//动态裁剪
	pipelineInfo.layout = m_pipelineLayout;//布局
	pipelineInfo.renderPass = graphicpipelineInfo.renderPass;//绑定的RenderPass
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional

	//创建管线
	if (vkCreateGraphicsPipelines(graphicpipelineInfo.device,
		VK_NULL_HANDLE,
		1,
		&pipelineInfo,
		nullptr,
		&m_graphicsPipeline) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline!");
	}

	vkDestroyShaderModule(graphicpipelineInfo.device, fragShaderModule, nullptr);
	vkDestroyShaderModule(graphicpipelineInfo.device, vertShaderModule, nullptr);

}
