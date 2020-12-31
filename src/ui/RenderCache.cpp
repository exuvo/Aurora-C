/*
 * RenderCache.cpp
 *
 *  Created on: Dec 31, 2020
 *      Author: exuvo
 */

#include <entt/entt.hpp>
#include <boost/functional/hash.hpp>
#include <functional>

#include "RenderCache.hpp"

vk2d::Mesh& RenderCache::getTextMesh(size_t hash, vk2d::FontResource* font, vk2d::Vector2f position, std::string text,
                                    vk2d::Colorf color) {
	auto found = textCache.find(hash);
	
	TextCacheEntry* cache = nullptr;
	
	if (found != textCache.end()) {
		TextCacheEntry* val = &found->second;
//		std::cout << "maybe cached text mesh" << val->text << std::endl;
		if (val->text == text) {
//			std::cout << "found cached text mesh" << val->text << std::endl;
			cache = val;
		}
	}
	
	if (cache == nullptr) {
//		std::cout << "creating cached text mesh \"" << text << "\" " << position << std::endl;
		TextCacheEntry val = { text, position, vk2d::GenerateTextMesh(font, position, text) };
		cache = &(textCache[hash] = val);
	}
	
	if (cache->mesh.vertices[0].color != color) {
//		std::cout << "recoloring cached text mesh \"" << text << "\" " << cache->mesh.vertices[0].color << " to " << color << std::endl;
		cache->mesh.SetVertexColor(color);
	}
	
	if (cache->position != position) {
//		std::cout << "translating cached text mesh \"" << text << "\" " << cache->position << " to " << position << std::endl;
		cache->mesh.Translate(position - cache->position);
		cache->position = position;
//		cache->mesh.RecalculateAABBFromVertices();
	}
	
	return cache->mesh;
}

vk2d::Mesh& RenderCache::getTextMesh(vk2d::FontResource* font, vk2d::Vector2f position, std::string text,
                                    vk2d::Colorf color) {
	size_t hash = std::hash<std::string>{}(text);
//	std::cout << "hash " << hash << std::endl;
	
	return getTextMesh(hash, font, position, text, color);
}

vk2d::Mesh& RenderCache::getTextMeshCallerCentric(vk2d::FontResource* font, vk2d::Vector2f position, std::string text,
                                                  vk2d::Colorf color, uint8_t stackDepth) {
	
	uintptr_t retAddr;
	
	if (stackDepth == 0) {
		retAddr = (uintptr_t) __builtin_return_address(0);
		
	} else if (stackDepth == 1) {
		retAddr = (uintptr_t) __builtin_return_address(1);
		
	} else {
		throw std::invalid_argument("stackDepth > 1");
	}
	
	size_t hash = 13 * retAddr;
//	std::cout << "hash " << hash << std::endl;
	
	return getTextMesh(hash, font, position, text, color);
}
