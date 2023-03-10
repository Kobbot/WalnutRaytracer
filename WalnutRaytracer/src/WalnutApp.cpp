#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Random.h"
#include "Walnut/Timer.h"

#include "Renderer.h"
#include "Camera.h"

namespace IG = ImGui;
namespace WN = Walnut;

class ExampleLayer : public WN::Layer
{
public:
	ExampleLayer()
		: m_Camera(45.0f, 0.1f, 100.f) {}
	virtual void OnUpdate(float ts) override 
	{
		m_Camera.OnUpdate(ts);
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
		m_Renderer.Render(m_Camera);

		m_LastRenderTime = timer.ElapsedMillis();
	}
private:
	Renderer m_Renderer;
	Camera m_Camera;
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