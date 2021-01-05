/*
 * ShaderTestLayer.cpp
 *
 *  Created on: Jan 3, 2021
 *      Author: exuvo
 */

#include <array>
#include <vector>
#include <stdexcept>
#include <vector>
#include <cstring>
#include <set>
#include <vulkan/vulkan.h>

#include <VK2D.h>

#include "Aurora.hpp"
#include "ShaderTestLayer.hpp"
#include "utils/Math.hpp"
#include "ui/RenderCache.hpp"

struct Vertex {
	Vector2f pos = {};
	vk2d::Colorf color = {};

	static VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription { };
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		
		return bindingDescription;
	}
	
	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions { };
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, pos);
		
		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);
		
		return attributeDescriptions;
	}
};

struct UniformBufferObject {
	alignas(16) Eigen::Matrix4f model; // 64 bytes
	alignas(16) Eigen::Matrix4f view;  // 64 bytes
	alignas(16) Eigen::Matrix4f proj;  // 64 bytes
};

const std::vector<Vertex> vertices = {
	{{160.0f, 20.0f}, {1.0f, 0.0f, 0.0f}},
	{{190.0f, 20.0f}, {0.0f, 1.0f, 0.0f}},
	{{190.0f, 50.1f}, {0.0f, 0.0f, 1.0f}},
	{{160.0f, 50.1f}, {1.0f, 1.0f, 1.0f}}};

const std::vector<uint32_t> indices = { 0, 1, 2, 2, 3, 0 };

ShaderTestLayer::ShaderTestLayer(AuroraWindow& parentWindow): UILayer(parentWindow) {
	createVulkanIndexBuffer();
	createVulkanVertexBuffer();
	createGraphicsPipeline();
}

void ShaderTestLayer::eventResized() {
	cleanupResizing();
	createGraphicsPipeline();
}

ShaderTestLayer::~ShaderTestLayer() {
	cleanupResizing();
	
	vkDestroyBuffer(window.vk_device, indexBuffer, nullptr);
	vkFreeMemory(window.vk_device, indexBufferMemory, nullptr);
	
	vkDestroyBuffer(window.vk_device, vertexBuffer, nullptr);
	vkFreeMemory(window.vk_device, vertexBufferMemory, nullptr);
}

void ShaderTestLayer::cleanupResizing() {
	vkDestroyPipeline(window.vk_device, graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(window.vk_device, pipelineLayout, nullptr);
}

void ShaderTestLayer::render() {
	VkCommandBuffer commandBuffer = window.vk_command_buffer;
	
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
	
	VkBuffer vertex_buffers[1] = { vertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertex_buffers, offsets);
	vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
	
//	VkViewport viewport {};
//	viewport.x = 0;
//	viewport.y = 0;
//	viewport.width  = (float) window.vk_extent.width;
//	viewport.height = (float) window.vk_extent.height;
//	viewport.minDepth	= 0.0f;
//	viewport.maxDepth	= 1.0f;
//	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
	
//	VkRect2D scissor {
//		{ 0, 0 },
//		window.vk_extent
//	};
//	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	
	{
		float scale[2];
		scale[0] = 2.0f / window.vk_extent.width;
		scale[1] = 2.0f / window.vk_extent.height;
		float translate[2];
		translate[0] = -1.0f;
		translate[1] = -1.0f;
		vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, sizeof(float) * 0, sizeof(float) * 2, scale);
		vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, sizeof(float) * 2, sizeof(float) * 2, translate);
	}
	
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
	
	window.restore_VK2D_render(false, false);
	
	// Restore VK2D render pipeline
//	window.window->impl->previous_pipeline_settings = vk2d::_internal::GraphicsPipelineSettings {};
//	window.window->impl->mesh_buffer->bound_vertex_buffer_block = nullptr;
//	window.window->impl->mesh_buffer->bound_index_buffer_block = nullptr;
}

//Helper functions!

uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice physicalDevice) {
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
	
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}
	
	throw std::runtime_error("failed to find suitable memory type!");
}

