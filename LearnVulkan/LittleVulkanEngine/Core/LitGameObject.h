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
		glm::mat4 mat4() {
			const float c3 = glm::cos(rotation.z);
			const float s3 = glm::sin(rotation.z);
			const float c2 = glm::cos(rotation.x);
			const float s2 = glm::sin(rotation.x);
			const float c1 = glm::cos(rotation.y);
			const float s1 = glm::sin(rotation.y);
			return glm::mat4{
				glm::vec4{
					scale.x * (c1 * c3 + s1 * s2 * s3),
					scale.x * (c2 * s3),
					scale.x * (c1 * s2 * s3 - c3 * s1),
					0.0f,
				},
				glm::vec4{
					scale.y * (c3 * s1 * s2 - c1 * s3),
					scale.y * (c2 * c3),
					scale.y * (c1 * c3 * s2 + s1 * s3),
					0.0f,
				},
				glm::vec4{
					scale.z * (c2 * s1),
					scale.z * (-s2),
					scale.z * (c1 * c2),
					0.0f,
				},
				glm::vec4{translation.x, translation.y, translation.z, 1.0f} };
		}
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