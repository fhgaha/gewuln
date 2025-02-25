#ifndef MESH_H
#define MESH_H
#define MAX_BONE_INFLUENCE 4

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <gewuln/shader.h>

#include <string>
#include <vector>

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;	

	//below is for animation	
	int m_BoneIDs[MAX_BONE_INFLUENCE]; //bone indexes which will influence this vertex
	float m_Weights[MAX_BONE_INFLUENCE]; //weights from each bone
};

struct Texture
{
	unsigned int id;
	std::string type; //"texture_diffuse" or "texture_specular"
	std::string path; // store path of texture to compare with other textures
};

struct BoneInfo
{
    int id; /*id is index in finalBoneMatrices*/
    glm::mat4 offset; /*offset matrix transforms vertex from model space to bone space*/
};

class Mesh
{
public:
	std::vector<Vertex>   vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture>  textures;

	Mesh(
		std::vector<Vertex>   vertices, 
		std::vector<unsigned int> indices,
		std::vector<Texture>  textures
	);
	
	void Draw(Shader &shader);

private:
	// render data	
	unsigned int VAO, VBO, EBO;
	
	void setupMesh();
};

#endif
