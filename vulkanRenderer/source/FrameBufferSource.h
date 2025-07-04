#pragma once
#include<vulkan/vulkan.h>
#include<array>
#include<vector>

//创建framebuffer的参数传递
struct FramebufferCreateInfo {
	std::vector<VkImageView> swapChainImageViews;
	std::array<VkImageView, 3> attachments{};
	VkRenderPass                renderPass= VK_NULL_HANDLE;
	VkExtent2D swapChainExtent{};
	VkDevice device= VK_NULL_HANDLE;
};

class FrameBufferSource
{
public:
	FrameBufferSource(const FramebufferCreateInfo& frameBufferCreateInfo);
	~FrameBufferSource() = default;
	void createFramebuffers(const FramebufferCreateInfo &frameBufferCreateInfo);
	std::vector<VkFramebuffer> &getSwapChainFramebuffers() { return m_swapChainFramebuffers; }
private:
	std::vector<VkFramebuffer> m_swapChainFramebuffers;
};

