#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"

#include "Renderer.h"
#include "Scene.h"

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
		m_EditorCamera->setViewportSize(Width, Height);
		m_ActiveScene = std::make_shared<Scene>();

		m_Renderer->setCamera(m_EditorCamera);
		m_Renderer->setScene(m_ActiveScene);

		Material DefaultMaterial;
		DefaultMaterial.m_Albedo = { 0.5f, 0.5f, 0.5f };
		m_ActiveScene->m_Materials.emplace_back(DefaultMaterial);

		Sphere BlueSphere;
		BlueSphere.m_Radius = 5.0f;
		m_ActiveScene->m_Spheres.emplace_back(BlueSphere);
	}
	void OnDetach() override {}

	void OnUpdate(float ts) override {
		//m_Renderer->onResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		//m_EditorCamera->setViewportSize(m_ViewportSize.x, m_ViewportSize.y);
		m_EditorCamera->onUpdate(ts);
		render();
	}

	void OnUIRender() override {
		ImGui::Begin("Viewport");
		auto Size = ImGui::GetContentRegionAvail();
		m_ViewportSize = { Size.x, Size.y };
		const auto& RenderResult = m_Renderer->getRenderResult();
		ImGui::Image(RenderResult->GetDescriptorSet(), { (float)RenderResult->GetWidth(), (float)RenderResult->GetHeight() }, { 0, 1 }, { 1, 0 });
		ImGui::End();

		ImGui::Begin("Settings");
		ImGui::Text("Time : ");
		ImGui::SameLine();
		ImGui::Text(std::to_string(m_LastRenderTime).append(" ms").c_str());
		if (ImGui::Button("Render")) {

		}
		ImGui::End();
	}

	void render() {
		Timer Timer;
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