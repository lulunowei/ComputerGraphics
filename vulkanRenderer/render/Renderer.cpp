#include "Renderer.h"
#include"../utils/VkFormatUtils.h"
#include"../const/LoadModels.h"
#include"../init/AllHeads.h"

Renderer::Renderer(ApplicationContext& context):
	m_context(context)
{
	init();
}

void Renderer::init()
{
	auto physicalDevice = m_context.deviceContext->getPhysicalDevice();
	auto logicalDevice=m_context.deviceContext->getLogicalDevice();
	auto msaaSamples = m_context.deviceContext->getMsaaSamples();

	m_swapchainManager = std::make_shared<VulkanSwapChainManager>(m_context);//初始化交换链
	auto colorFormat = m_swapchainManager->getSwapChainImageFormat();//获取颜色格式
	auto depthFormt = VkFormatUtils::
		findDepthFormat(physicalDevice);//获取深度格式
	auto swapchainImageviews = m_swapchainManager->getSwapChainImageViews();//获取交换链图像视图
	auto swapChainExtent = m_swapchainManager->getSwapChainExtent();//获取交换链分辨率

	//初始化渲染流程
	m_renderpassManager = std::make_shared<VulkanRenderPassManager>(
		m_context,
		colorFormat,
		depthFormt,
		msaaSamples);
	auto renderpass = m_renderpassManager->getRenderPass();//获取渲染流程
	//初始化m_SourceContext
	m_SourceContext = std::make_unique<SourceContext>();

	//初始化布局资源
	m_SourceContext->descriptorResourceContext =
		std::make_shared<DescriptorSource>();
	m_SourceContext->descriptorResourceContext->initDescriptorSetLayout(logicalDevice);//创建描述符布局
	auto descriptorSetLayout = m_SourceContext->descriptorResourceContext->getDescriptorSetLayout();//获取描述符布局

	GraphicPipeLineCreateInfo graphicPipeLineInfo{ 
		logicalDevice,
		msaaSamples,
		descriptorSetLayout,
		renderpass
	};
	//初始化图形管道
	m_SourceContext->graphicsPipelineResourceContext =
		std::make_shared<GraphicsPipelineSource>();
	m_SourceContext->graphicsPipelineResourceContext->initGraphicsPipeline(graphicPipeLineInfo);
	auto graphicsPipeline=m_SourceContext->graphicsPipelineResourceContext->getGraphicsPipeline();//获取图形管道

	//初始化命令池
	m_commandManager =
		std::make_shared<VulkanCommandManager>(m_context);
	auto commandpool=m_commandManager->getCommandPool();//获取命令池
	auto commandbuffers = m_commandManager->getCommandBuffers();//获取命令缓存

	//初始化深度信息
	m_SourceContext->depthResourceContext =
		std::make_shared<DepthResource>(
			m_context,
			*m_commandManager,
			*m_swapchainManager);
	auto depthImageView = m_SourceContext->depthResourceContext->getDepthImageView();//获取深度视图

	//初始化图形资源
	m_SourceContext->colorResourceContext =
		std::make_shared<ColorResource>(
			m_context,
			*m_commandManager,
			*m_swapchainManager);
	auto colorImageView = m_SourceContext->colorResourceContext->getColorImageView();//获取多重采样图像视图

	//获取创建帧缓存参数
	std::array<VkImageView, 3> attachments = {
		colorImageView,
		depthImageView,
		swapchainImageviews[0] };
	FramebufferCreateInfo framebufferInfo{
		swapchainImageviews,//交换链图像视图
		attachments,//附件
		renderpass,//渲染流程对象
		swapChainExtent,//交换链图像分辨率
		logicalDevice
	};
	//初始化帧缓存
	m_SourceContext->frameBufferResourceContext =
		std::make_shared<FrameBufferSource>(framebufferInfo);

	//初始化纹理资源
	m_SourceContext->textureResourceContext =
		std::make_shared<TextureResource>(m_context,*m_commandManager);
	auto textureImageView = m_SourceContext->textureResourceContext->getTextureImageView();//获取纹理视图
	auto textureSampler = m_SourceContext->textureResourceContext->getTextureSampler();//获取纹理采样器


	//初始化顶点资源
	m_SourceContext->vertexResourceContext =
		std::make_shared<VertexResource>(m_context, *m_commandManager);

	//初始化统一资源
	m_SourceContext->UBOResourceContext =
		std::make_shared<UBOResource>(m_context, *m_commandManager);
	auto UBO = m_SourceContext->UBOResourceContext->getUniformBuffers();

	DescriptorSetsCreateInfo descriptorSetsInfo{
		logicalDevice,
		UBO,
		textureSampler,
		textureImageView
	};
	//初始化描述符集
	m_SourceContext->descriptorResourceContext->initDescriptorSets(descriptorSetsInfo);

	//创建信号量
	m_syncObjectManager =
		std::make_shared<VulkanSyncObjectsManager>(m_context,logicalDevice);
}

