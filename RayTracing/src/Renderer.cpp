#include "Renderer.h"
#include "Ray.h"

#include <numeric>
#include <iostream>

namespace Util {
	static uint32_t ConvertGLMColorToRGBA(const glm::vec4& vColor) {
		uint8_t r = static_cast<uint8_t>(vColor.r * 255.0f);
		uint8_t g = static_cast<uint8_t>(vColor.g * 255.0f);
		uint8_t b = static_cast<uint8_t>(vColor.b * 255.0f);
		uint8_t a = static_cast<uint8_t>(vColor.a * 255.0f);
		return (a << 24) | (b << 16) | (g << 8) | r;
	}
}

Renderer::Renderer(uint32_t vWidth, uint32_t vHeight)
{
	m_ImageData = std::make_unique<std::vector<uint32_t>>(vWidth * vHeight);
	m_RenderResult = std::make_shared<Walnut::Image>(vWidth, vHeight, Walnut::ImageFormat::RGBA, m_ImageData->data());
}

void Renderer::onResize(uint32_t vWidth, uint32_t vHeight)
{
	if (m_RenderResult) {
		if (vWidth == m_RenderResult->GetWidth() && vHeight == m_RenderResult->GetHeight()) {
			return;
			m_RenderResult->Resize(vWidth, vHeight);
		}
	}
}

void Renderer::Render()
{
	uint32_t Width = m_RenderResult->GetWidth();
	uint32_t Height = m_RenderResult->GetHeight();
	if (m_Scene && m_Camera) {
		for (uint32_t y = 0; y < Height; ++y) {
			for (uint32_t x = 0; x < Width; ++x) {
				auto Color = perPixel(x, y);
				m_ImageData->at(y * Width + x) = Util::ConvertGLMColorToRGBA(Color);
			}
		}
		m_RenderResult->SetData(m_ImageData->data());
	}

}

glm::vec4 Renderer::perPixel(uint32_t x, uint32_t y)
{
	Ray Ray(m_Camera->getPosition(), m_Camera->getRayDirections()[y * m_RenderResult->GetWidth() + x]);
	glm::vec3 Light{ 0.0f };         // 光线颜色
	glm::vec3 Contribution{ 1.0f };  // 权重

	int Bounce = 1;
	for (int i = 0; i < Bounce; ++i) {
		auto& HitPayload = traceRay(Ray);
		if (HitPayload.m_HitDistance < 0.0f) {
			glm::vec3 SkyClolor{ 0.6f, 0.7f, 0.9f };
			Light += SkyClolor * Contribution;
			break;
		}
		else {
			const Sphere& Sphere = m_Scene->m_Spheres[HitPayload.m_SphereIndex];
			Light += m_Scene->m_Materials[Sphere.m_MaterialIndex].m_Albedo;
		}

	}
	return glm::vec4(Light, 1.0f);
}

Renderer::HitPayload Renderer::traceRay(const Ray& vRay)
{
	// (bx^2 + by^2)t^2 + (2(axbx + ayby))t + (ax^2 + ay^2 - r^2) = 0
	// where
	// a = ray origin
	// b = ray direction
	// r = radius
	// t = hit distance
	int ClosestSphereIndex = -1;
	float HitDistance = std::numeric_limits<float>::max();
	for (int i = 0; i < (int)m_Scene->m_Spheres.size(); ++i) {
		const Sphere& Sphere = m_Scene->m_Spheres[i];
		glm::vec3 OriginDistance = vRay.m_Origin - Sphere.m_Position;
		float a = glm::dot(vRay.m_Direction, vRay.m_Direction);
		float b = 2.0f * glm::dot(OriginDistance, vRay.m_Direction);
		float c = glm::dot(OriginDistance, OriginDistance) - Sphere.m_Radius * Sphere.m_Radius;

		float Discriminant = b * b - 4.0f * a * c;
		if (Discriminant < 0.0f)
			continue;

		float ClosestHit = (-b - glm::sqrt(Discriminant)) / (2.0f * a);
		if (ClosestHit > 0.0f && ClosestHit < HitDistance) {
			HitDistance = ClosestHit;
			ClosestSphereIndex = i;
		}
	}
	if (ClosestSphereIndex == -1) {
		return missRay(vRay);
	}

	return closestHit(vRay, HitDistance, ClosestSphereIndex);
}

Renderer::HitPayload Renderer::missRay(const Ray& vRay)
{
	HitPayload Payload;
	Payload.m_HitDistance = -1.0f;
	return Payload;
}

Renderer::HitPayload Renderer::closestHit(const Ray& vRay, float vHitDistance, int vSphereIndex)
{
	HitPayload Payload;
	Payload.m_HitDistance = vHitDistance;
	Payload.m_SphereIndex = vSphereIndex;
	Payload.m_WorldPosition = vRay.m_Origin + vHitDistance * vRay.m_Direction;
	Payload.m_WorldNormal = glm::normalize(Payload.m_WorldPosition - m_Scene->m_Spheres[vSphereIndex].m_Position);
	return Payload;
}
