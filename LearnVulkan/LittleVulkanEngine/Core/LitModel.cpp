#include "LitModel.h"

// lib headers
#define GLM_ENABLE_EXPERIMENTAL

// std
#include <cassert>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>

namespace Lit
{
	LitModel::LitModel(LitDevice& InDevice) :
		device(InDevice) 
	{ 
		InitMeshVertices();
		CreateVertexBuffer(); 
	}
	void LitModel::InitMeshVertices()
	{
		vertices.emplace_back(Vertex{ glm::vec2{0.0f, -0.5f} });
		vertices.emplace_back(Vertex{ glm::vec2{0.5f, 0.5f} });
		vertices.emplace_back(Vertex{ glm::vec2{-0.5f, 0.5f} });
	}

	void LitModel::CreateVertexBuffer()
	{
		vertexCount = static_cast<uint32_t>(vertices.size());
		assert(vertexCount >= 3 && "vertex count must be at least 3");
		VkDeviceSize bufferSize = sizeof(Vertex) * vertexCount;
		device.CreateBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vertexBuffer, vertexBufferMemory);
		void* data;
		vkMapMemory(device.GetDevice(), vertexBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), (size_t)bufferSize);
		vkUnmapMemory(device.GetDevice(), vertexBufferMemory);
	}

	void LitModel::CleanUp()
	{
		vkDestroyBuffer(device.GetDevice(), vertexBuffer, nullptr);
		vkFreeMemory(device.GetDevice(), vertexBufferMemory, nullptr);
	}

	void LitModel::Draw(VkCommandBuffer commandBuffer)
	{
		vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
	}

	void LitModel::Bind(VkCommandBuffer commandBuffer)
	{
		VkBuffer buffers[] = { vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
	}



}
