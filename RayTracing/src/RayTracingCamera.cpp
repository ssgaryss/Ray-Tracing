#include "RayTracingCamera.h"
#include <algorithm>
#include <Walnut/Input/Input.h>


RayTracingCamera::RayTracingCamera()
{
	updateCameraInverseViewMatrix();
	updateCameraInverseProjectionMatrix();
	updateRayDirections();
}


bool RayTracingCamera::onUpdate(float vTimestep)
{
	bool Moved = false;
	// TODO : 这里并没有用Shortcut无法更改按键
	if (Walnut::Input::IsKeyDown(Walnut::KeyCode::LeftAlt) || Walnut::Input::IsKeyDown(Walnut::KeyCode::RightAlt)) {
		const glm::vec2 MousePosition = Walnut::Input::GetMousePosition();
		glm::vec2 Delta = (MousePosition - m_MousePosition) * 0.03f;
		m_MousePosition = MousePosition;
		if (Walnut::Input::IsMouseButtonDown(Walnut::MouseButton::Left))
			onMouseRotate(Delta), Moved = true;
		if (Walnut::Input::IsMouseButtonDown(Walnut::MouseButton::Middle))
			onMousePan(Delta), Moved = true;
		if (Walnut::Input::IsMouseButtonDown(Walnut::MouseButton::Right))
			onMouseZoom(Delta.y), Moved = true;
		updatePosition();       // 注意更新m_Position
	}
	if (Walnut::Input::IsMouseButtonDown(Walnut::MouseButton::Right)) {
		const glm::vec2 MousePosition = Walnut::Input::GetMousePosition();
		glm::vec2 Delta;
		Delta.x = std::clamp(MousePosition.x - m_MousePosition.x, -50.0f, 50.0f) * 0.03f;
		Delta.y = std::clamp(MousePosition.y - m_MousePosition.y, -50.0f, 50.0f) * 0.03f;
		m_MousePosition = MousePosition;
		onMouseRotate(Delta);
		onKeyMove(vTimestep);   // 注意更新m_FocalPoint
		updateFocalPoint();
		Moved = true;
	}
	if (Moved) {
		updateCameraInverseViewMatrix();
		updateRayDirections();
	}
	return Moved;
}

void RayTracingCamera::setViewportSize(float vWidth, float vHeight)
{
	m_AspectRatio = vWidth / vHeight;
	m_ViewportWidth = vWidth;
	m_ViewportHeight = vHeight;
	updateRayDirections();
	updateCameraInverseProjectionMatrix();
}

void RayTracingCamera::setPosition(const glm::vec3& vPosition)
{
	m_Position = vPosition;
	updateFocalPoint();
}

void RayTracingCamera::setFocalPoint(const glm::vec3& vFocalPoint)
{
	m_FocalPoint = vFocalPoint;
	updatePosition();
}

void RayTracingCamera::updateCameraInverseViewMatrix()
{
	m_InverseViewMatrix = glm::inverse(glm::lookAt(m_Position, m_FocalPoint, getUpDirection()));
}

void RayTracingCamera::updateCameraInverseProjectionMatrix()
{
	switch (m_ProjectionMode)
	{
	case CameraProjectionMode::Othographic:
	{
		float vLeft = -m_OthographicSize * m_AspectRatio;
		float vRight = m_OthographicSize * m_AspectRatio;
		float vBottom = -m_OthographicSize;
		float vTop = m_OthographicSize;
		m_InverseProjectionMatrix = glm::inverse(glm::ortho(vLeft, vRight, vBottom, vTop, m_OthographicNear, m_OthographicFar));
		return;
	}
	case CameraProjectionMode::Perspective:
	{
		m_InverseProjectionMatrix = glm::inverse(glm::perspective(glm::radians(m_PerspectiveFOV), m_AspectRatio, m_PerspectiveNear, m_PerspectiveFar));
		return;
	}
	}
}

