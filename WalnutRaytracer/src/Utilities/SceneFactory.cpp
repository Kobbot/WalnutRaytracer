#include "SceneFactory.h"

namespace SceneFactory {
	Scene createScene() {

		Scene scene;
		//The way this is done, another function would not need to know the index. I could make it it's own function.
		Material& mainSphere = scene.Materials.emplace_back();
		mainSphere.Albedo = { 1.0f, 1.0f, 0.0f };
		mainSphere.Roughness = 0.6f;
		mainSphere.Reflectivity = 0.4f;

		Material& smallSphere = scene.Materials.emplace_back();
		smallSphere.Albedo = { 0.0f, 1.0f, 1.0f };
		smallSphere.Roughness = 0.2f;
		smallSphere.Reflectivity = 0.6f;

		Material& groundSphere = scene.Materials.emplace_back();
		groundSphere.Albedo = { 0.4f, 1.0f, 0.4f };
		groundSphere.Roughness = 0.1f;
		groundSphere.Reflectivity = 0.4f;

		Material& groundPlane = scene.Materials.emplace_back();
		groundPlane.Albedo = { 1.0f, 0.2f, 0.0f };
		groundPlane.Roughness = 0.1f;
		groundPlane.Reflectivity = 0.4f;

		Material& shadowedSphere = scene.Materials.emplace_back();
		shadowedSphere.Albedo = { 0.0f, 1.0f, 0.0f };
		shadowedSphere.Roughness = 0.8f;
		shadowedSphere.Reflectivity = 0.4f;

		Material& ceilingPlane = scene.Materials.emplace_back();
		ceilingPlane.Albedo = { 0.2f, 0.0f, 0.4f };
		ceilingPlane.Roughness = 0.8f;
		ceilingPlane.Reflectivity = 0.4f;

		Material& leftWall = scene.Materials.emplace_back();
		leftWall.Albedo = { 0.2f, 1.0f, 0.4f };
		leftWall.Roughness = 0.02f;
		leftWall.Reflectivity = 0.98f;

		Material& rightWall = scene.Materials.emplace_back();
		rightWall.Albedo = { 0.2f, 0.4f, 1.0f };
		rightWall.Roughness = 0.8f;
		rightWall.Reflectivity = 0.4f;

		Material& backwall = scene.Materials.emplace_back();
		backwall.Albedo = { 1.0f, 1.0f, 1.0f };
		backwall.Roughness = 0.6f;
		backwall.Reflectivity = 0.3f;

		Material& mirrorSphere = scene.Materials.emplace_back();
		mirrorSphere.Albedo = { .8f, .8f, .8f };
		mirrorSphere.Roughness = 0.01f;
		mirrorSphere.Reflectivity = 0.99f;

		Material& glassSphere = scene.Materials.emplace_back();
		glassSphere.Albedo = { .2f, .2f, 1.0f };
		glassSphere.Roughness = 0.4f;
		glassSphere.Reflectivity = 0.4f;
		glassSphere.Transparency = 0.2f;
		glassSphere.Refractivity = 1.06f;

		Material& sunSphere = scene.Materials.emplace_back();
		sunSphere.Albedo = { 1.0f, 1.0f, 1.0f };
		sunSphere.Roughness = 0.95f;
		sunSphere.Reflectivity = 0.3f;

		Material& triangle = scene.Materials.emplace_back();
		triangle.Albedo = { 1.0f, 0.8f, 0.8f };
		triangle.Roughness = 0.9f;
		triangle.Reflectivity = 0.4f;

		{
			glm::vec3 position = glm::vec3({ 0.0f, 0.0f, 0.0f });
			float radius = 0.5f;
			int matIndex = 0;

			scene.Spheres.push_back(Sphere{ position, matIndex, radius });
		}

		{
			glm::vec3 position = glm::vec3({ -0.6f, 1.2f, -0.4f });
			float radius = 0.4f;
			int matIndex = 9;

			scene.Spheres.push_back(Sphere{ position, matIndex, radius });
		}

		{

			glm::vec3 position = glm::vec3({ 0.7f, 0.7f, 0.7f });
			float radius = 0.1f;
			int matIndex = 1;

			scene.Spheres.push_back(Sphere{ position, matIndex, radius });
		}

		{

			glm::vec3 position = glm::vec3({ -0.7f, -0.3f, 0.2f });
			float radius = 0.2f;
			int matIndex = 4;

			scene.Spheres.push_back(Sphere{ position, matIndex, radius });
		}

		{

			glm::vec3 position = glm::vec3({ 0.0f,-0.5f,0.0f });
			glm::vec3 normal = glm::normalize(glm::vec3({ 0.0f, 1.0f, 0.0f }));
			int matIndex = 3;

			scene.Planes.push_back(Plane{ position, matIndex, normal });
		}

		{

			glm::vec3 position = glm::vec3({ 0.0f, 2.5f, 0.0f });
			glm::vec3 normal = glm::normalize(glm::vec3({ 0.0f, -1.0f, 0.0f }));
			int matIndex = 5;

			scene.Planes.push_back(Plane{ position, matIndex, normal });
		}

		{

			glm::vec3 position = glm::vec3({ -1.5f, 0.0f, 0.0f });
			glm::vec3 normal = glm::normalize(glm::vec3({ 1.0f, 0.0f, 0.0f }));
			int matIndex = 6;

			scene.Planes.push_back(Plane{ position, matIndex, normal });
		}

		{

			glm::vec3 position = glm::vec3({ 4.0f, 0.0f, 0.0f });
			glm::vec3 normal = glm::normalize(glm::vec3({ -1.0f, 0.0f, 0.0f }));
			int matIndex = 7;

			scene.Planes.push_back(Plane{ position, matIndex, normal });
		}

		{

			glm::vec3 position = glm::vec3({ 0.0f, 0.0f, -1.5f });
			glm::vec3 normal = glm::normalize(glm::vec3({ 0.0f, 0.0f, 1.0f }));
			int matIndex = 8;

			scene.Planes.push_back(Plane{ position, matIndex, normal });
		}

		{

			glm::vec3 position = glm::vec3({ 0.0f, 0.0f, 6.0f });
			glm::vec3 normal = glm::normalize(glm::vec3({ 0.0f, 0.0f, -1.0f }));
			int matIndex = 8;

			scene.Planes.push_back(Plane{ position, matIndex, normal });
		}

		{

			glm::vec3 position = glm::vec3({ 2.0f, -0.1f, 2.0f });
			float radius = 0.4f;
			int matIndex = 10; //Glass ball

			scene.Spheres.push_back(Sphere{ position, matIndex, radius });
		}

		{

			glm::vec3 position = glm::vec3({ 2.25f, -0.3f, 1.15f });
			float radius = 0.2f;
			int matIndex = 4;

			scene.Spheres.push_back(Sphere{ position, matIndex, radius });
		}

		{	//TEST TRIANGLE: Made of 4 triangle faces.

			glm::vec3 position = glm::vec3({ 0.0f, 1.0f, 4.0f });
			int matIndex = 12;

			glm::vec3 p1 = glm::vec3({ 0.0f, 0.0f, 3.5f });
			glm::vec3 p2 = glm::vec3({ -0.5f, 1.5f, 4.0f });
			glm::vec3 p3 = glm::vec3({ 0.5f, 1.5f, 4.0f });
			glm::vec3 p4 = glm::vec3({ 0.0f, 1.5f, 3.0f });

			{

				glm::vec3 l1 = p1 - p2;
				glm::vec3 l2 = p2 - p3;

				glm::vec3 n = -glm::normalize(glm::cross(l1, l2));

				scene.Triangles.push_back(Triangle{ position, matIndex, p1, p2, p3, n });
			}

			{
				glm::vec3 l1 = p1 - p2;
				glm::vec3 l2 = p1 - p4;

				glm::vec3 n = -glm::normalize(glm::cross(l1, l2));

				scene.Triangles.push_back(Triangle{ position, matIndex, p1, p2, p4, n });

			}

			{
				glm::vec3 l1 = p1 - p3;
				glm::vec3 l2 = p1 - p4;

				glm::vec3 n = -glm::normalize(glm::cross(l1, l2));

				scene.Triangles.push_back(Triangle{ position, matIndex, p1, p3, p4, n });

			}

			{
				glm::vec3 l1 = p2 - p3;
				glm::vec3 l2 = p2 - p4;

				glm::vec3 n = -glm::normalize(glm::cross(l1, l2));

				scene.Triangles.push_back(Triangle{ position, matIndex, p2, p3, p4, n });
			}
		}

		GlobalLight gl;
		gl.Intensity = 0.8f;
		gl.Direction = glm::normalize(glm::vec3({ 0.8f, -1.0f, -1.0f }));
		scene.GlobalLight = gl;

		SphereLight& sl = scene.SphereLights.emplace_back();
		sl.Intensity = 12.0f;
		Sphere sl_Sphere{ { 1.4f, 1.9f, 1.5f } , 11, 0.2f };
		sl.Sphere = sl_Sphere;

		//PointLight& pl1 = m_Scene.PointLights.emplace_back();
		//pl1.Intensity = 12.0f;
		//pl1.Position = glm::vec3({ 1.4f, 1.9f, 1.5f });
		//
		return scene;
	}
}