void Renderer::draw()
{
	drawFrame();
}

void Renderer::rendererCleanup()
{
	cleanupSwapChain();//释放交换链
	//销毁图形管道和布局
	vkDestroyPipeline(m_context.deviceContext->getLogicalDevice(),
		m_SourceContext->graphicsPipelineResourceContext->getGraphicsPipeline(),
		nullptr);
	vkDestroyPipelineLayout(m_context.deviceContext->getLogicalDevice(),
		m_SourceContext->graphicsPipelineResourceContext->getPipelineLayout(),
		nullptr);
	//销毁renderpass
	m_renderpassManager->renderpassCleanup();
	//销毁UBO资源
	m_SourceContext->UBOResourceContext->UBOCleanup();

	//销毁描述符池
	vkDestroyDescriptorPool(m_context.deviceContext->getLogicalDevice(),
		m_SourceContext->descriptorResourceContext->getDescriptorPool(), nullptr);
	//销毁纹理资源和采样器
	m_SourceContext->textureResourceContext->textureCleanup();

	//销毁描述符布局
	vkDestroyDescriptorSetLayout(m_context.deviceContext->getLogicalDevice(),
		m_SourceContext->descriptorResourceContext->getDescriptorSetLayout(),
		nullptr);
	//销毁顶点及索引
	m_SourceContext->vertexResourceContext->VertexCleanup();
	//销毁信号量
	m_syncObjectManager->syncObjectsCleanup();
	//销毁命令池
	m_commandManager->commandCleanup();
}

/**
 * @descrip 绘制一帧
 * 
 * @functionName:  drawFrame
 * @functionType:    void
 */
void Renderer::drawFrame()
{
	auto device = m_context.deviceContext->getLogicalDevice();
	auto inFlightFences = m_syncObjectManager->getinFlightFences();
	auto imageAvailableSemaphores = m_syncObjectManager->getImageAvailableSemaphores();
	auto renderFinishedSemaphores = m_syncObjectManager->getRenderFinishedSemaphores();

	auto swapChain = m_swapchainManager->getSwapChain();
	//1.等待前一帧，等待fence触发
	vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);//cpu等待gpu画完
	//vkResetFences(device, 1, &inFlightFences[currentFrame]);//重置信号量
	//2.获取交换区图像
	uint32_t imageIndex;//保存交换区图像索引

	//表示获取图像是否成功
	VkResult result = vkAcquireNextImageKHR
	(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

	//如果获取图像失败，重新创建交换链
	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		recreateSwapChain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	updateUniformBuffer(currentFrame);//更新全局数据

	// Only reset the fence if we are submitting work,延迟重置,仅在确认提交任务前重置栅栏，防止重新创建交换链的时候直接返回，导致fence永远不能重置
	vkResetFences(device, 1, &inFlightFences[currentFrame]);//重置栅栏
	//3.提交到命令缓冲区
	vkResetCommandBuffer(m_commandManager->getCommandBuffers()[currentFrame], 0);//重置命令缓冲区
	recordCommandBuffer(m_commandManager->getCommandBuffers()[currentFrame], imageIndex);//录制新的命令缓冲区
	//4.提交到命令缓冲区到队列 
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };//执行前等待的获取的图像信号
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };//图像可用信号量触发，且到达颜色附件输出阶段时，命令才真正开始执行。
	submitInfo.waitSemaphoreCount = 1;

	//信号量与pWaitDstStageMask中相同的索引关系相对应
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;//指明信号阻塞在哪里
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &m_commandManager->getCommandBuffers()[currentFrame];

	// 执行提交的命令缓冲区中的所有操作后发送renderFinishedSemaphore信号
	VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	//5.提交到图形队列，非阻塞操作
	if (vkQueueSubmit(m_context.deviceContext->getGraphicsQueue(), 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	//6.展示到屏幕
	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;//等待渲染任务已完成信号

	VkSwapchainKHR swapChains[] = { swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;

	// 表示图像呈现是否成功
	result = vkQueuePresentKHR(m_context.deviceContext->getPresentQueue(), &presentInfo);

	//显示检查是否需要重新创建交换链
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_context.windowContext->getFramebufferResized()) {
		m_context.windowContext->m_framebufferResized = false;
		recreateSwapChain();
	}
	else if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to present swap chain image!");
	}

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

}


