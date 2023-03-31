#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Random.h"
#include "Walnut/Timer.h"

#include "Renderer/Renderer.h"
#include "Camera.h"

#include "Utilities/SceneFactory.h"

namespace IG = ImGui;
namespace WN = Walnut;

class ExampleLayer : public WN::Layer
{
public:
	ExampleLayer()
		: m_Camera(45.0f, 0.1f, 100.f) 
	{
		m_Scene = SceneFactory::createScene();
	}
	virtual void OnUpdate(float ts) override 
	{
		if (m_Camera.OnUpdate(ts)) {
			m_Renderer.ResetFrameIndex();
		};
	}
	virtual void OnUIRender() override
	{
		//Set Settings tab
		IG::Begin("Settings");
		IG::Text("Last render: %.3fms", m_LastRenderTime);
		//if (IG::Button("Render"))
		//{
		//	Render();
		//}

		IG::Text("");
		IG::Text("--- Path Tracing:");
		IG::Checkbox("Accumulation", &m_Renderer.GetSettings().Accumulate);
		IG::Checkbox("Use Logarithmic Accumulation", &m_Renderer.GetSettings().LogAccumulation);
		if (IG::Button("Reset Buffer")) 
		{
			m_Renderer.ResetFrameIndex();
		}

		IG::Text("");
		IG::Text("--- Renderer Settings:");
		IG::Checkbox("Gamma Correction", &m_Renderer.GetSettings().GammaCorrection);
		IG::Checkbox("Allow Transparency", &m_Renderer.GetSettings().AllowTransparency);
		IG::Text("Bounces per Ray");
		IG::SliderInt("##dd", &m_Renderer.GetSettings().BouncesPerRay, 1, 8);

		IG::Text("");
		IG::Text("--- Shading Settings:");
		IG::Checkbox("Soft Shadows", &m_Renderer.GetSettings().SoftShadows);
		IG::Text("Shadow Samples");
		IG::SliderInt("##d", &m_Renderer.GetSettings().ShadowSamples, 1, 4);

		IG::End();

		//Set Viewport for rendered image
		IG::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		IG::Begin("Viewport");
		m_ViewportWidth = IG::GetContentRegionAvail().x;
		m_ViewportHeight = IG::GetContentRegionAvail().y;

		auto image = m_Renderer.GetFinalImage();

		if (image)
			//IG::Image(image->GetDescriptorSet(), { (float)image->GetWidth(), (float)image->GetHeight() });
			IG::Image(image->GetDescriptorSet(), { (float)image->GetWidth(), (float)image->GetHeight() }, 
					ImVec2(0,1), ImVec2(1, 0));

		IG::End();
		IG::PopStyleVar();

		//IG::ShowDemoWindow();
		CallRenderer();
	}

	void CallRenderer()
	{
		WN::Timer timer;

		m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_Camera.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_Renderer.Render(m_Scene, m_Camera);

		m_LastRenderTime = timer.ElapsedMillis();
	}
private:
	Renderer m_Renderer;
	Camera m_Camera;
	Scene m_Scene;
	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
	float m_LastRenderTime = 0.0f;
};

WN::Application* WN::CreateApplication(int argc, char** argv)
{
	WN::ApplicationSpecification spec;
	spec.Name = "Walnut Raytracer";

	WN::Application* app = new WN::Application(spec);
	app->PushLayer<ExampleLayer>();
	app->SetMenubarCallback([app]()
		{
			if (IG::BeginMenu("File"))
			{
				if (IG::MenuItem("Exit"))
				{
					app->Close();
				}
				IG::EndMenu();
			}
		});
	return app;
}