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
		m_Renderer = std::make_unique<Renderer>(1000, 1000);
		m_Renderer->setCamera(std::make_shared<RayTracingCamera>(m_EditorCamera));
		m_Renderer->setScene(std::make_shared<Scene>(m_ActiveScene));

		Sphere BlueSphere;
		BlueSphere.m_Radius = 5.0f;
		m_ActiveScene.m_Spheres.emplace_back(BlueSphere);
	}
	void OnDetach() override {}

	void OnUpdate(float ts) override {
		m_Renderer->onResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_EditorCamera.onUpdate(ts);
		m_Renderer->Render();
	}

	void OnUIRender() override {
		ImGui::Begin("Viewport");
		auto Size = ImGui::GetContentRegionAvail();
		m_ViewportSize = { Size.x, Size.y };
		const auto& RenderResult = m_Renderer->getRenderResult();
		ImGui::Image(RenderResult->GetDescriptorSet(), { (float)RenderResult->GetWidth(), (float)RenderResult->GetHeight() }, { 0, 1 }, { 1, 0 });
		ImGui::End();

		ImGui::Begin("Settings");
		if (ImGui::Button("Render")) {

		}
		ImGui::End();
	}
private:
	std::unique_ptr<Renderer> m_Renderer = nullptr;
	Scene m_ActiveScene;
	RayTracingCamera m_EditorCamera;
	bool isShowSettingsPanel = true;
	glm::vec2 m_ViewportSize{ 0.0f, 0.0f };
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