/**
 * @descrip 重新绘制交换链
 * 
 * @functionName:  recreateSwapChain
 * @functionType:    void
 */
void Renderer::recreateSwapChain()
{
	int width = 0, height = 0;
	glfwGetFramebufferSize(m_context.windowContext->getWindow(), &width, &height);// 获取窗口的帧缓冲区
	while (width == 0 || height == 0) {//如果宽度或高度为0（窗口最小化或尺寸异常），进入等待状态。
		glfwGetFramebufferSize(m_context.windowContext->getWindow(), &width, &height);
		glfwWaitEvents();//阻塞线程，等待窗口尺寸变化
	}

	vkDeviceWaitIdle(m_context.deviceContext->getLogicalDevice());//等待设备处理完空闲之后

	//cleanupSwapChain();

	m_swapchainManager->createSwapChain();
	m_swapchainManager->createImageViews();
	m_SourceContext->colorResourceContext->createColorResources();//调整多重采样缓冲区
	m_SourceContext->depthResourceContext->createDepthResources();//调整深度缓冲区

	//获取创建帧缓存参数
	std::array<VkImageView, 3> attachments = {
		m_SourceContext->colorResourceContext->getColorImageView(),
		m_SourceContext->depthResourceContext->getDepthImageView(),
		(m_swapchainManager->getSwapChainImageViews())[0] };
	FramebufferCreateInfo framebufferInfo{
		m_swapchainManager->getSwapChainImageViews(),//交换链图像视图
		attachments,//附件
		m_renderpassManager->getRenderPass(),//渲染流程对象
		m_swapchainManager->getSwapChainExtent(),//交换链图像分辨率
		m_context.deviceContext->getLogicalDevice()
	};

	m_SourceContext->frameBufferResourceContext->createFramebuffers(framebufferInfo);
}

/**
 * @descrip 清理交换链
 * 
 * @functionName:  cleanupSwapChain
 * @functionType:    void
 */
void Renderer::cleanupSwapChain()
{
	vkDestroyImageView(
		m_context.deviceContext->getLogicalDevice(),
		m_SourceContext->depthResourceContext->getDepthImageView(),
		nullptr);
	vkDestroyImage(
		m_context.deviceContext->getLogicalDevice(), 
		m_SourceContext->depthResourceContext->getDepthImage(),
		nullptr);
	vkFreeMemory(
		m_context.deviceContext->getLogicalDevice(),
		m_SourceContext->depthResourceContext->getDepthImageMemory(),
		nullptr);

	vkDestroyImageView(
		m_context.deviceContext->getLogicalDevice(),
		m_SourceContext->colorResourceContext->getColorImageView(),
		nullptr);
	vkDestroyImage(m_context.deviceContext->getLogicalDevice(), 
		m_SourceContext->colorResourceContext->getColorImage(),
		nullptr);
	vkFreeMemory(m_context.deviceContext->getLogicalDevice(), 
		m_SourceContext->colorResourceContext->getcolorImageMemory(),
		nullptr);

	for (size_t i = 0; i < m_SourceContext->frameBufferResourceContext->getSwapChainFramebuffers()
		.size(); i++) {
		vkDestroyFramebuffer(m_context.deviceContext->getLogicalDevice(),
			m_SourceContext->frameBufferResourceContext->getSwapChainFramebuffers()[i],
			nullptr);
	}
	for (size_t i = 0; i < m_swapchainManager->getSwapChainImageViews().size(); i++) {
		vkDestroyImageView(m_context.deviceContext->getLogicalDevice(), m_swapchainManager->getSwapChainImageViews()[i], nullptr);
	}

	vkDestroySwapchainKHR(m_context.deviceContext->getLogicalDevice(), m_swapchainManager->getSwapChain(), nullptr);
}

