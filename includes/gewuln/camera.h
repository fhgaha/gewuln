#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.4f;
const float ZOOM        =  45.0f;

class Camera
{
public:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // euler Angles
    float Yaw;     // курс   - rotation around Oy
    float Pitch;   // тангаж - rotation around Ox
    // float Roll; // крен   - rotation around Oz

    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    //TODO try deleting this
    // Camera(){}

    Camera(
    	glm::vec3 position,
    	glm::vec3 up,
    	float yaw,
    	float pitch,
        float zoom
    ):
    Front(glm::vec3(0.0f, 0.0f, -1.0f)),
    MovementSpeed(SPEED),
    MouseSensitivity(SENSITIVITY)
    {
    	Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        Zoom = zoom;
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    virtual glm::mat4 GetViewMatrix() = 0;
    virtual void ProcessKeyboard(Camera_Movement direction, float deltaTime) = 0;
    virtual void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true) = 0;
    virtual void ProcessMouseScroll(float yoffset) = 0;
    virtual void LookAt(const glm::vec3 *target) = 0;
};

#endif