#ifndef ShaderAPI_Hpp
#define ShaderAPI_Hpp

#include <vulkan/vulkan.h>
#include <vector>
#include <glm/glm.hpp>
#include <array>

namespace shaderAPI
{
    struct Vertex
    {
        glm::vec2 pos = glm::vec2{};
        glm::vec3 color = glm::vec3{};

        static VkVertexInputBindingDescription getBindingDescription();
        static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();
    };

    struct UniformBufferObject
    {
        alignas(16) glm::mat4 model; // 64 bytes
        alignas(16) glm::mat4 view;  // 64 bytes
        alignas(16) glm::mat4 proj;  // 64 bytes
    };

    const std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}};

    const std::vector<uint32_t> indices = {
        0, 1, 2, 2, 3, 0};





    class ShaderAPI
    {

    public:
        ShaderAPI() = delete;

        ShaderAPI(VkInstance instance, VkDevice logicalDevice, VkPhysicalDevice physicalDevice, VkRenderPass renderPass, VkExtent2D windowSize);

        void draw(VkCommandBuffer commandBuffer);
        void clear();

    private:
        VkInstance instance = VK_NULL_HANDLE;
        VkDevice logicalDevice = VK_NULL_HANDLE;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

        VkRenderPass renderPass = VK_NULL_HANDLE;
        VkExtent2D windowSize = VkExtent2D{};

        VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
        VkPipeline graphicsPipeline = VK_NULL_HANDLE;

        VkBuffer vertexBuffer = VK_NULL_HANDLE;
        VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;
        VkBuffer indexBuffer = VK_NULL_HANDLE;
        VkDeviceMemory indexBufferMemory = VK_NULL_HANDLE;

        void createVulkanVertexBuffer();
        void createVulkanIndexBuffer();

        void createGraphicsPipeline();


        void cleanupResizing();
        void cleanup();
    };
} // namespace shaderAPI

#endif