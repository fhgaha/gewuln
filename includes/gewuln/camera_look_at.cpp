#include <gewuln/camera_look_at.h>

CameraLookAt::CameraLookAt(
	glm::vec3 position,
	glm::vec3 up,
	float yaw,
	float pitch,
    float zoom
):
Camera(
	position,
	up,
	yaw,
	pitch,
    zoom
){
    updateCameraVectors();
}

glm::mat4 CameraLookAt::GetViewMatrix()
{
    updateCameraVectors();
    return glm::lookAt(Position, target, WorldUp);  //using WorldUp prevents camera rotation

    // return glm::lookAt(Position, target, up);    //if you need rotation around local Oz (i.e. roll) use this
}

// no need this in case of camera look at
void CameraLookAt::ProcessKeyboard(Camera_Movement direction, float deltaTime){}

// no need this in case of camera look at
void CameraLookAt::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch){}

void CameraLookAt::ProcessMouseScroll(float yoffset)
{
    Zoom -= (float)yoffset;
    if (Zoom < 1.0f)
        Zoom = 1.0f;
    if (Zoom > 45.0f)
        Zoom = 45.0f;
}

void CameraLookAt::LookAt(const glm::vec3 *target)
{
    this->target = *target;
}

void CameraLookAt::updateCameraVectors()
{
    // calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(-Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    // also re-calculate the Right and Up vector
    Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    Up    = glm::normalize(glm::cross(Right, Front));
}
