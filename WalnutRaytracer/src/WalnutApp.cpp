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
		: m_Camera(45.0f, 0.1f, 100.f) 
	{

		//The way this is done, another function would not need to know the index. I could make it it's own function.
		Material& mainSphere = m_Scene.Materials.emplace_back();
		mainSphere.Albedo = { 1.0f, 1.0f, 0.0f };
		mainSphere.Roughness = 0.6f;
		mainSphere.Reflectivity = 0.4f;

		Material& smallSphere = m_Scene.Materials.emplace_back();
		smallSphere.Albedo = { 0.0f, 1.0f, 1.0f };
		smallSphere.Roughness = 0.2f;
		smallSphere.Reflectivity = 0.6f;

		Material& groundSphere = m_Scene.Materials.emplace_back();
		groundSphere.Albedo = { 0.4f, 1.0f, 0.4f };
		groundSphere.Roughness = 0.1f;
		groundSphere.Reflectivity = 0.4f;

		Material& groundPlane = m_Scene.Materials.emplace_back();
		groundPlane.Albedo = { 1.0f, 0.2f, 0.0f };
		groundPlane.Roughness = 0.1f;
		groundPlane.Reflectivity = 0.4f;

		Material& shadowedSphere = m_Scene.Materials.emplace_back();
		shadowedSphere.Albedo = { 0.0f, 1.0f, 0.0f };
		shadowedSphere.Roughness = 0.8f;
		shadowedSphere.Reflectivity = 0.4f;

		Material& ceilingPlane = m_Scene.Materials.emplace_back();
		ceilingPlane.Albedo = { 0.2f, 0.0f, 0.4f };
		ceilingPlane.Roughness = 0.8f;
		ceilingPlane.Reflectivity = 0.4f;

		Material& leftWall = m_Scene.Materials.emplace_back();
		leftWall.Albedo = { 0.2f, 1.0f, 0.4f };
		leftWall.Roughness = 0.02f;
		leftWall.Reflectivity = 0.98f;

		Material& rightWall = m_Scene.Materials.emplace_back();
		rightWall.Albedo = { 0.2f, 0.4f, 1.0f };
		rightWall.Roughness = 0.8f;
		rightWall.Reflectivity = 0.4f;

		Material& backwall = m_Scene.Materials.emplace_back();
		backwall.Albedo = { 1.0f, 1.0f, 1.0f };
		backwall.Roughness = 0.6f;
		backwall.Reflectivity = 0.3f;

		Material& mirrorSphere = m_Scene.Materials.emplace_back();
		mirrorSphere.Albedo = { .8f, .8f, .8f };
		mirrorSphere.Roughness = 0.01f;
		mirrorSphere.Reflectivity = 0.99f;

		Material& glassSphere = m_Scene.Materials.emplace_back();
		glassSphere.Albedo = { .2f, .2f, 1.0f };
		glassSphere.Roughness = 0.8f;
		glassSphere.Reflectivity = 0.2f;
		glassSphere.Transparency = 0.4f;

		Material& sunSphere = m_Scene.Materials.emplace_back();
		sunSphere.Albedo = { 1.0f, 1.0f, 1.0f };
		sunSphere.Roughness = 0.95f;
		sunSphere.Reflectivity = 0.3f;

		{
			glm::vec3 position = glm::vec3({ 0.0f, 0.0f, 0.0f });
			float radius = 0.5f;
			int matIndex = 0;

			m_Scene.Spheres.push_back(Sphere{ position, matIndex, radius });
		}

		{
			glm::vec3 position = glm::vec3({ -0.6f, 1.2f, -0.4f });
			float radius = 0.4f;
			int matIndex = 9;

			m_Scene.Spheres.push_back(Sphere{ position, matIndex, radius });
		}

		{

			glm::vec3 position = glm::vec3({ 0.7f, 0.7f, 0.7f });
			float radius = 0.1f;
			int matIndex = 1;

			m_Scene.Spheres.push_back(Sphere{ position, matIndex, radius });
		}

		{

			glm::vec3 position = glm::vec3({ -0.7f, -0.3f, 0.2f });
			float radius = 0.2f;
			int matIndex = 4;

			m_Scene.Spheres.push_back(Sphere{ position, matIndex, radius });
		}

		{

			glm::vec3 position = glm::vec3({ 0.0f,-0.5f,0.0f });
			glm::vec3 normal = glm::normalize(glm::vec3({ 0.0f, 1.0f, 0.0f }));
			int matIndex = 3;

			m_Scene.Planes.push_back(Plane{ position, matIndex, normal });
		}

		{

			glm::vec3 position = glm::vec3({ 0.0f, 2.5f, 0.0f });
			glm::vec3 normal = glm::normalize(glm::vec3({ 0.0f, -1.0f, 0.0f }));
			int matIndex = 5;

			m_Scene.Planes.push_back(Plane{ position, matIndex, normal });
		}

		{

			glm::vec3 position = glm::vec3({ -1.5f, 0.0f, 0.0f });
			glm::vec3 normal = glm::normalize(glm::vec3({ 1.0f, 0.0f, 0.0f }));
			int matIndex = 6;

			m_Scene.Planes.push_back(Plane{ position, matIndex, normal });
		}

		{

			glm::vec3 position = glm::vec3({ 4.0f, 0.0f, 0.0f });
			glm::vec3 normal = glm::normalize(glm::vec3({ -1.0f, 0.0f, 0.0f }));
			int matIndex = 7;

			m_Scene.Planes.push_back(Plane{ position, matIndex, normal });
		}

		{

			glm::vec3 position = glm::vec3({ 0.0f, 0.0f, -1.5f });
			glm::vec3 normal = glm::normalize(glm::vec3({ 0.0f, 0.0f, 1.0f }));
			int matIndex = 8;

			m_Scene.Planes.push_back(Plane{ position, matIndex, normal });
		}

		{

			glm::vec3 position = glm::vec3({ 0.0f, 0.0f, 6.0f });
			glm::vec3 normal = glm::normalize(glm::vec3({ 0.0f, 0.0f, -1.0f }));
			int matIndex = 8;

			m_Scene.Planes.push_back(Plane{ position, matIndex, normal });
		}

		{

			glm::vec3 position = glm::vec3({ 2.8f, -0.1f, -0.4f });
			float radius = 0.4f;
			int matIndex = 10;

			m_Scene.Spheres.push_back(Sphere{ position, matIndex, radius });
		}

		{
			//TEST TRIANGLE: It works :3 
			glm::vec3 position = glm::vec3({ 0.0f, 1.0f, 0.0f });
			int matIndex = 1;

			glm::vec3 p1 = glm::vec3({ 0.0f, 0.5f, 0.0f });
			glm::vec3 p2 = glm::vec3({ -0.5f, 1.0f, 0.0f });
			glm::vec3 p3 = glm::vec3({ 0.5f, 1.0f, 0.0f });
			glm::vec n = glm::vec3({ 0.0f, 0.0f, 1.0f });

			m_Scene.Triangles.push_back(Triangle{ position, matIndex, p1, p2, p3, n });
		}

		GlobalLight gl;
		gl.Intensity = 0.8f;
		gl.Direction = glm::normalize(glm::vec3({ 0.8f, -1.0f, -1.0f }));
		m_Scene.GlobalLight = gl;

		SphereLight& sl = m_Scene.SphereLights.emplace_back();
		sl.Intensity = 12.0f;
		Sphere sl_Sphere{ { 1.4f, 1.9f, 1.5f } , 11, 0.2f };
		sl.Sphere = sl_Sphere;

		//PointLight& pl1 = m_Scene.PointLights.emplace_back();
		//pl1.Intensity = 12.0f;
		//pl1.Position = glm::vec3({ 1.4f, 1.9f, 1.5f });
		//
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

		IG::Text("--- Path Tracing:");
		IG::Checkbox("Accumulation", &m_Renderer.GetSettings().Accumulate);

		if (IG::Button("Reset Buffer")) 
		{
			m_Renderer.ResetFrameIndex();
		}


		IG::Text("--- Renderer Settings:");
		IG::Checkbox("Gamma Correction", &m_Renderer.GetSettings().GammaCorrection);
		IG::Checkbox("Soft Shadows", &m_Renderer.GetSettings().SoftShadows);

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