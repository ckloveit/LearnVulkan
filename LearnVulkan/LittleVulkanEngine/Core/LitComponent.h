#pragma once
#include <cassert>
#include <unordered_map>

#include <glm/glm.hpp>

namespace Lit
{
	using gameObjectId_t = int64_t;

	template<class T, std::size_t BlockSizeBytes>
	class LitComponentTable
	{
		static_assert(sizeof(T) <= BlockSizeBytes, "ize of component type for component table exceeds block size");

	public:
		LitComponentTable(std::unordered_map<gameObjectId_t, char[BlockSizeBytes]>& tableRef)
			:table(tableRef)
		{

		}

		T& operator[](int index)
		{
			assert(index >= 0 && index < BlockSizeBytes);
			return *reinterpret_cast<T*>(&table[index]);
		}

	private:
		std::unordered_map<gameObjectId_t, char[BlockSizeBytes]>& table;
	};

	class Transform
	{
	public:
		glm::vec3 position;
		glm::vec4 rotation;
		glm::vec3 scale;
	};



}