#pragma once
#include <glm/glm.hpp>

struct Ray
{
	glm::vec3 m_Origin;
	glm::vec3 m_Direction;
	Ray() = default;
	Ray(const glm::vec3& vOrigin, const glm::vec3& vDirection)
		: m_Origin{ vOrigin }, m_Direction{ vDirection } {}
};