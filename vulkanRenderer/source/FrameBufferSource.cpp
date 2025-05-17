#include "FrameBufferSource.h"
#include <stdexcept>

/**
 * @descrip 创建帧缓存
 * 
 * @functionName:  createFramebuffers
 * @functionType:    void
 * @param frameBufferCreateInfo
 */
void FrameBufferSource::createFramebuffers(const FramebufferCreateInfo& frameBufferCreateInfo)
{
	swapChainFramebuffers.resize(frameBufferCreateInfo.swapChainImageViews.size());//初始化大小与交换区图像的数量一致

	//遍历交换区视图
	for (size_t i = 0; i < frameBufferCreateInfo.swapChainImageViews.size(); i++) {
		//图像视图和深度视图，以及多重采样视图
		//std::array<VkImageView, 3> attachments = {
		//	colorImageView,//多重采样图像视图
		//	depthImageView,
		//	swapChainImageViews[i]
		//};
		std::array<VkImageView, 3> attachments = frameBufferCreateInfo.attachments;
		attachments[2] = frameBufferCreateInfo.swapChainImageViews[i];

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = frameBufferCreateInfo.renderPass;//一个framebuffer对应一个renderpass
		framebufferInfo.attachmentCount = static_cast<uint32_t>(frameBufferCreateInfo.attachments.size());
		framebufferInfo.pAttachments = frameBufferCreateInfo.attachments.data();
		framebufferInfo.width = frameBufferCreateInfo.swapChainExtent.width;
		framebufferInfo.height = frameBufferCreateInfo.swapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(frameBufferCreateInfo.device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}
	}

}
