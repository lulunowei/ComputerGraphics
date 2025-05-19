#pragma once
#include<memory>
class VulkanDevice;
class VulkanInstance;
class Window;

class ColorResource;
class DepthResource;
class VertexResource;
class TextureResource;
class UBOResource;
class DescriptorSource;
class FrameBufferSource;
class GraphicsPipelineSource;

struct ApplicationContext {
	std::shared_ptr<Window> windowContext;
	std::shared_ptr<VulkanInstance> instanceContext;
	std::shared_ptr<VulkanDevice> deviceContext;
};

struct SourceContext {
	std::shared_ptr<ColorResource> colorResourceContext;
	std::shared_ptr<DepthResource> depthResourceContext;
	std::shared_ptr<VertexResource> vertexResourceContext;
	std::shared_ptr<TextureResource> textureResourceContext;
	std::shared_ptr<UBOResource> UBOResourceContext;
	std::shared_ptr<DescriptorSource> descriptorResourceContext;
	std::shared_ptr<FrameBufferSource> frameBufferResourceContext;
	std::shared_ptr<GraphicsPipelineSource> graphicsPipelineResourceContext;
};