void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer,
                  VkDeviceMemory& bufferMemory, VkDevice device, VkPhysicalDevice physicalDevice) {
	VkBufferCreateInfo bufferInfo { };
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	
	if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create vertex buffer!");
	}
	
	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, buffer, &memRequirements);
	
	VkMemoryAllocateInfo allocInfo { };
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties, physicalDevice);
	
	if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate vertex buffer memory!");
	}
	
	vkBindBufferMemory(device, buffer, bufferMemory, 0);
}

//Private
void ShaderTestLayer::createVulkanVertexBuffer() {
	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
	
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
	             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vertexBuffer,
	             vertexBufferMemory, window.vk_device, window.vk_physical_device);
	
	void* data;
	vkMapMemory(window.vk_device, vertexBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, vertices.data(), (size_t) bufferSize);
	vkUnmapMemory(window.vk_device, vertexBufferMemory);
}

void ShaderTestLayer::createVulkanIndexBuffer() {
	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();
	
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
	             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, indexBuffer,
	             indexBufferMemory, window.vk_device, window.vk_physical_device);
	
	void* data;
	vkMapMemory(window.vk_device, indexBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices.data(), (size_t) bufferSize);
	vkUnmapMemory(window.vk_device, indexBufferMemory);
}

void ShaderTestLayer::createGraphicsPipeline() {
	vk2d::_internal::GraphicsShaderProgram& shader = RenderCache::getShader("shader"_hs);
	
	VkPipelineShaderStageCreateInfo vertShaderStageInfo { };
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = shader.vertex;
	vertShaderStageInfo.pName = "main";
	
	VkPipelineShaderStageCreateInfo fragShaderStageInfo { };
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = shader.fragment;
	fragShaderStageInfo.pName = "main";
	
	VkPipelineVertexInputStateCreateInfo vertexInputInfo { };
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	
	auto bindingDescription = Vertex::getBindingDescription();
	auto attributeDescriptions = Vertex::getAttributeDescriptions();
	
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
	
	VkPipelineInputAssemblyStateCreateInfo inputAssembly { };
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;
	
	VkViewport viewport { };
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float) window.vk_extent.width;
	viewport.height = (float) window.vk_extent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	
	VkRect2D scissor { };
	scissor.offset = { 0, 0 };
	scissor.extent = window.vk_extent;
	
	VkPipelineViewportStateCreateInfo viewportState { };
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.pScissors = &scissor;
	
//	VkDynamicState dynamic_states[2] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
//	VkPipelineDynamicStateCreateInfo dynamic_state = { };
//	dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
//	dynamic_state.dynamicStateCount = (uint32_t) ARRAY_LEN(dynamic_states);
//	dynamic_state.pDynamicStates = dynamic_states;
	
	VkPipelineRasterizationStateCreateInfo rasterizer { };
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	
	VkPipelineMultisampleStateCreateInfo multisampling { };
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	
	VkPipelineColorBlendAttachmentState colorBlendAttachment { };
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT
	                                      | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;
	
	VkPipelineColorBlendStateCreateInfo colorBlending { };
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;
	
	// Constants: we are using 'vec2 offset' and 'vec2 scale' instead of a full 3d projection matrix
	VkPushConstantRange push_constants[1] = {};
	push_constants[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	push_constants[0].offset = sizeof(float) * 0;
	push_constants[0].size = sizeof(float) * 4;
	
	VkPipelineLayoutCreateInfo pipelineLayoutInfo { };
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0;
	pipelineLayoutInfo.pushConstantRangeCount = 1;
	pipelineLayoutInfo.pPushConstantRanges = push_constants;
	
	if (vkCreatePipelineLayout(window.vk_device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}
	
	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };
	
	VkGraphicsPipelineCreateInfo pipelineInfo { };
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = window.vk_render_pass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
//	pipelineInfo.pDynamicState = &dynamic_state;
	
	if (vkCreateGraphicsPipelines(window.vk_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline!");
	}
}


