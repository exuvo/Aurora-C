/*
 * RenderCache.cpp
 *
 *  Created on: Dec 31, 2020
 *      Author: exuvo
 */

#include <boost/functional/hash.hpp>
#include <functional>
#include <fstream>
#include <algorithm>

#include <entt/entt.hpp>
#include <Interface/InstanceImpl.h>
#include <Interface/WindowImpl.h>

#include <glslang/Public/ShaderLang.h>
#include <StandAlone/ResourceLimits.h>	// To get glslang::DefaultTBuiltInResource so we don't have to make one ourselves
#include <SPIRV/Logger.h>
#include <SPIRV/SpvTools.h>
#include <SPIRV/GLSL.std.450.h>
#include <SPIRV/GlslangToSpv.h>

#include "RenderCache.hpp"
#include "Aurora.hpp"


VkShaderModule RenderCache::createShaderModule(std::string name) {
	
	if (!std::filesystem::exists("cache/shaders")) {
		std::filesystem::create_directories("cache/shaders");
	}
	
	std::filesystem::path sourcePath = "assets/shaders/" + name;
	std::filesystem::path compiledPath = "cache/shaders/" + name + ".spv";
	
	if (!std::filesystem::exists(sourcePath)) {
		std::stringstream ss;
		ss << "shader file not found: " << sourcePath;
		LOG4CXX_ERROR(log, ss.str());
		throw std::runtime_error(ss.str());
	}
	
	bool compileShader = true;
	
	if (std::filesystem::exists(compiledPath)) {
		
		auto sourceModified = std::filesystem::last_write_time(sourcePath);
		auto compiledModified = std::filesystem::last_write_time(compiledPath);
		
		if (compiledModified >= sourceModified) {
			compileShader = false;
		}
	}
	
	if (compileShader) {
		size_t size = std::filesystem::file_size(sourcePath);
		char* code = (char*) malloc(size);
		
		std::ifstream inStream {sourcePath.c_str(), std::ios::binary};
		
		if (!inStream.is_open()) {
			std::stringstream ss;
			ss << "unable to open shader source file: " << sourcePath;
			LOG4CXX_ERROR(log, ss.str());
			throw std::runtime_error(ss.str());
		}
		
		inStream.read(code, size);
		inStream.close();
	
		glslang::InitializeProcess();
		
		EShLanguage shaderType;
		
		auto fileExtension = sourcePath.extension();
		
		if (fileExtension == ".vert") {
			shaderType = EShLangVertex;
		} else if (fileExtension == ".frag") {
			shaderType = EShLangFragment;
		} else {
			std::stringstream ss;
			ss << "Unsupported shader type: " << fileExtension;
			throw std::runtime_error(ss.str());
		}
		
		glslang::TShader shader(shaderType);
		
		shader.setStringsWithLengths(&code, (int*) &size, 1);
//		shader.setStringsWithLengthsAndNames(code, (int*) &size, name.data(), 1);
		shader.setEnvInput(glslang::EShSource::EShSourceGlsl, shaderType, glslang::EShClient::EShClientVulkan, 450);
		shader.setEnvClient(glslang::EShClient::EShClientVulkan, glslang::EShTargetClientVersion::EShTargetVulkan_1_2);
		shader.setEnvTarget(glslang::EShTargetLanguage::EShTargetSpv, glslang::EShTargetLanguageVersion::EShTargetSpv_1_5);
		
		bool shaderCompiled = shader.parse(&glslang::DefaultTBuiltInResource, 110, false, EShMessages::EShMsgDefault);
		
		if (shader.getInfoLog() && std::strlen(shader.getInfoLog())) {
			std::string info_log = shader.getInfoLog();
			
			std::vector<std::string> suppressed_warnings { "extension not supported: GL_KHR_vulkan_glsl" };
			
			if (std::none_of(suppressed_warnings.begin(), suppressed_warnings.end(), [&info_log](std::string& s) {
				return info_log.find(s) != std::string::npos;
			})) {
				
				// No suppressed warnings, display it.
				std::cout << "Shader compilation of " << name << ": " << shader.getInfoLog() << std::endl;
				LOG4CXX_INFO(shaderLog, "Shader compilation of " << name << ": " << shader.getInfoLog());
			}
		}
		
		if (shader.getInfoDebugLog() && std::strlen(shader.getInfoDebugLog())) {
			LOG4CXX_DEBUG(shaderLog, "Shader compilation of " << name << ": " << shader.getInfoDebugLog());
		}
		
		if (!shaderCompiled) {
			std::stringstream ss;
			ss << "shader compilation failed: " << name;
			LOG4CXX_ERROR(shaderLog, ss.str());
			throw std::runtime_error(ss.str());
		}
		
		glslang::TProgram program;
		program.addShader(&shader);

		bool shaderLinked = program.link(EShMessages::EShMsgDefault);
		
		if (!program.mapIO()) {
			shaderLinked = false;
		}
		
		if (program.getInfoLog() && std::strlen(program.getInfoLog())) {
			LOG4CXX_INFO(shaderLog, "Shader linking of " << name << ": " << program.getInfoLog());
		}
		
		if (program.getInfoDebugLog() && std::strlen(program.getInfoDebugLog())) {
			LOG4CXX_DEBUG(shaderLog, "Shader linking of " << name << ": " << program.getInfoDebugLog());
		}
		
		if (!shaderLinked) {
			std::stringstream ss;
			ss << "shader linking failed: " << name;
			LOG4CXX_ERROR(shaderLog, ss.str());
			throw std::runtime_error(ss.str());
		}
		
//		program.buildReflection();
//		program.dumpReflection();
		
		std::vector<unsigned int> spirv;
		spv::SpvBuildLogger logger;
		
		glslang::SpvOptions spv_options;
		spv_options.generateDebugInfo = true;
		spv_options.disableOptimizer = false;
		spv_options.optimizeSize = true;
		spv_options.disassemble = false;
		spv_options.validate = false;
		
		glslang::GlslangToSpv(*program.getIntermediate(shaderType), spirv, &logger, &spv_options);
		auto logger_messages = logger.getAllMessages();
		if (logger_messages.size()) {
			LOG4CXX_INFO(shaderLog, "Shader gl to spir-v of " << name << ": " << logger.getAllMessages());
		}
		
		std::ofstream outStream { compiledPath.c_str(), std::ios::binary};
		
		if (!outStream.is_open()) {
			std::stringstream ss;
			ss << "unable to open compiled spir-v file for writing: " << compiledPath << " errno: " << errno << " " << strerror(errno);
			LOG4CXX_ERROR(log, ss.str());
			throw std::runtime_error(ss.str());
		}
		
		for (uint32_t d : spirv) {
			outStream.write((char*) &d, 4);
		}
		
//		outStream.write(spirv.data(), spirv.size());
		outStream.close();
		
		glslang::FinalizeProcess();
	}
	
	size_t size = std::filesystem::file_size(compiledPath);
	uint32_t* spirv = (uint32_t*) malloc(size);
	
	std::ifstream inStream {compiledPath.c_str(), std::ios::binary};
	
	if (!inStream.is_open()) {
		std::stringstream ss;
		ss << "unable to open compiled spir-v file for reading: " << compiledPath << " errno: " << errno << " " << strerror(errno);
		LOG4CXX_ERROR(log, ss.str());
		throw std::runtime_error(ss.str());
	}
	
	inStream.read((char*) spirv, size);
	inStream.close();
	
	VkShaderModuleCreateInfo createInfo {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = size;
	createInfo.pCode = spirv;
	
	VkShaderModule shaderModule {};
	auto ret = vkCreateShaderModule(Aurora.vk2dInstance->impl->GetVulkanDevice(), &createInfo, nullptr, &shaderModule);
	free(spirv);
	
	if (ret != VK_SUCCESS) {
		std::stringstream ss;
		ss << "shader compilation failed for " << name << ": " << ret;
		LOG4CXX_ERROR(log, ss.str());
		throw std::runtime_error(ss.str());
	}
	
	return shaderModule;
}

vk2d::_internal::GraphicsShaderProgram& RenderCache::getShader(entt::hashed_string path) {
	auto found = shaderCache.find(path);
	
	if (found != shaderCache.end()) {
		return found->second;
	}
	
	std::string filename { path.data() };
	
	VkShaderModule vertexShader = createShaderModule(filename + ".vert");
	VkShaderModule fragmentShader = createShaderModule(filename + ".frag");
	
	return (shaderCache[path] = vk2d::_internal::GraphicsShaderProgram(vertexShader, fragmentShader));
}

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

void RenderCache::clear() {
	textCache.clear();
	
	for (auto& pair : shaderCache) {
		vk2d::_internal::GraphicsShaderProgram& shader = pair.second;
		vkDestroyShaderModule(
			Aurora.vk2dInstance->impl->GetVulkanDevice(),
			shader.vertex,
			nullptr
		);
		vkDestroyShaderModule(
			Aurora.vk2dInstance->impl->GetVulkanDevice(),
			shader.fragment,
			nullptr
		);
	}
	shaderCache.clear();
	
}
