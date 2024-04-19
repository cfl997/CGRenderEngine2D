#include "CGCamera.h"
#include "CGCore.h"

using namespace CGRender;

struct CGCamera::PrivateData
{
	glm::vec2 pressPos;
	bool isMove = false;

	bool isScorll = false;

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

}


CGRender::CGCamera::~CGCamera()
{
}

//CGCamera::CGCamera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
//{
//    Position = glm::vec3(posX, posY, posZ);
//    WorldUp = glm::vec3(upX, upY, upZ);
//    Yaw = yaw;
//    Pitch = pitch;
//    updateCameraVectors();
//}

glm::mat4 CGCamera::GetViewMatrix()
{
#ifdef DEBUG
	if (std::isnan(Position.x) || std::isnan(Position.y))
	{
		__debugbreak();
	}
#endif // DEBUG
	return glm::lookAt(Position, Position + Front, Up);
}

void CGRender::CGCamera::ProcessMouseMoveXY(float x, float y)
{
	//cfl-20240416
	return;
	auto& d = *m_priv;
	if (d.isMove)
	{

		static const float movePosSpeed = .1f;
		auto xoffset = (x - d.pressPos.x) * movePosSpeed;
		Position.x -= xoffset;
		auto fn = [](float&a){
			if (a < -1000)
				a = -1000;
			if (a > 1000)
				a = 1000;
			};

		//fn(Position.x);


		auto yoffset = (y - d.pressPos.y) * movePosSpeed;

		Position.y += yoffset;
		//fn(Position.y);

		if (std::isnan(Position.x) || std::isnan(Position.y))
		{
			__debugbreak();
		}

		d.pressPos.x = x;
		d.pressPos.y = y;

		//d.pressPos.x = Position.x;
		//d.pressPos.y = Position.y;

		GLRender_LOG("cameraPosition: x", Position.x);
		GLRender_LOG("cameraPosition: y", Position.y);
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
	Zoom -= (float)yoffset;
	if (Zoom < 1.0f)
		Zoom = 1.0f;
	if (Zoom > 45.0f)
		Zoom = 45.0f;
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