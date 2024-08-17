#pragma once
#include "Scene.h"
#include "RayTracingCamera.h"
#include "Ray.h"

#include <memory>
#include <Walnut/Image.h>

class Renderer
{
public:
	struct Settings
	{
		bool m_IsAccumulate = true;
	};
public:
	Renderer() = default;
	Renderer(uint32_t vWidth, uint32_t vHeight);

	void onResize(uint32_t vWidth, uint32_t vHeight);
	void Render();

	inline void setScene(const std::shared_ptr<Scene>& vScene) { m_Scene = vScene; }
	inline void setCamera(const std::shared_ptr<RayTracingCamera>& vCamera) { m_Camera = vCamera; }
	inline const std::shared_ptr<Walnut::Image>& getRenderResult() const { return m_RenderResult; }
	inline void resetFrameIndex() { m_FrameIndex = 1; }
private:
	struct HitPayload
	{
		float m_HitDistance;
		glm::vec3 m_WorldPosition;
		glm::vec3 m_WorldNormal;

		int m_SphereIndex;
	};

	glm::vec4 perPixel(uint32_t x, uint32_t y); // A ray path trace result
	HitPayload traceRay(const Ray& vRay);        // Single ray trace
	HitPayload missRay(const Ray& vRay);         // Miss a ray
	HitPayload closestHit(const Ray& vRay, float vHitDistance, int vSphereIndex);
private:
	std::shared_ptr<Walnut::Image> m_RenderResult = nullptr;
	std::unique_ptr<std::vector<uint32_t>> m_ImageData;
	Settings m_Settings;
	std::unique_ptr<std::vector<glm::vec4>> m_AccumulateColor;
	uint32_t m_FrameIndex = 1;

	std::shared_ptr<Scene> m_Scene = nullptr;
	std::shared_ptr<RayTracingCamera> m_Camera = nullptr;
	// Muti-thread
	std::vector<uint32_t> m_VerticalIterator, m_HorizontalIterator;
};

