#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Random.h"
#include "Walnut/Timer.h"

namespace IG = ImGui;
namespace WN = Walnut;

class ExampleLayer : public WN::Layer
{
public:
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

		if (m_Image)
			IG::Image(m_Image->GetDescriptorSet(), { (float)m_Image->GetWidth(),(float)m_Image->GetHeight() });

		IG::End();
		IG::PopStyleVar();

		//IG::ShowDemoWindow();
		Render();
	}

	void Render()
	{
		WN::Timer timer;

		if (!m_Image || m_ViewportWidth != m_Image->GetWidth() || m_ViewportHeight != m_Image->GetHeight())
		{
			m_Image = std::make_shared<WN::Image>(m_ViewportWidth, m_ViewportHeight, WN::ImageFormat::RGBA);
			delete[] m_ImageData;
			m_ImageData = new uint32_t[m_ViewportWidth * m_ViewportHeight];
		}

		for (uint32_t i = 0; i < m_ViewportWidth * m_ViewportHeight; i++) {
			m_ImageData[i] = WN::Random::UInt();
			m_ImageData[i] |= 0xff000000;
		}

		m_Image->SetData(m_ImageData);

		m_LastRenderTime = timer.ElapsedMillis();
	}
private:
	std::shared_ptr<WN::Image> m_Image;
	uint32_t* m_ImageData = nullptr;
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