#ifndef CAMERA_FLY_H
#define CAMERA_FLY_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <gewuln/camera/camera.h>

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class CameraFly: public Camera
{
public:
    // Yaw - курс, pitch - тангаж
    CameraFly(
		glm::vec3 position = glm::vec3(0.0f),
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
		float yaw = YAW,
		float pitch = PITCH,
        float zoom = ZOOM
	);

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix() override;
    void ProcessKeyboard(Camera_Movement direction, float deltaTime) override;
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true) override;
    void ProcessMouseScroll(float yoffset) override;
    void LookAt(const glm::vec3 *target) override;

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors();
};

#endif