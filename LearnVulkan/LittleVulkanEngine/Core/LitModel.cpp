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
	LitModel::LitModel(LitDevice& inDevice, const Builder& builder):
		device(inDevice)
	{ 
		CreateVertexBuffer(builder.vertices);
		createIndexBuffers(builder.indices);
	}


	void LitModel::CreateVertexBuffer(const std::vector<Vertex>& vertices)
	{
		vertexCount = static_cast<uint32_t>(vertices.size());
		assert(vertexCount >= 3 && "vertex count must be at least 3");
		VkDeviceSize bufferSize = sizeof(Vertex) * vertexCount;

		// using stage buffer
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;

		device.CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
		void* data;
		vkMapMemory(device.GetDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), (size_t)bufferSize);
		vkUnmapMemory(device.GetDevice(), stagingBufferMemory);
		// copy stage buffer to vertex buffer

		device.CreateBuffer(
			bufferSize,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			vertexBuffer,
			vertexBufferMemory);
		device.CopyBuffer(stagingBuffer, vertexBuffer, bufferSize);
		vkDestroyBuffer(device.GetDevice(), stagingBuffer, nullptr);
		vkFreeMemory(device.GetDevice(), stagingBufferMemory, nullptr);
	}
	void LitModel::createIndexBuffers(const std::vector<uint32_t>& indices) 
	{
		indexCount = static_cast<uint32_t>(indices.size());
		hasIndexBuffer = indexCount > 0;

		if (!hasIndexBuffer) {
			return;
		}

		VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		device.CreateBuffer(
			bufferSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer,
			stagingBufferMemory);

		void* data;
		vkMapMemory(device.GetDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(device.GetDevice(), stagingBufferMemory);

		device.CreateBuffer(
			bufferSize,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			indexBuffer,
			indexBufferMemory);

		device.CopyBuffer(stagingBuffer, indexBuffer, bufferSize);

		vkDestroyBuffer(device.GetDevice(), stagingBuffer, nullptr);
		vkFreeMemory(device.GetDevice(), stagingBufferMemory, nullptr);
	}

	void LitModel::CleanUp()
	{
		vkDestroyBuffer(device.GetDevice(), vertexBuffer, nullptr);
		vkFreeMemory(device.GetDevice(), vertexBufferMemory, nullptr);
		if (hasIndexBuffer) 
		{
			vkDestroyBuffer(device.GetDevice(), indexBuffer, nullptr);
			vkFreeMemory(device.GetDevice(), indexBufferMemory, nullptr);
		}
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
		VkBuffer buffers[] = { vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
		if (hasIndexBuffer) 
		{
			vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		}
	}



}
