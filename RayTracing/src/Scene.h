#pragma once
#include <memory>
#include <vector>
#include <glm/glm.hpp>

struct Material
{
	glm::vec3 m_Albedo{ 1.0f };
};

struct Sphere
{
	glm::vec3 m_Position = glm::vec3(0.0f);
	float m_Radius = 1.0f;

	int m_MaterialIndex = 0;
};

struct Scene
{
	std::vector<Sphere> m_Spheres;
	std::vector<Material> m_Materials;
	Scene() {
		Material DefaultMaterial;
		DefaultMaterial.m_Albedo = { 0.5f, 0.5f, 0.5f };
		m_Materials.emplace_back(DefaultMaterial);
	}
};