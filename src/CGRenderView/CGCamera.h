#ifndef _CAMERA_H_
#define _CAMERA_H_



#include "CGglm.h"





namespace CGRender
{

	/*
	* YAW 通常用于表示摄像机的左右旋转角度。这个值设为 -90 度通常代表摄像机的初始方向是朝向场景的负 X 轴方向。正数表示顺时针旋转，负数表示逆时针旋转。
	* PITCH 通常用于表示摄像机的上下旋转角度。这个值设为 0 度通常代表摄像机的初始方向是水平的，不倾斜。正数表示向上旋转，负数表示向下旋转。
	* SPEED 表示摄像机移动的速度。这个值表示摄像机在单位时间内移动的距离。在游戏中，它通常用于控制摄像机的移动速度。
	* SENSITIVITY 表示摄像机旋转的灵敏度。这个值用于控制鼠标或者其他输入设备移动时对视角的调整速度。通常用于控制摄像机的旋转速度。
	* ZOOM 表示摄像机的缩放级别，通常用于控制摄像机的视场（Field of View，FOV）。这个值表示摄像机的透视投影中的垂直视野角度。较小的值会导致更大的视野范围，反之亦然。
	*/
	const float YAW = -90.0f;
	const float PITCH = 0.0f;
	const float SPEED = 2.5f;
	const float SENSITIVITY = 0.1f;
	const float ZOOM = 45.0f;
	class CGCamera
	{
	public:
		// camera Attributes
		glm::vec3 Position;
		glm::vec3 Front;
		glm::vec3 Up;
		glm::vec3 Right;
		glm::vec3 WorldUp;
		// euler Angles
		float Yaw;
		float Pitch;
		// camera options
		float MovementSpeed;
		float MouseSensitivity;
		float Zoom;

		CGCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 1000.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);
		//CGCamera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

		~CGCamera();

		glm::mat4 GetViewMatrix();

		void ProcessMouseMoveXY(float x, float y);
		void ProcessMousePress(float x, float y);
		void ProcessMouseRelease(float x, float y);
		void ProcessMouseScroll(float yoffset, float x, float y);

	private:
		void updateCameraVectors();

		void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);

		struct PrivateData;
		PrivateData* m_priv;
	};
}
#endif