/**
 * @descrip 更新全局变量
 * 
 * @functionName:  updateUniformBuffer
 * @functionType:    void
 * @param currentImage
 */
void Renderer::updateUniformBuffer(uint32_t currentImage) {
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	//以浮点精度计算自渲染开始以来的秒数
	float time = std::chrono::duration<float, std::chrono::seconds::period>
		(currentTime - startTime).count();

	UniformBufferObject ubo{};


	ubo.model = glm::rotate(glm::mat4(1.0f),//单位矩阵
		time * glm::radians(90.0f),//每秒旋转90°
		glm::vec3(0.0f, 1.0f, 0.0f));

	//从上方以 45 度角观察几何形状
	//ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f),
	//	glm::vec3(0.0f, 0.0f, 0.0f), 
	//	glm::vec3(0.0f, 0.0f, 1.0f));

	ubo.view = glm::lookAt(
		//glm::vec3(2.0f, 50.0f, 2.0f),   // 相机位置：y=100，在模型正上方
		glm::vec3(2.0f, 2.0f, 2.0f),   // 相机位置：y=100，在模型正上方
		glm::vec3(0.0f, 0.0f, 0.0f),     // 看向原点
		glm::vec3(0.0f, 0.0f, 1.0f));   // 向下看（Z轴作为上方向）

	// 45 度垂直视场角的透视投影
	ubo.proj = glm::perspective(glm::radians(45.0f),
		m_swapchainManager->getSwapChainExtent().width / (float)m_swapchainManager->getSwapChainExtent().height,
		1.0f,
		500.0f);

	ubo.time = time;//更新时间

	//翻转 Y 轴缩放因子，与OPENGL相反
	ubo.proj[1][1] *= -1;
	//复制数据到统一缓冲区，uniformBuffersMapped指针指向该内存
	memcpy(m_SourceContext->UBOResourceContext->getUniformBuffersMapped()[currentImage], &ubo, sizeof(ubo));

}

/**
 * @descrip 录制绘制命令
 * 
 * @functionName:  recordCommandBuffer
 * @functionType:    void
 * @param commandBuffer
 * @param imageIndex
 */
void Renderer::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
	//开始录制命令
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0; // Optional
	beginInfo.pInheritanceInfo = nullptr; // Optional

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("failed to begin recording command buffer!");
	}
	//2.设置渲染通道开始信息
	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = m_renderpassManager->getRenderPass();
	renderPassInfo.framebuffer = m_SourceContext->frameBufferResourceContext->getSwapChainFramebuffers()[imageIndex];
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = m_swapchainManager->getSwapChainExtent();

	//VkClearValue clearColor = { {{1.0f, 1.0f, 1.0f, 1.0f}} };
	std::array<VkClearValue, 2> clearValues{};//设置每帧的清除值
	clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };//清除颜色附件
	clearValues[1].depthStencil = { 1.0f, 0 };//清除深度和模板附件
	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());;
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);//启动渲染通道

	vkCmdBindPipeline(commandBuffer, 
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		m_SourceContext->graphicsPipelineResourceContext->getGraphicsPipeline()
	);//绑定图形管线

	//动态设置视口和裁剪矩形
	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(m_swapchainManager->getSwapChainExtent().width);
	viewport.height = static_cast<float>(m_swapchainManager->getSwapChainExtent().height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = m_swapchainManager->getSwapChainExtent();
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	VkBuffer vertexBuffers[] = { m_SourceContext->vertexResourceContext->getVertexBuffer()};
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);//绑定顶点缓冲

	vkCmdBindIndexBuffer(commandBuffer, m_SourceContext->vertexResourceContext->getIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);//绑定索引缓冲
	//使用描述符集，每帧的描述符集绑定到对应着色器
	vkCmdBindDescriptorSets(commandBuffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,//绑定到图形管线
		m_SourceContext->graphicsPipelineResourceContext->getPipelineLayout(),
		0,//set编号：0
		1,//一次绑定一个set
		&m_SourceContext->descriptorResourceContext->getDescriptorSets()[currentFrame],//绑定的descriptor set
		0,
		nullptr);

	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(m_SourceContext->vertexResourceContext->getIndices().size()), 1, 0, 0, 0);

	vkCmdEndRenderPass(commandBuffer);//结束渲染通道

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to record command buffer!");
	}

}


