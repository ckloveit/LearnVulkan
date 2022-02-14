#include "LitBuffer.h"

// std
#include <cassert>
#include <cstring>

namespace Lit
{
	/**
	 * Returns the minimum instance size required to be compatible with devices minOffsetAlignment
	 *
	 * @param instanceSize The size of an instance
	 * @param minOffsetAlignment The minimum required alignment, in bytes, for the offset member (eg
	 * minUniformBufferOffsetAlignment)
	 *
	 * @return VkResult of the buffer mapping call
	 */
	VkDeviceSize LitBuffer::GetAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment)
	{
		if (minOffsetAlignment > 0)
		{
			return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
		}
		return instanceSize;
	}

	LitBuffer::LitBuffer(LitDevice& device, VkDeviceSize& instanceSize, 
		uint32_t instanceCount, VkBufferUsageFlags usageFlags, 
		VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize minOffsetAlignment /* = 1 */)
		:litDevice(device), instanceSize(instanceSize),instanceCount(instanceCount),
		usageFlags(usageFlags), memoryPropertyFlags(memoryPropertyFlags)
	{
		alignmentSize = GetAlignment(instanceSize, minOffsetAlignment);
		bufferSize = alignmentSize * instanceCount;
		device.CreateBuffer(bufferSize, usageFlags, memoryPropertyFlags, buffer, memory);
	}

	LitBuffer::~LitBuffer()
	{
		UnMap();
		vkDestroyBuffer(litDevice.GetDevice(), buffer, nullptr);
		vkFreeMemory(litDevice.GetDevice(), memory, nullptr);
	}

	/**
	 * Map a memory range of this buffer. If successful, mapped points to the specified buffer range.
	 *
	 * @param size (Optional) Size of the memory range to map. Pass VK_WHOLE_SIZE to map the complete
	 * buffer range.
	 * @param offset (Optional) Byte offset from beginning
	 *
	 * @return VkResult of the buffer mapping call
	 */
	VkResult LitBuffer::Map(VkDeviceSize size, VkDeviceSize offset)
	{
		assert(buffer && memory && "called map on buffer before create");
		/*if (size == VK_WHOLE_SIZE)
		{
			return vkMapMemory(litDevice.GetDevice(), memory, 0, bufferSize, 0, &mapped);
		}*/
		return vkMapMemory(litDevice.GetDevice(), memory, offset, size, 0, &mapped);
	}

	/**
	 * Unmap a mapped memory range
	 *
	 * @note Does not return a result as vkUnmapMemory can't fail
	 */
	void LitBuffer::UnMap()
	{
		if (mapped)
		{
			vkUnmapMemory(litDevice.GetDevice(), memory);
			mapped = nullptr;
		}
	}

	/**
	 * Copies the specified data to the mapped buffer. Default value writes whole buffer range
	 *
	 * @param data Pointer to the data to copy
	 * @param size (Optional) Size of the data to copy. Pass VK_WHOLE_SIZE to flush the complete buffer
	 * range.
	 * @param offset (Optional) Byte offset from beginning of mapped region
	 *
	 */
	void LitBuffer::WriteToBuffer(void* data, VkDeviceSize size, VkDeviceSize offset)
	{
		assert(mapped && "cannot copy to unmapped buffer");
		if (size == VK_WHOLE_SIZE)
		{
			memcpy(mapped, data, bufferSize);
		}
		else
		{
			char* memoffset = (char*)mapped;
			memoffset += offset;
			memcpy(memoffset, data, size);
		}
	}

	/**
	 * Flush a memory range of the buffer to make it visible to the device
	 *
	 * @note Only required for non-coherent memory
	 *
	 * @param size (Optional) Size of the memory range to flush. Pass VK_WHOLE_SIZE to flush the
	 * complete buffer range.
	 * @param offset (Optional) Byte offset from beginning
	 *
	 * @return VkResult of the flush call
	 */
	VkResult LitBuffer::Flush(VkDeviceSize size, VkDeviceSize offset)
	{
		VkMappedMemoryRange mappedRange = {};
		mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		mappedRange.memory = memory;
		mappedRange.offset = offset;
		mappedRange.size = size;
		return vkFlushMappedMemoryRanges(litDevice.GetDevice(), 1, &mappedRange);
	}

	/**
	 * Invalidate a memory range of the buffer to make it visible to the host
	 *
	 * @note Only required for non-coherent memory
	 *
	 * @param size (Optional) Size of the memory range to invalidate. Pass VK_WHOLE_SIZE to invalidate
	 * the complete buffer range.
	 * @param offset (Optional) Byte offset from beginning
	 *
	 * @return VkResult of the invalidate call
	 */
	VkResult LitBuffer::Invalidate(VkDeviceSize size, VkDeviceSize offset) 
	{
		VkMappedMemoryRange mappedRange = {};
		mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		mappedRange.memory = memory;
		mappedRange.offset = offset;
		mappedRange.size = size;
		return vkInvalidateMappedMemoryRanges(litDevice.GetDevice(), 1, &mappedRange);
	}

	/**
	 * Create a buffer info descriptor
	 *
	 * @param size (Optional) Size of the memory range of the descriptor
	 * @param offset (Optional) Byte offset from beginning
	 *
	 * @return VkDescriptorBufferInfo of specified offset and range
	 */
	VkDescriptorBufferInfo LitBuffer::DescriptorInfo(VkDeviceSize size, VkDeviceSize offset)
	{
		return VkDescriptorBufferInfo{
			buffer,
			offset,
			size
		};
	}

	void LitBuffer::WriteToIndex(void* data, int index)
	{
		WriteToBuffer(data, instanceSize, index * alignmentSize);
	}

	VkResult LitBuffer::FlushIndex(int index) { return Flush(alignmentSize, index * alignmentSize); }

	VkDescriptorBufferInfo LitBuffer::DescriptorInfoForIndex(int index)
	{
		return DescriptorInfo(alignmentSize, index * alignmentSize);
	}

	VkResult LitBuffer::InvalidateIndex(int index)
	{
		return Invalidate(alignmentSize, index * alignmentSize);
	}


}
