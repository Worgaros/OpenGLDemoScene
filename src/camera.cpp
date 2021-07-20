#include "camera.h"

namespace gl
{
	// constructor with vectors
	Camera::Camera(
		glm::vec3 position,
		glm::vec3 up,
		float yaw,
		float pitch) :
		front(glm::vec3(0.0f, 0.0f, -1.0f)),
		movement_speed_(SPEED),
		mouse_sensitivity_(SENSITIVITY),
		zoom_(ZOOM)
	{
		this->position = position;
		this->world_up = up;
		this->yaw = yaw;
		this->pitch = pitch;
		UpdateCameraVectors();
	}
	// constructor with scalar values
	Camera::Camera(
		float posX, float posY, float posZ,
		float upX, float upY, float upZ,
		float yaw, float pitch) :
		front(glm::vec3(0.0f, 0.0f, -1.0f)),
		movement_speed_(SPEED),
		mouse_sensitivity_(SENSITIVITY),
		zoom_(ZOOM)
	{
		position = glm::vec3(posX, posY, posZ);
		world_up = glm::vec3(upX, upY, upZ);
		yaw = yaw;
		pitch = pitch;
		UpdateCameraVectors();
	}

	// returns the view matrix calculated using Euler Angles and the LookAt
	// Matrix
	glm::mat4 Camera::GetViewMatrix()
	{
		return glm::lookAt(position, position + front, up);
	}

	// processes input received from any keyboard-like input system.
	// Accepts input parameter in the form of camera defined ENUM (to
	// abstract it from windowing systems)
	void Camera::ProcessKeyboard(CameraMovementEnum direction, float deltaTime)
	{
		float velocity = movement_speed_ * deltaTime;
		if (direction == CameraMovementEnum::UP)
			position += vertical * velocity;
		if (direction == CameraMovementEnum::DOWN)
			position -= vertical * velocity;
		if (direction == CameraMovementEnum::LEFT)
			position -= right * velocity;
		if (direction == CameraMovementEnum::RIGHT)
			position += right * velocity;
	}

	// processes input received from a mouse input system. Expects the
	// offset value in both the x and y direction.
	void Camera::ProcessMouseMovement(
		float xoffset,
		float yoffset,
		GLboolean constrainpitch)
	{
		xoffset *= mouse_sensitivity_;
		yoffset *= mouse_sensitivity_;

		yaw += xoffset;
		pitch += yoffset;

		// make sure that when pitch is out of bounds, screen doesn't get
		// flipped
		if (constrainpitch)
		{
			if (pitch > 89.0f)
				pitch = 89.0f;
			if (pitch < -89.0f)
				pitch = -89.0f;
		}

		// update front, right and up Vectors using the updated Euler
		// angles
		UpdateCameraVectors();
	}

	// processes input received from a mouse scroll-wheel event. Only
	// requires input on the vertical wheel-axis
	void Camera::ProcessMouseScroll(float yoffset)
	{
		zoom_ -= (float)yoffset;
		if (zoom_ < 1.0f)
			zoom_ = 1.0f;
		if (zoom_ > 45.0f)
			zoom_ = 45.0f;
	}

	void Camera::SetState(glm::vec3 position, float pitch, float yaw)
	{
		this->position = position;
		this->pitch = pitch;
		this->yaw = yaw;
		UpdateCameraVectors();
	}

	// calculates the front vector from the Camera's (updated) Euler Angles
	void Camera::UpdateCameraVectors()
	{
		// calculate the new front vector
		glm::vec3 front_;
		front_.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front_.y = sin(glm::radians(pitch));
		front_.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		front = glm::normalize(front_);
		// also re-calculate the right and up vector
		right = glm::normalize(glm::cross(front, world_up));
		// normalize the vectors, because their length gets closer to 0 the
		// more you look up or down which results in slower movement.
		up = glm::normalize(glm::cross(right, front));
	}
}
