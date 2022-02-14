#include "LitModel.h"

#include "LitUtils.h"

// libs
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

// lib headers
#define GLM_ENABLE_EXPERIMENTAL

// std
#include <cassert>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>

namespace std
{
	// special partical template
	template<>
	struct hash<Lit::LitModel::Vertex>
	{
		size_t operator()(Lit::LitModel::Vertex const& vertex) const
		{
			size_t seed = 0;
			//Lit::HashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
			return seed;
		}
	};
}

namespace Lit
{
	std::vector<VkVertexInputBindingDescription> LitModel::Vertex::GetVertexInputBindingDesc()
	{
		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1, VkVertexInputBindingDescription{});
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescriptions;
	}

	std::vector<VkVertexInputAttributeDescription> LitModel::Vertex::GetVertexInputAttributeDesc()
	{
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions(4, VkVertexInputAttributeDescription{});
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, position);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, normal);

		attributeDescriptions[3].binding = 0;
		attributeDescriptions[3].location = 3;
		attributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[3].offset = offsetof(Vertex, uv);

		return attributeDescriptions;
	}

	LitModel::LitModel(LitDevice& inDevice, const Builder& builder):
		device(inDevice)
	{ 
		CreateVertexBuffer(builder.vertices);
		createIndexBuffers(builder.indices);
	}
	LitModel::~LitModel()
	{

	}
	std::unique_ptr<LitModel> LitModel::CreateModelFromFile(LitDevice& device, const std::string& filepath) 
	{
		Builder builder{};
		builder.LoadModel(filepath);
		return std::make_unique<LitModel>(device, builder);
	}
	void LitModel::CreateVertexBuffer(const std::vector<Vertex>& vertices)
	{
		vertexCount = static_cast<uint32_t>(vertices.size());
		assert(vertexCount >= 3 && "vertex count must be at least 3");
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
		VkDeviceSize vertexSize = sizeof(vertices[0]);
		LitBuffer stagingBuffer(device, vertexSize, vertexCount, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		stagingBuffer.Map();
		stagingBuffer.WriteToBuffer((void*)vertices.data());
		vertexBuffer = std::make_unique<LitBuffer>(device, vertexSize, vertexCount,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		device.CopyBuffer(stagingBuffer.GetBuffer(), vertexBuffer->GetBuffer(), bufferSize);
	}

	void LitModel::createIndexBuffers(const std::vector<uint32_t>& indices) 
	{
		indexCount = static_cast<uint32_t>(indices.size());
		hasIndexBuffer = indexCount > 0;

		if (!hasIndexBuffer) {
			return;
		}

		VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;
		VkDeviceSize indexSize = sizeof(indices[0]);
		LitBuffer stagingBuffer(device, indexSize, indexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		stagingBuffer.Map();
		stagingBuffer.WriteToBuffer((void*)indices.data());
		indexBuffer = std::make_unique<LitBuffer>(device, indexSize, indexCount,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		device.CopyBuffer(stagingBuffer.GetBuffer(), indexBuffer->GetBuffer(), bufferSize);
	}

	void LitModel::Draw(VkCommandBuffer commandBuffer)
	{
		if (hasIndexBuffer)
		{
			vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
		}
		else {
			vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
		}
	}

	void LitModel::Bind(VkCommandBuffer commandBuffer)
	{
		VkBuffer buffers[] = { vertexBuffer->GetBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
		if (hasIndexBuffer) 
		{
			vkCmdBindIndexBuffer(commandBuffer, indexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
		}
	}

	void LitModel::Builder::LoadModel(const std::string& filepath)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str())) {
			throw std::runtime_error(warn + err);
		}

		vertices.clear();
		indices.clear();

		std::unordered_map<Vertex, uint32_t> uniqueVertices{};
		for (const auto& shape : shapes) {
			for (const auto& index : shape.mesh.indices) {
				Vertex vertex{};

				if (index.vertex_index >= 0) {
					vertex.position = glm::vec3{
						attrib.vertices[3 * index.vertex_index + 0],
						attrib.vertices[3 * index.vertex_index + 1],
						attrib.vertices[3 * index.vertex_index + 2],
					};

					//auto colorIndex = 3 * index.vertex_index + 2;
					//if (colorIndex < attrib.colors.size()) {
					//	vertex.color = glm::vec3{
					//		attrib.colors[colorIndex - 2],
					//		attrib.colors[colorIndex - 1],
					//		attrib.colors[colorIndex - 0],
					//	};
					//}
					//else {
					//	vertex.color = glm::vec3{ 1.f, 1.f, 1.f };  // set default color
					//}
					vertex.color = glm::vec3{
					   attrib.colors[3 * index.vertex_index + 0],
					   attrib.colors[3 * index.vertex_index + 1],
					   attrib.colors[3 * index.vertex_index + 2],
					};
				}

				if (index.normal_index >= 0) {
					vertex.normal = glm::vec3{
						attrib.normals[3 * index.normal_index + 0],
						attrib.normals[3 * index.normal_index + 1],
						attrib.normals[3 * index.normal_index + 2],
					};
				}

				if (index.texcoord_index >= 0) {
					vertex.uv = glm::vec2{
						attrib.texcoords[2 * index.texcoord_index + 0],
						attrib.texcoords[2 * index.texcoord_index + 1],
					};
				}

				if (uniqueVertices.count(vertex) == 0) {
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
				}
				indices.push_back(uniqueVertices[vertex]);
			}
		}
	}
}
