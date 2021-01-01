/*
 * RenderUtils.cpp
 *
 *  Created on: Dec 18, 2020
 *      Author: exuvo
 */

#include <math.h>

#include <VK2D.h>
#include <Interface/InstanceImpl.h>
#include <Interface/WindowImpl.h>

#include "RenderUtils.hpp"

vk2d::Vector2f vectorToVK2D(Vector2l vec) {
	return {vec.x(), vec.y()};
}

vk2d::Vector2f vectorToVK2D(Vector2i vec) {
	return {vec.x(), vec.y()};
}

Vector2i vk2dToVector(vk2d::Vector2i vec) {
	return {vec.x, vec.y};
}

Vector2d vk2dToVector(vk2d::Vector2d vec) {
	return {vec.x, vec.y};
}

vk2d::Rect2f matrixToVK2D(Matrix2i mat) {
	return { mat(0,0), mat(0,1), mat(1,0), mat(1,1) };
}

float linearRGBtoSRGB(float color) {
	if (color <= 0.0031308) {
		return color * 12.92f;
	}
	return (1.055 * pow(color, 1 / 2.4) - 0.055);
}

float sRGBtoLinearRGB(float color){
	if (color <= 0.04045) {
		return color / 12.92f;
	}
	return pow((color + 0.055) / 1.055, 2.4);
}

vk2d::Colorf linearRGBtoSRGB(vk2d::Colorf c) {
	return { linearRGBtoSRGB(c.r), linearRGBtoSRGB(c.g), linearRGBtoSRGB(c.b), c.a };
}

vk2d::Colorf sRGBtoLinearRGB(vk2d::Colorf c) {
	return { sRGBtoLinearRGB(c.r), sRGBtoLinearRGB(c.g), sRGBtoLinearRGB(c.b), c.a };
}

void resetVK2dRenderState(vk2d::Window* window) {
	window->impl->previous_pipeline_settings = vk2d::_internal::GraphicsPipelineSettings {};
	
	window->impl->mesh_buffer->bound_index_buffer_block = nullptr;
	window->impl->mesh_buffer->bound_vertex_buffer_block = nullptr;
	window->impl->mesh_buffer->bound_transformation_buffer_block = nullptr;
	window->impl->mesh_buffer->bound_texture_channel_weight_buffer_block = nullptr;
	
	VkCommandBuffer command_buffer = window->impl->vk_render_command_buffers[window->impl->next_image];
	
	VkViewport viewport {};
	viewport.x			= 0;
	viewport.y			= 0;
	viewport.width		= float( window->impl->extent.width );
	viewport.height		= float( window->impl->extent.height );
	viewport.minDepth	= 0.0f;
	viewport.maxDepth	= 1.0f;
	vkCmdSetViewport(
		command_buffer,
		0, 1, &viewport
	);
	
	VkRect2D scissor {
		{ 0, 0 },
		window->impl->extent
	};
	vkCmdSetScissor(
		command_buffer,
		0, 1, &scissor
	);
	
	// Window frame data.
	vkCmdBindDescriptorSets(
		command_buffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		window->impl->instance->GetGraphicsPrimaryRenderPipelineLayout(),
		vk2d::_internal::GRAPHICS_DESCRIPTOR_SET_ALLOCATION_WINDOW_FRAME_DATA,
		1, &window->impl->frame_data_descriptor_set.descriptorSet,
		0, nullptr
	);
}
