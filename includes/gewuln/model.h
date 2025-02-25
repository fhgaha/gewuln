#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb/stb_image.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <gewuln/mesh.h>
#include <gewuln/shader.h>
#include <gewuln/assimp_glm_helpers.h>

#include <gewuln/resource_manager.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include "texture.h"

class Model
{
public:
	Model();
	Model(const aiScene *scene, std::string directory, bool animated);

	void Draw(Shader &shader);
	std::map<std::string, BoneInfo>& GetBoneInfoMap();
    int& GetBoneCount();
	bool IsAnimated() const;
	
private:
	// model data
	std::vector<Mesh> meshes;
	std::string directory;
	std::vector<Texture> textures_loaded;
	
	std::map<std::string, BoneInfo> m_BoneInfoMap; //
    int m_BoneCounter = 0;
	bool animated;
	
	void processNode(aiNode *node, const aiScene *scene);
	void setVertexBoneDataToDefault(Vertex& vertex);
	Mesh processMesh(aiMesh *mesh, const aiScene *scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
	void setVertexBoneData(Vertex& vertex, int boneID, float weight);
    void extractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);
};

#endif
