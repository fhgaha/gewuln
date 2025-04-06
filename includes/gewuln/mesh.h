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
	int boneIDs[MAX_BONE_INFLUENCE];   //bone indexes which will influence this vertex
	float weights[MAX_BONE_INFLUENCE]; //weights from each bone
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
	std::vector<Vertex>       vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture>      textures;
	bool IsAnimated() const;

	Mesh(){}
	Mesh(
		std::vector<Vertex>       vertices, 
		std::vector<unsigned int> indices,
		std::vector<Texture>      textures,
		bool                      animated
	);
	
	void Draw(Shader &shader);

private:
	bool animated;
	// render data	
	unsigned int VAO, VBO, EBO;
	
	void setupMesh();
};

#endif
