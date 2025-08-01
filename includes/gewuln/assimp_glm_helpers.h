#pragma once

#include<assimp/quaternion.h>
#include<assimp/vector3.h>
#include<assimp/matrix4x4.h>
#include<glm/glm.hpp>
#include<glm/gtc/quaternion.hpp>
#include<iostream>

class Assimp_GLM_Helpers
{
public:

	static inline glm::mat4 ai_mat_to_glm(const aiMatrix4x4& from)
	{
		glm::mat4 to;
		//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
		to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
		to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
		to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
		to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
		return to;
	}

	static inline glm::vec3 GetGLMVec(const aiVector3D& vec) 
	{ 
		return glm::vec3(vec.x, vec.y, vec.z); 
	}

	static inline glm::quat GetGLMQuat(const aiQuaternion& pOrientation)
	{
		return glm::quat(pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z);
	}
	
	static inline void PrintMatix4(const glm::mat4& mat)
	{
		std::cout
		<< mat[0][0] << "\t" << mat[1][0] << "\t"<< mat[2][0] << "\t"<< mat[3][0] << "\n"
		<< mat[0][1] << "\t" << mat[1][1] << "\t"<< mat[2][1] << "\t"<< mat[3][1] << "\n"
		<< mat[0][2] << "\t" << mat[1][2] << "\t"<< mat[2][2] << "\t"<< mat[3][2] << "\n"
		<< mat[0][3] << "\t" << mat[1][3] << "\t"<< mat[2][3] << "\t"<< mat[3][3] << "\n"
		<< "\n";
	}
};