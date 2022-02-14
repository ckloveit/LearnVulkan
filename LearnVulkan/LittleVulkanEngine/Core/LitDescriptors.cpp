#include "LitDescriptors.h"

//std
#include <cassert>
#include <stdexcept>

namespace Lit
{
	//---------------------Descriptor Set Layout Builder---------------------//
	LitDescriptorSetLayout::Builder& LitDescriptorSetLayout::Builder::AddBinding(uint32_t bindingIndex, VkDescriptorType descriptorType, 
		VkShaderStageFlags stageFlags, uint32_t count /* = 1 */)
	{
		assert(bindings.count(bindingIndex) == 0 && "Binding already in use");
		VkDescriptorSetLayoutBinding layoutBinding = {};
		layoutBinding.binding = bindingIndex;
		layoutBinding.descriptorType = descriptorType;
		layoutBinding.descriptorCount = count;
		layoutBinding.stageFlags = stageFlags;
		bindings[bindingIndex] = layoutBinding;
		return *this;
	}
	std::unique_ptr<LitDescriptorSetLayout> LitDescriptorSetLayout::Builder::Build() const
	{
		return std::make_unique<LitDescriptorSetLayout>(device, bindings);
	}

	//---------------------Descriptor Set Layout------------------------------//
	LitDescriptorSetLayout::LitDescriptorSetLayout(LitDevice& device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings)
		:device(device), bindings(bindings)
	{
		std::vector<VkDescriptorSetLayoutBinding> setlayoutBindings = {};
		for (auto& kv : bindings)
		{
			setlayoutBindings.push_back(kv.second);
		}
		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo = {};
		descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setlayoutBindings.size());
		descriptorSetLayoutInfo.pBindings = setlayoutBindings.data();

		if (vkCreateDescriptorSetLayout(device.GetDevice(), &descriptorSetLayoutInfo,
			nullptr, &descriptorSetLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create descriptor set layout");
		}
	}

	LitDescriptorSetLayout::~LitDescriptorSetLayout()
	{
		vkDestroyDescriptorSetLayout(device.GetDevice(), descriptorSetLayout, nullptr);
	}

	//-----------------------Descriptor pool Builder---------------------------------//
	LitDescriptorPool::Builder& LitDescriptorPool::Builder::AddPoolSize(VkDescriptorType descriptorType, uint32_t count)
	{
		poolSizes.push_back({ descriptorType, count });
		return *this;
	}

	LitDescriptorPool::Builder& LitDescriptorPool::Builder::SetPoolFlags(VkDescriptorPoolCreateFlags flags)
	{
		poolFlags = flags;
		return *this;
	}

	LitDescriptorPool::Builder& LitDescriptorPool::Builder::SetMaxSets(uint32_t count)
	{
		maxSets = count;
		return *this;
	}

	std::unique_ptr<LitDescriptorPool> LitDescriptorPool::Builder::Build() const
	{
		return std::make_unique<LitDescriptorPool>(device, maxSets, poolFlags, poolSizes);
	}

	// *************** Descriptor Pool *********************
	LitDescriptorPool::LitDescriptorPool(LitDevice& litDevice, uint32_t maxSets, VkDescriptorPoolCreateFlags poolFlags,
		const std::vector<VkDescriptorPoolSize>& poolSizes)
		: device(litDevice)
	{
		VkDescriptorPoolCreateInfo descriptorPoolInfo{};
		descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		descriptorPoolInfo.pPoolSizes = poolSizes.data();
		descriptorPoolInfo.maxSets = maxSets;
		descriptorPoolInfo.flags = poolFlags;
		if (vkCreateDescriptorPool(device.GetDevice(), &descriptorPoolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create descriptor pool!");
		}
	}

	LitDescriptorPool::~LitDescriptorPool()
	{
		vkDestroyDescriptorPool(device.GetDevice(), descriptorPool, nullptr);
	}

	bool LitDescriptorPool::AllocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const 
	{
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.pSetLayouts = &descriptorSetLayout;
		allocInfo.descriptorSetCount = 1;

		// Might want to create a "DescriptorPoolManager" class that handles this case, and builds
		// a new pool whenever an old pool fills up. But this is beyond our current scope
		if (vkAllocateDescriptorSets(device.GetDevice(), &allocInfo, &descriptor) != VK_SUCCESS) {
			return false;
		}
		return true;
	}
	void LitDescriptorPool::FreeDescriptors(std::vector<VkDescriptorSet> &descriptors) const 
	{
		vkFreeDescriptorSets(
			device.GetDevice(),
			descriptorPool,
			static_cast<uint32_t>(descriptors.size()),
			descriptors.data());
	}

	void LitDescriptorPool::ResetPool()
	{
		vkResetDescriptorPool(device.GetDevice(), descriptorPool, 0);
	}

	// *************** Descriptor Writer *********************
	LitDescriptorWriter::LitDescriptorWriter(LitDescriptorSetLayout& setLayout, LitDescriptorPool& pool)
		: setLayout{ setLayout }, pool{ pool } {}

	LitDescriptorWriter& LitDescriptorWriter::WriteBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo)
	{
		assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

		auto& bindingDescription = setLayout.bindings[binding];

		assert(
			bindingDescription.descriptorCount == 1 &&
			"Binding single descriptor info, but binding expects multiple");

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.descriptorType = bindingDescription.descriptorType;
		write.dstBinding = binding;
		write.pBufferInfo = bufferInfo;
		write.descriptorCount = 1;

		writes.push_back(write);
		return *this;
	}
	LitDescriptorWriter& LitDescriptorWriter::WriteImage(uint32_t binding, VkDescriptorImageInfo * imageInfo)
	{
		assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

		auto& bindingDescription = setLayout.bindings[binding];

		assert(
			bindingDescription.descriptorCount == 1 &&
			"Binding single descriptor info, but binding expects multiple");

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.descriptorType = bindingDescription.descriptorType;
		write.dstBinding = binding;
		write.pImageInfo = imageInfo;
		write.descriptorCount = 1;

		writes.push_back(write);
		return *this;
	}
	bool LitDescriptorWriter::Build(VkDescriptorSet & set) 
	{
		bool success = pool.AllocateDescriptor(setLayout.GetDescriptorSetLayout(), set);
		if (!success) {
			return false;
		}
		OverWrite(set);
		return true;
	}

	void LitDescriptorWriter::OverWrite(VkDescriptorSet& set) 
	{
		for (auto& write : writes) 
		{
			write.dstSet = set;
		}
		vkUpdateDescriptorSets(pool.device.GetDevice(), writes.size(), writes.data(), 0, nullptr);
	}
}


