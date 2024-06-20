#include "CGCamera.h"
#include "CGCore.h"
#include "CGWindowsWindos.h"
#include "CGData.h"

using namespace CGRender;

struct CGCamera::PrivateData
{
	glm::vec2 pressPos;
	bool isMove = false;

	bool isScorll = false;

	glm::mat4 viewMatrix;

	glm::mat4 scaleMatrix;
	float scaleCoefficient = 1.f;
	float imageMinScale = .1f;//图片的比例
	int scaleFlag = 10;

#ifdef USE_ORTHO
	glm::vec3 originPos = glm::vec3{ 1 };
#endif

	WindowsWindow* winWindow = nullptr;
};

CGCamera::CGCamera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) :
	Front(glm::vec3(0.0f, 0.0f, -1.0f)),
	MovementSpeed(SPEED),
	MouseSensitivity(SENSITIVITY),
	Zoom(ZOOM),
	m_priv(new PrivateData)
{
	Position = position;
	WorldUp = up;
	Yaw = yaw;
	Pitch = pitch;
	updateCameraVectors();

	m_priv->viewMatrix = glm::lookAt(Position, Position + Front, Up);


	m_priv->scaleMatrix = glm::mat4{ 1 };

}


CGRender::CGCamera::~CGCamera()
{
	SAFE_DELETE(m_priv);
}

void CGRender::CGCamera::setWindow(WindowsWindow* winWindow)
{
	m_priv->winWindow = winWindow;
}

#ifdef USE_ORTHO
void CGRender::CGCamera::resetPosition(uint32_t windowWidth, uint32_t windowHeight)
{
	Position.x = -static_cast<float>(windowWidth) / 2.;
	Position.y = -static_cast<float>(windowHeight) / 2.;
	auto& d = *m_priv;
	d.originPos = Position;
}
glm::vec3 CGRender::CGCamera::OriginPos()
{
	auto& d = *m_priv;
	return d.originPos;
}
#endif //USE_ORTHO

glm::mat4 CGCamera::GetViewMatrix()
{
#ifdef DEBUG
	if (std::isnan(Position.x) || std::isnan(Position.y))
	{
		__debugbreak();
	}
#endif // DEBUG
	auto& d = *m_priv;
	if (!d.isMove)
		d.viewMatrix = glm::lookAt(Position, Position + Front, Up);
	return d.viewMatrix;
}

glm::mat4 CGRender::CGCamera::GetSalceMatrix()
{
	auto& d = *m_priv;
	if (d.scaleCoefficient > 0)
	{
		d.scaleMatrix[0][0] = d.scaleCoefficient;
		d.scaleMatrix[1][1] = d.scaleCoefficient;
	}
	return d.scaleMatrix;
}

float CGRender::CGCamera::ScaleCoefficient()
{
	auto& d = *m_priv;
	return d.scaleCoefficient;
}

void CGRender::CGCamera::ScaleCoefficient(float scale)
{
	auto& d = *m_priv;
	d.scaleCoefficient = scale;
}

void CGRender::CGCamera::ImageMinScale(float scale)
{
	auto& d = *m_priv;
	d.imageMinScale = scale;
}

float CGRender::CGCamera::ImageMinScale()
{
	return m_priv->imageMinScale;
}

void CGRender::CGCamera::ProcessMouseMoveXY(float x, float y, bool xChange, bool yChange)
{
	auto& d = *m_priv;
	if (d.isMove)
	{
		glm::vec2 offset = glm::vec2{ x,y } - d.pressPos;
#ifdef USE_ORTHO
		//先移动 layer 再同步到 camera
		offset *= d.scaleCoefficient;
#endif // USE_ORTHO
		if (xChange)
			Position.x -= offset.x;
		if (yChange)
			Position.y -= offset.y;

		d.pressPos = glm::vec2{ x,y };
		return;
	}

}

void CGRender::CGCamera::ProcessMousePress(float x, float y)
{
	auto& d = *m_priv;
	d.pressPos.x = x;
	d.pressPos.y = y;
	d.isMove = true;
}

void CGRender::CGCamera::ProcessMouseRelease(float x, float y)
{
	auto& d = *m_priv;
	d.pressPos.x = x;
	d.pressPos.y = y;
	d.isMove = false;
}

void CGCamera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
	xoffset *= MouseSensitivity;
	yoffset *= MouseSensitivity;

	Yaw += xoffset;
	Pitch += yoffset;

	if (constrainPitch)
	{
		if (Pitch > 89.0f)
			Pitch = 89.0f;
		if (Pitch < -89.0f)
			Pitch = -89.0f;
	}

	updateCameraVectors();
}




void CGCamera::ProcessMouseScroll(float yoffset, float x, float y)
{


#ifdef USE_ORTHO
	auto& d = *m_priv;

	yoffset > 0 ? d.scaleFlag++ : d.scaleFlag--;
	if (d.scaleFlag < 0)
		d.scaleFlag = 0;
	if (d.scaleFlag > CAMERA_SCALE_NUMS - 1)
		d.scaleFlag = CAMERA_SCALE_NUMS - 1;
	d.scaleCoefficient = g_Camera_SCALE[d.scaleFlag];
	if (d.scaleCoefficient < d.imageMinScale)
	{
		d.scaleCoefficient = d.imageMinScale;
	}

	GLRender_LOG("camera sclaeCoefficient :", d.scaleCoefficient);
#else
	float offset = yoffset > 0 ? 0.5 : -0.5;
	Zoom -= offset;
	if (Zoom < 1.0f)
		Zoom = 1.0f;
	if (Zoom > 45.0f)
		Zoom = 45.0f;
#endif // USE_ORTHO
}

void CGCamera::updateCameraVectors()
{
	glm::vec3 front;
	front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	front.y = sin(glm::radians(Pitch));
	front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	Front = glm::normalize(front);
	Right = glm::normalize(glm::cross(Front, WorldUp));
	Up = glm::normalize(glm::cross(Right, Front));
}