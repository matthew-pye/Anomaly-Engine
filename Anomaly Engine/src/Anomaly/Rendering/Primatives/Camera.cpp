#include "aepch.h"
#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Anomaly
{
//Orthographic Camera ----------------------------------------------------------------------------------------------------------------------------------------------
	OrthoCamera::OrthoCamera(float left, float right, float bottom, float top, float nearZ, float farZ)
		: m_ProjMatrix(glm::ortho(left, right, bottom, top, nearZ, farZ)), m_ViewMatrix(1.0f)
	{
		m_ViewProjMatrix = m_ProjMatrix * m_ViewMatrix;	
	}
	
	void OrthoCamera::RecalcuateViewProjMatrix()
	{
		//Adds the cameras position to the transform
		glm::mat4 Transform = glm::translate(glm::mat4(1.0f), m_CamPos) * glm::rotate(glm::mat4(1.0f), glm::radians(m_CamRotationZ), glm::vec3(0, 0, 1));
		//Adds the cameras rotation to the transform
		//Transform = glm::rotate(Transform, glm::radians(m_CamRotationZ), glm::vec3(0, 0, 1));

		m_ViewMatrix = glm::inverse(Transform);
		m_ViewProjMatrix = m_ProjMatrix * m_ViewMatrix;
	}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------

//Perspective Camera -----------------------------------------------------------------------------------------------------------------------------------------------
	PerspecCamera::PerspecCamera(float FOV, float width, float height, float nearValue, float farValue)
	{
		m_ProjMatrix = glm::perspective(glm::radians(FOV), width/height, nearValue, farValue) * glm::mat4(-1.0f);
		m_ViewMatrix = glm::mat4(1.0f);
		
		m_CamPos =	glm::vec3(0.0f, 0.0f,  3.0f);
		m_CamUp = glm::vec3(0.0f, 1.0f, 0.0f);
		m_CamFront = glm::vec3( 0.0f, 0.0f,  -1.0f);
	}

	void PerspecCamera::RecalculateProjMatrix(float FOV, float width, float height, float nearValue, float farValue)
	{
		m_ProjMatrix = glm::perspective(glm::radians(FOV), width/height, nearValue, farValue);
	}

	void PerspecCamera::RecalculateCameraRotation()
	{
		if(m_Pitch > 89.0f)
			m_Pitch = 89.0f;
		if(m_Pitch < -89.0f)
			m_Pitch = -89.0f;
		
		m_CamRotation.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
		m_CamRotation.y = sin(glm::radians(m_Pitch));
		m_CamRotation.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));

		m_CamFront = glm::normalize(m_CamRotation);
	}
	void PerspecCamera::RecalculateViewMatrix()
	{
		m_ViewMatrix = glm::lookAt(m_CamPos, m_CamPos + m_CamFront, m_CamUp);
	}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
}
