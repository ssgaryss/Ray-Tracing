#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"

#include "Renderer.h"
#include "Scene.h"

#include <glm/gtc/type_ptr.hpp>

using namespace Walnut;

class EditorLayer : public Layer
{
public:
	void OnAttach() override {
		uint32_t Width = 1000;
		uint32_t Height = 1000;
		m_Renderer = std::make_unique<Renderer>(Width, Height);
		m_Renderer->onResize(Width, Height);
		m_EditorCamera = std::make_shared<RayTracingCamera>();
		m_EditorCamera->setViewportSize((float)Width, (float)Height);
		m_ActiveScene = std::make_shared<Scene>();

		m_Renderer->setCamera(m_EditorCamera);
		m_Renderer->setScene(m_ActiveScene);

		Material DefaultMaterial; // 0
		DefaultMaterial.m_Albedo = { 0.5f, 0.5f, 0.5f };
		m_ActiveScene->m_Materials.emplace_back(DefaultMaterial);

		Material Blue; // 1
		Blue.m_Albedo = { 0.1f, 0.1f, 0.8f };
		Blue.m_Roughness = 0.0f;
		m_ActiveScene->m_Materials.emplace_back(Blue);

		Material Water; // 2
		Water.m_Albedo = { 0.08f, 0.765f, 0.84f };
		Water.m_Roughness = 0.2f;
		m_ActiveScene->m_Materials.emplace_back(Water);

		Material Pink; // 3
		Pink.m_Albedo = { 0.9f, 0.1f, 0.9f };
		Pink.m_Roughness = 0.0f;
		m_ActiveScene->m_Materials.emplace_back(Pink);

		Material Light; // 4
		Light.m_Albedo = { 1.0f, 1.0f, 0.5f };
		Light.m_EmissionColor = { 1.0f, 0.7f, 0.2f };
		Light.m_EmissionPower = 1.0f;
		m_ActiveScene->m_Materials.emplace_back(Light);

		Sphere BlueSphere;
		BlueSphere.m_Radius = 5.0f;
		BlueSphere.m_MaterialIndex = 1;
		m_ActiveScene->m_Spheres.emplace_back(BlueSphere);

		Sphere PinkSphere;
		PinkSphere.m_Radius = 3.0f;
		PinkSphere.m_Position = { 10.0f, 3.0f, 3.0f };
		PinkSphere.m_MaterialIndex = 3;
		m_ActiveScene->m_Spheres.emplace_back(PinkSphere);

		Sphere LightSphere;
		LightSphere.m_Radius = 11.8f;
		LightSphere.m_Position = { 40.0f, 10.0f, 100.0f };
		LightSphere.m_MaterialIndex = 4;
		m_ActiveScene->m_Spheres.emplace_back(LightSphere);


		Sphere Ground;
		Ground.m_Position = { 0.0f, 0.0f, -1010.0f };
		Ground.m_Radius = 1000.0f;
		Ground.m_MaterialIndex = 2;
		m_ActiveScene->m_Spheres.emplace_back(Ground);
	}
	void OnDetach() override {}

	void OnUpdate(float ts) override {
		if (m_EditorCamera->onUpdate(ts))
			m_Renderer->resetFrameIndex();
		render();
	}

	void OnUIRender() override {
		ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar);
		const auto& Size = ImGui::GetContentRegionAvail();
		m_ViewportSize = { Size.x, Size.y };
		const auto& RenderResult = m_Renderer->getRenderResult();
		ImGui::Image(RenderResult->GetDescriptorSet(), { (float)RenderResult->GetWidth(), (float)RenderResult->GetHeight() }, { 0, 1 }, { 1, 0 });
		ImGui::End();

		ImGui::Begin("Settings");
		ImGui::Text("Time : ");
		ImGui::SameLine();
		ImGui::Text(std::to_string(m_LastRenderTime).append(" ms").c_str());

		bool IsResetFrameIndex = false;
		ImGui::Text("Blue Sphere");
		IsResetFrameIndex |= ImGui::DragFloat3("##Blue Sphere Position", glm::value_ptr(m_ActiveScene->m_Spheres[0].m_Position), 0.1f);
		IsResetFrameIndex |= ImGui::ColorEdit3("Albedo##Blue", glm::value_ptr(m_ActiveScene->m_Materials[1].m_Albedo));
		ImGui::Separator();
		ImGui::Text("Pink Sphere");
		IsResetFrameIndex |= ImGui::DragFloat3("##Pink Sphere Position", glm::value_ptr(m_ActiveScene->m_Spheres[1].m_Position), 0.1f);
		IsResetFrameIndex |= ImGui::ColorEdit3("Albedo##Pink", glm::value_ptr(m_ActiveScene->m_Materials[3].m_Albedo));
		ImGui::Separator();
		ImGui::Text("Light Sphere");
		IsResetFrameIndex |= ImGui::DragFloat3("##Light Sphere Position", glm::value_ptr(m_ActiveScene->m_Spheres[2].m_Position), 0.1f);
		IsResetFrameIndex |= ImGui::DragFloat("Radius##Light", &m_ActiveScene->m_Spheres[2].m_Radius, 0.2f, 1.0f, 1000.0f);
		IsResetFrameIndex |= ImGui::ColorEdit3("Albedo##Light", glm::value_ptr(m_ActiveScene->m_Materials[4].m_Albedo));
		IsResetFrameIndex |= ImGui::DragFloat("Power##Light", &m_ActiveScene->m_Materials[4].m_EmissionPower, 0.2f, 0.0f, 1000.0f);
		IsResetFrameIndex |= ImGui::ColorEdit3("Light Color##Light", glm::value_ptr(m_ActiveScene->m_Materials[4].m_EmissionColor));
		ImGui::Separator();
		ImGui::Text("Water");
		IsResetFrameIndex |= ImGui::DragFloat("Power##Water", &m_ActiveScene->m_Materials[2].m_EmissionPower, 0.2f, 0.0f, 1000.0f);
		IsResetFrameIndex |= ImGui::ColorEdit3("Light Color##Water", glm::value_ptr(m_ActiveScene->m_Materials[2].m_EmissionColor));
		ImGui::Separator();
		if (IsResetFrameIndex)
			m_Renderer->resetFrameIndex();
		ImGui::End();
	}

	void render() {
		Timer Timer;
		if (m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && (m_Renderer->getRenderResult()->GetWidth() != m_ViewportSize.x ||
			m_Renderer->getRenderResult()->GetHeight() != m_ViewportSize.y)) 
		{
			m_Renderer->onResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_EditorCamera->setViewportSize(m_ViewportSize.x, m_ViewportSize.y);
		}
		m_Renderer->Render();
		m_LastRenderTime = Timer.ElapsedMillis();
	}
private:
	std::unique_ptr<Renderer> m_Renderer = nullptr;
	std::shared_ptr<Scene> m_ActiveScene;
	std::shared_ptr<RayTracingCamera> m_EditorCamera;
	bool isShowSettingsPanel = true;
	glm::vec2 m_ViewportSize{ 0.0f, 0.0f };
	float m_LastRenderTime = 0.0f;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Ray Tracing";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<EditorLayer>();
	app->SetMenubarCallback([app]()
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Exit"))
				{
					app->Close();
				}
				ImGui::EndMenu();
			}
		});
	return app;
}