/*
 * ShaderTestLayer.hpp
 *
 *  Created on: Jan 3, 2021
 *      Author: exuvo
 */

#ifndef SRC_UI_SHADERTESTLAYER_HPP_
#define SRC_UI_SHADERTESTLAYER_HPP_

#include <vulkan/vulkan.h>

#include "ui/UILayer.hpp"

class ShaderTestLayer: public UILayer {
	public:
		ShaderTestLayer(AuroraWindow& parentWindow);
		virtual ~ShaderTestLayer() override;
		
		virtual void render() override;
		virtual void eventResized() override;
		
	private:
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
};

#endif /* SRC_UI_SHADERTESTLAYER_HPP_ */
