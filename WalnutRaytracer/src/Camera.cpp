#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Walnut/Input/Input.h"

namespace WN = Walnut;

Camera::Camera(float verticalFOV, float nearClip, float farClip)
	: m_VerticalFOV(verticalFOV), m_NearClip(nearClip), m_FarClip(farClip)
{
	m_ForwardDirection = glm::vec3(0, 0, -1);
	m_Position = glm::vec3(0, 0, 3);
}

bool Camera::OnUpdate(float ts) 
{
	const float mouseSensitivity = 0.002f; //Pretty arbitrary
	glm::vec2 mousePos = WN::Input::GetMousePosition();
	glm::vec2 delta = (mousePos - m_LastMousePosition) * mouseSensitivity;
	m_LastMousePosition = mousePos;

	if (!WN::Input::IsMouseButtonDown(WN::MouseButton::Right))
	{
		WN::Input::SetCursorMode(WN::CursorMode::Normal);
		return false;
	}

	WN::Input::SetCursorMode(WN::CursorMode::Locked);

	bool moved = false;

	constexpr glm::vec3 upDirection(0.0f, 1.0f, 0.0f);
	glm::vec3 rightDirection = glm::cross(m_ForwardDirection, upDirection);
	

	float speed = 5.0f;

	//Movement
	if (WN::Input::IsKeyDown(WN::KeyCode::W)) {
		m_Position += m_ForwardDirection * ts * speed;
		moved = true;
	}
	else if (WN::Input::IsKeyDown(WN::KeyCode::S)) {
		m_Position -= m_ForwardDirection * ts * speed;
		moved = true;
	}
	if (WN::Input::IsKeyDown(WN::KeyCode::A)) {
		m_Position -= rightDirection * ts * speed;
		moved = true;
	}
	else if (WN::Input::IsKeyDown(WN::KeyCode::D)) {
		m_Position += rightDirection * ts * speed;
		moved = true;
	}
	if (WN::Input::IsKeyDown(WN::KeyCode::Q)) {
		m_Position -= upDirection * ts * speed;
		moved = true;
	}
	else if (WN::Input::IsKeyDown(WN::KeyCode::E)) {
		m_Position += upDirection * ts * speed;
		moved = true;
	}

	//Rotation
	if (delta.x != 0.0f || delta.y != 0.0f)
	{
		float pitchDelta = delta.y * GetRotationSpeed();
		float yawDelta = delta.x * GetRotationSpeed();

		//Calculating a new direction taking the pitch and yaw delta into account
		glm::quat q = glm::normalize(glm::cross(glm::angleAxis(-pitchDelta, rightDirection),
			glm::angleAxis(-yawDelta, glm::vec3(0.0f, 1.0f, 0.0f))));
		m_ForwardDirection = glm::rotate(q, m_ForwardDirection);

		moved = true;
	}

	if (moved)
	{
		RecalculateView();
		RecalculateRayDirections();
	}
	return moved;
}

void Camera::OnResize(uint32_t width, uint32_t height)
{
	if (width == m_ViewportWidth && height == m_ViewportHeight)
		return;
	
	m_ViewportWidth = width;
	m_ViewportHeight = height;

	RecalculateProjection();
	RecalculateRayDirections();
}

float Camera::GetRotationSpeed() 
{
	return m_RotationSpeed;
}

void Camera::RecalculateProjection() 
{
	//Perspective matrix based on the field of view
	m_Projection = glm::perspectiveFov(glm::radians(m_VerticalFOV),
		(float)m_ViewportWidth, (float)m_ViewportHeight,
		m_NearClip, m_FarClip);

	m_InverseProjection = glm::inverse(m_Projection);
}

void Camera::RecalculateView() {
	m_View = glm::lookAt(m_Position, m_Position + m_ForwardDirection, glm::vec3(0, 1, 0));
	m_InverseView = glm::inverse(m_View);
}

void Camera::RecalculateRayDirections() {
	m_RayDirections.resize(m_ViewportWidth * m_ViewportHeight);

	for (uint32_t y = 0; y < m_ViewportHeight; y++)
	{
		for (uint32_t x = 0; x < m_ViewportWidth; x++) 
		{
			glm::vec2 coord = { (float)x / (float)m_ViewportWidth, (float)y / (float)m_ViewportHeight };
			coord = coord * 2.0f - 1.0f; // Space of -1 to 1

			glm::vec4 target = m_InverseProjection * glm::vec4(coord.x, coord.y, 1, 1);
			glm::vec3 rayDirection = glm::vec3(m_InverseView * glm::vec4(glm::normalize(glm::vec3(target) / target.w), 0)); // World Space
			m_RayDirections[x + y * m_ViewportWidth] = rayDirection;
		}
	}
}