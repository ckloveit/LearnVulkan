#pragma once

#include <memory>

#include "LitModel.h"

namespace Lit
{
	struct Transform2DComponent
	{
		glm::vec2 translation{}; // position offset
		glm::vec2 scale{ 1.0f, 1.0f };
		float rotation;
		glm::mat2 LocalToWorldMatrix()
		{
			const float s = glm::sin(rotation);
			const float c = glm::cos(rotation);
			glm::mat2 rotationMatrix{ glm::vec2{c, s}, glm::vec2{-s,c} };
			glm::mat2 scaleMatrix = glm::mat2{ glm::vec2{scale.x, 0.0f}, glm::vec2{0.0f, scale.y} };
			return rotationMatrix * scaleMatrix;
		}
	};

	struct TransformComponent
	{
		glm::vec3 translation{};
		glm::vec3 scale{ 1.f, 1.f, 1.f };
		glm::vec3 rotation{};

		// Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
		 // Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
		 // https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
		glm::mat4 mat4();
		glm::mat4 normalMatrix();
	};

	class LitGameObject
	{
	public:
		using id_t = unsigned int;

		static LitGameObject CreateGameObject()
		{
			static id_t objectId = 0;
			return LitGameObject{ objectId++ };
		}

		LitGameObject(const LitGameObject&) = delete;
		LitGameObject& operator=(const LitGameObject&) = delete;
		LitGameObject(LitGameObject&&) = default;
		LitGameObject& operator=(LitGameObject&&) = default;
		id_t GetID() { return id; }
	private:
		LitGameObject(id_t objId) : id{ objId } {}

	public:
		std::shared_ptr<LitModel> model{};
		glm::vec3 color{};
		TransformComponent transform{};
	private:
		id_t id;
	};

}