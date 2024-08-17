#pragma once
#include <memory>
#include <vector>
#include <glm/glm.hpp>

struct Material
{
	glm::vec3 m_Albedo{ 1.0f };
	float m_Roughness = 1.0f;
	float m_Metalic = 0.0f;
	glm::vec3 m_EmissionColor{ 0.0f };
	float m_EmissionPower{ 0.0f };

	glm::vec3 getEmission() const { return m_EmissionColor * m_EmissionPower; }
};

struct Sphere
{
	glm::vec3 m_Position = glm::vec3(0.0f);
	float m_Radius = 1.0f;

	int m_MaterialIndex = 0;

	//glm::vec4 getEmission{}
};

struct Scene
{
	std::vector<Sphere> m_Spheres;
	std::vector<Material> m_Materials;
};