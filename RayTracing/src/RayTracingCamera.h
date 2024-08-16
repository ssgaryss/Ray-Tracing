#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

class RayTracingCamera final
{
public:
	enum class CameraProjectionMode {
		Othographic = 0,
		Perspective = 1
	};
public:
	RayTracingCamera();

	void onUpdate(float vTimestep);

	void setViewportSize(float vWidth, float vHeight);
	void setPosition(const glm::vec3& vPosition);
	void setFocalPoint(const glm::vec3& vFocalPoint);

	inline bool isOthograhic() const { return m_ProjectionMode == CameraProjectionMode::Othographic; }
	inline const glm::vec3& getPosition() const { return m_Position; }
	inline float getPitch() const { return m_Pitch; }
	inline float getYaw() const { return m_Yaw; }
	inline glm::vec3 getUpDirection() const { return glm::rotate(getOrientation(), glm::vec3{ 0.0f, 1.0f, 0.0f }); }
	inline glm::vec3 getRightDirection() const { return glm::rotate(getOrientation(), glm::vec3{ 1.0f, 0.0f, 0.0f }); }
	inline glm::vec3 getForwardDirection() const { return glm::rotate(getOrientation(), glm::vec3{ 0.0f, 0.0f, -1.0f }); } // 满足OpenGL习惯，相机看向-z
	inline glm::quat getOrientation() const { return glm::quat(glm::radians(glm::vec3(-m_Pitch, -m_Yaw, 0.0f))); } // 将世界坐标系转换到相机坐标系的四元数
	inline const std::vector<glm::vec3>& getRayDirections() const { return m_RayDirections; }
private:
	void updateCameraInverseViewMatrix();
	void updateCameraInverseProjectionMatrix();
	void updateRayDirections();
	inline void updatePosition() { m_Position = m_FocalPoint - getForwardDirection() * m_Distance; }
	inline void updateFocalPoint() { m_FocalPoint = m_Position + getForwardDirection() * m_Distance; }

	void onKeyMove(float vTimestep);                       // w,a,s,d任意移动 
	void onMousePan(const glm::vec2& vDelta);                 // 横向移动
	void onMouseRotate(const glm::vec2& vDelta);              // 旋转,改变m_Yaw,m_Pitch
	void onMouseZoom(float vDelta);                           // 改变Distance

	std::pair<float, float> getPanSpeed() const;     // 横向移动速度（x方向，y方向）
	float getRotationSpeed() const;                  // Yaw, Pitch 的速度
	float getMoveSpeed() const;                      // Move速度
	float getZoomSpeed() const;                      // Zoom速度,随着m_Distance值变化而变化(相机位置越接近光心速度越小)
private:
	// Mouse相关
	glm::vec2 m_MousePosition = { 0.0f, 0.0f };      // 记录上一个时刻鼠标位置
	// Viewport相关
	float m_ViewportWidth = 1000.0f, m_ViewportHeight = 1000.0f;
	// 外参
	glm::vec3 m_Position = { 0.0f, 0.0f, 10.0f };    // 由m_FocalPoint和m_Distance计算得出,可由外部控制器改变位置
	glm::vec3 m_FocalPoint = { 0.0f, 0.0f, 0.0f };   // 光心
	float m_Distance = 10.0f;                        // m_FocalPoint沿ForwardDirection方向距离m_Position的距离,最小为1.0f
	float m_Pitch = 0.0f, m_Yaw = 0.0f;              // 角度制(这里我就不要roll了感觉用不到)
	glm::mat4 m_InverseViewMatrix = glm::mat4(1.0f);

	// 内参
	CameraProjectionMode m_ProjectionMode = CameraProjectionMode::Perspective;
	glm::mat4 m_InverseProjectionMatrix = glm::mat4(1.0f);
	float m_AspectRatio = 1.0f;
	// Othographic
	float m_OthographicSize = 10.0f; // 视口 Height / 2
	float m_OthographicNear = 0.1f, m_OthographicFar = 100.0f;
	// Perspective
	float m_PerspectiveFOV = 90.0f;  // Zoom我没有通过改变FOV实现，而是改变m_Distance间接改变摄像机位置来实现类似效果 
	float m_PerspectiveNear = 0.1f, m_PerspectiveFar = 100.0f;
	// Ray Tracing
	std::vector<glm::vec3> m_RayDirections;
};
