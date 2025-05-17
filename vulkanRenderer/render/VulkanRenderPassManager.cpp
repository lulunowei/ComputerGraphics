#include "VulkanRenderPassManager.h"
#include<vulkan/vulkan.h>
#include"../const.h"

VulkanRenderPassManager::VulkanRenderPassManager(
	ApplicationContext& context):
	m_context(context)
{

}

/**
 * @descrip 渲染流程，定义渲染前后怎么处理像素数据
 * 
 * @functionName:  createRenderPass
 * @functionType:    void
 */
void VulkanRenderPassManager::createRenderPass(
	VkFormat colorFormat,
	VkFormat depthFormat,
	VkSampleCountFlagBits msaaSamples
)
{
	//颜色附件
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = colorFormat;
	colorAttachment.samples = msaaSamples;//多重采样
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;//渲染前清除附件内容(清除为黑色)
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;//渲染后保存附件内容
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;//不关心模板数据
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;//不关心模板数据
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;// 初始布局	,视为“未初始化”状态
	//colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;// 最终布局,图像将被提交到交换链进行屏幕呈现。
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;// 最终布局,图像将被提交到离线屏幕处理多重采样，多重采样不能直接呈现到屏幕

	//深度附件
	VkAttachmentDescription depthAttachment{};
	//depthAttachment.format = findDepthFormat();
	depthAttachment.format = depthFormat;
	depthAttachment.samples = msaaSamples;//多重采样
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;//渲染前清除附件内容(清除为黑色)
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;//渲染后不用保存附件内容
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;//不关心模板数据
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;//不关心模板数据
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;// 初始布局	,视为“未初始化”状态
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;// 最适合 GPU 读取深度和模板缓冲的布局，也不会呈现

	//解析附件
	VkAttachmentDescription colorAttachmentResolve{};
	colorAttachmentResolve.format = colorFormat;
	colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;//单采样
	colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;//呈现到屏幕

	//。。。其他附件（深度等）

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0; //引用主流程附件列表中的第一个附件
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;// 子流程运行时附件的布局

	VkAttachmentReference depthAttachmentRef{};
	depthAttachmentRef.attachment = 1; //引用主流程附件列表中的第二个附件
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;// 子流程运行时附件的布局

	VkAttachmentReference colorAttachmentResolveRef{};
	colorAttachmentResolveRef.attachment = 2;//引用主流程附件列表中的第三个附件
	colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	//子流程创建
	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;// 声明为图形渲染子流程
	subpass.colorAttachmentCount = 1;// 使用 1 个颜色附件	
	subpass.pColorAttachments = &colorAttachmentRef;// 指向颜色附件引用
	subpass.pDepthStencilAttachment = &depthAttachmentRef;//指向深度附件引用
	subpass.pResolveAttachments = &colorAttachmentResolveRef;//指向多重采样附件引用

	//子流程依赖
	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;//源
	dependency.dstSubpass = 0;//目标
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
		VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;//前一个操作在图像呈现阶段结束
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
		VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;//目标阶段是写入颜色附件和做早期深度测试的阶段
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
		VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;//写颜色附件和深度缓冲

	//创建渲染流程
	VkRenderPassCreateInfo renderPassInfo{};
	std::array<VkAttachmentDescription, 3> attachments =
	{ colorAttachment,
		depthAttachment,
			colorAttachmentResolve };
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());// 主流程的附件数量（此处只有颜色附件）
	renderPassInfo.pAttachments = attachments.data();// 主流程附件列表
	renderPassInfo.subpassCount = 1;// 子流程数量
	renderPassInfo.pSubpasses = &subpass;// 子流程配置
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	//创建渲染流程
	if (vkCreateRenderPass(m_context.deviceContext->getLogicalDevice(),
		&renderPassInfo, 
		nullptr,
		&m_renderPass) != VK_SUCCESS) {
		throw std::runtime_error("failed to create render pass!");
	}


}
