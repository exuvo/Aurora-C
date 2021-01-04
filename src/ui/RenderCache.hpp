/*
 * RenderCache.hpp
 *
 *  Created on: Dec 31, 2020
 *      Author: exuvo
 */

#ifndef SRC_UI_RENDERCACHE_HPP_
#define SRC_UI_RENDERCACHE_HPP_

#include <unordered_map>
#include <filesystem>

#include <VK2D.h>
#include <System/ShaderInterface.h>

#include <entt/entt.hpp>
#include "log4cxx/logger.h"

using namespace log4cxx;

class RenderCache {
public:
	
	static vk2d::Mesh& getTextMesh(vk2d::FontResource* font,vk2d::Vector2f position, std::string text,
	                       vk2d::Colorf color = vk2d::Colorf::WHITE());
	// Use stackDepth=1 if calling from a helper function
	static vk2d::Mesh& getTextMeshCallerCentric(vk2d::FontResource* font, vk2d::Vector2f position, std::string text,
	                                            vk2d::Colorf color = vk2d::Colorf::WHITE(), uint8_t stackDepth = 0);
	
	static vk2d::_internal::GraphicsShaderProgram& getShader(entt::hashed_string path);
	
	static void clear();
	
private:
	static inline LoggerPtr log = Logger::getLogger("aurora.render.cache");
	static inline LoggerPtr shaderLog = Logger::getLogger("aurora.render.cache.shader");
	
	static VkShaderModule createShaderModule(std::string name);
	
	static vk2d::Mesh& getTextMesh(size_t hash, vk2d::FontResource* font,vk2d::Vector2f position, std::string text,
	                       vk2d::Colorf color = vk2d::Colorf::WHITE());
	
	struct noop_hasher {
		size_t operator()(const size_t& s) const { return s; };
	};
	
	struct TextCacheEntry {
		std::string text;
		vk2d::Vector2f position;
		vk2d::Mesh mesh;
		
		bool operator==(const TextCacheEntry& o) const {
			return text == o.text;
		}
	};
	
	static inline std::unordered_map<size_t, TextCacheEntry, noop_hasher> textCache;
	static inline std::unordered_map<entt::hashed_string::hash_type, vk2d::_internal::GraphicsShaderProgram> shaderCache;
};

#endif /* SRC_UI_RENDERCACHE_HPP_ */
