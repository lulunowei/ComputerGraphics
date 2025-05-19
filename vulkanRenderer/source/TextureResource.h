#pragma once
#include "IVulkanResourceManager.h"
class TextureResource :
    public IVulkanResourceManager
{
public:
    TextureResource(ApplicationContext& context, VulkanCommandManager& cmdManager);
    ~TextureResource() = default;
    VkImageView &getTextureImageView() { return m_textureImageView; }//图像视图
    VkSampler &getTextureSampler() { return m_textureSampler; }//采样器
    uint32_t &getMipLevels() { return m_mipLevels; }//mipmap层数
    void textureCleanup();
private:
    void createTextureImage();//创建纹理图
    void createTextureImageView();//创建纹理视图
    void createTextureSampler();//创建纹理采样器
    void generateMipmaps(VkImage image,
        VkFormat imageFormat,
        int32_t texWidth,
        int32_t texHeight,
        int32_t mipLevels);//生成mipmaps
    

private:
    uint32_t m_mipLevels;//mipmap层数
    VkImage m_textureImage;//图像缓存
    VkDeviceMemory m_textureImageMemory;//图像缓存内存
    VkImageView m_textureImageView;//图像视图
    VkSampler m_textureSampler;//采样器

    VkBuffer m_stagingBuffer;//暂存缓存
    VkDeviceMemory m_stagingBufferMemory;//暂存缓存内存
};