void RayTracingCamera::updateRayDirections()
{
	m_RayDirections.resize((size_t)m_ViewportWidth * (size_t)m_ViewportHeight);
	for (uint32_t y = 0; y < m_ViewportHeight; ++y) {
		for (uint32_t x = 0; x < m_ViewportWidth; ++x) {
			glm::vec2 Coordinate = { (float)x / (float)m_ViewportWidth, (float)y / m_ViewportHeight };
			Coordinate = Coordinate * 2.0f - 1.0f; // (-1, 1)
			switch (m_ProjectionMode)
			{
			case RayTracingCamera::CameraProjectionMode::Othographic:
			{
				glm::vec3 RayDirection = m_InverseProjectionMatrix * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
				m_RayDirections[(size_t)(y * m_ViewportWidth + x)] = RayDirection;
				break;
			}
			case RayTracingCamera::CameraProjectionMode::Perspective:
			{
				glm::vec4 Target = m_InverseProjectionMatrix * glm::vec4(Coordinate, m_PerspectiveNear, 1.0f);
				glm::vec3 RayDirection = m_InverseViewMatrix * glm::vec4(glm::normalize(glm::vec3(Target) / Target.w), 0); // 将光线方向从相机坐标转换到世界坐标
				m_RayDirections[(size_t)(y * m_ViewportWidth + x)] = RayDirection;
				break;
			}
			}
		}
	}
}

void RayTracingCamera::onKeyMove(float vTimestep)
{
	if (Walnut::Input::IsKeyDown(Walnut::KeyCode::W))
		m_Position += getForwardDirection() * getMoveSpeed();
	if (Walnut::Input::IsKeyDown(Walnut::KeyCode::A))
		m_Position += -getRightDirection() * getMoveSpeed();
	if (Walnut::Input::IsKeyDown(Walnut::KeyCode::S))
		m_Position += -getForwardDirection() * getMoveSpeed();
	if (Walnut::Input::IsKeyDown(Walnut::KeyCode::D))
		m_Position += getRightDirection() * getMoveSpeed();
	if (Walnut::Input::IsKeyDown(Walnut::KeyCode::Q))
		m_Position += -getUpDirection() * getMoveSpeed();
	if (Walnut::Input::IsKeyDown(Walnut::KeyCode::E))
		m_Position += getUpDirection() * getMoveSpeed();
}

void RayTracingCamera::onMousePan(const glm::vec2& vDelta)
{
	auto [XSpeed, YSpeed] = getPanSpeed();
	m_FocalPoint += -getRightDirection() * vDelta.x * XSpeed * m_Distance; // 离得越远移动绝对距离也要越大
	m_FocalPoint += getUpDirection() * vDelta.y * YSpeed * m_Distance;
}

void RayTracingCamera::onMouseRotate(const glm::vec2& vDelta)
{
	float YawDirection = getUpDirection().y < 0 ? -1.0f : 1.0f;  // 当头朝下的时候Yaw旋转的绝对方向反转了（保持相对方向不变）
	m_Yaw += 5.0f * YawDirection * getRotationSpeed() * vDelta.x;
	m_Pitch += 5.0f * getRotationSpeed() * vDelta.y;
}

void RayTracingCamera::onMouseZoom(float vDelta)
{
	m_Distance -= vDelta * getZoomSpeed();
	if (m_Distance < 1.0f) {
		m_FocalPoint += getForwardDirection();
		m_Distance = 1.0f;
	}
}

std::pair<float, float> RayTracingCamera::getPanSpeed() const
{
	float XFactor = std::min(m_ViewportWidth / 1000.0f, 2.4f);
	float YFactor = std::min(m_ViewportHeight / 1000.0f, 2.4f);
	float XSpeed = 0.0366f * (XFactor * XFactor) - 0.1778f * XFactor + 0.3021f;
	float YSpeed = 0.0366f * (YFactor * YFactor) - 0.1778f * YFactor + 0.3021f; // Magic Number !
	return std::make_pair(XSpeed, YSpeed);
}

float RayTracingCamera::getRotationSpeed() const
{
	return 0.8f;
}

float RayTracingCamera::getMoveSpeed() const
{
	return 0.5f;
}

float RayTracingCamera::getZoomSpeed() const
{
	float Distance = std::max(m_Distance * 0.2f, 0.0f);
	return std::min(Distance * Distance, 100.0f);
}
