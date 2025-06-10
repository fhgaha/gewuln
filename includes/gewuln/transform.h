#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm/glm.hpp>

struct Transform {
	glm::mat4	transformation	= glm::mat4();
	Transform	*parent			= nullptr;
};

#endif