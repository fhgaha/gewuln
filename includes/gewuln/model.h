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

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <optional>


unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);


/*
Custom properties:
	"is_collider"
	"is_interactable"
	"is_walkable_area"
*/


/*
For static models all their transformations should be "applied" in Blender, i.e. Location, Rotation and should be reseted to (0, 0, 0)
and Scale to (1, 1, 1). This is achieved through `Ctrl + A -> All Transforms` in Blender. No position, rotation or scale data is stored
once model loading process is finished. Blender's description: https://docs.blender.org/manual/en/latest/scene_layout/object/editing/apply.html
*/
class Model
{
public:
	glm::vec3			position;
	std::vector<Mesh>	meshes;
	std::optional<Mesh>	collider_mesh;
	std::optional<Mesh>	interactable_mesh;
	
	std::vector<Mesh> 	interactiable_meshes;
	
	std::optional<Mesh>	walkable_area;
	std::optional<Mesh>	room_exit;

	Model() {}
	Model(std::string const &path, bool animated, glm::vec3 pos = glm::vec3(0.0f))
	{
		this->animated = animated;
		this->position = pos;
		loadModel(path);
	}

	std::map<std::string, BoneInfo>& 	GetBoneInfoMap() { return boneInfoMap; }
    int& 								GetBoneCount() { return boneCounter; }

private:
	std::string          directory;
	std::vector<Texture> textures_loaded;

	std::map<std::string, BoneInfo> boneInfoMap;
    int boneCounter = 0;
	bool animated;

	void loadModel(std::string path)
	{
		Assimp::Importer import;
		const aiScene *scene = import.ReadFile(
			path,
			aiProcess_Triangulate |	aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices
		);
		
		bool error = !scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode;
		if(error)
		{
			std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
			return;
		}
		directory = path.substr(0, path.find_last_of('/'));

		processNode(scene->mRootNode, scene);
	}

	void processNode(aiNode *node, const aiScene *scene)
	{
		//looking for a collider
		//Usage: in Blender create a cube, select the Cube object, create custom property "is_collider".

		// std::cout << "node: " << node->mName.data << "\n";
		bool node_is_collider 		= false;
		bool node_is_interactable 	= false;
		bool node_is_walkable_area 	= false;
		bool node_is_room_exit 		= false;

		bool has_metadata = node->mMetaData != nullptr && node->mMetaData->mNumProperties > 0;
		if (has_metadata) {
			for (unsigned int i = 0; i < node->mMetaData->mNumProperties; i++)
			{
				//collider
				if (node->mMetaData->mKeys[i] == aiString("is_collider")){
					// std::cout << "!!found a collider " << node->mName.data
					// << " in a node " << node->mName.data
					// << " of a scene " << scene->mName.data
					// << "\n";

					if (node->mNumMeshes != 1) {
						std::cout << "COLLIDER NODE SHOULD HAVE ONE MESH. Having instead: "
							<< node->mNumMeshes << "\n";
					}

					node_is_collider = true;
				}

				//interactable
				if (node->mMetaData->mKeys[i] == aiString("is_interactable")){
					// std::cout << "!!found an interactable " << node->mName.data
					// << " in a node " << node->mName.data
					// << " of a scene " << scene->mName.data
					// << "\n";

					if (node->mNumMeshes != 1) {
						std::cout << "INTERACTABLE NODE SHOULD HAVE ONE MESH. Having instead: "
							<< node->mNumMeshes << "\n";
					}

					node_is_interactable = true;
				}

				//walking area
				if (node->mMetaData->mKeys[i] == aiString("is_walkable_area")){
					// if (node->mNumMeshes != 1) {
					// 	std::cout << "INTERACTABLE NODE SHOULD HAVE ONE MESH. Having instead: "
					// 		<< node->mNumMeshes << "\n";
					// }

					// std::cout << "!!found a walkable area " << node->mName.data
					// << " in a node " << node->mName.data
					// << " of a scene " << scene->mName.data
					// << "\n";

					node_is_walkable_area = true;
				}
				
				if (node->mMetaData->mKeys[i] == aiString("is_room_exit")){
					node_is_room_exit = true;
				}
			}
		}

		// process all the node’s meshes (if any)
		for(unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];

			if (node_is_collider) {
				collider_mesh = processMesh(mesh, scene, false);
				node_is_collider = false;
			} else if (node_is_interactable) {
				// interactable_mesh = processMesh(mesh, scene, false);
				interactiable_meshes.push_back(processMesh(mesh, scene, false));
				node_is_interactable = false;
			} else if (node_is_walkable_area){
				walkable_area = processMesh(mesh, scene, false);
				node_is_walkable_area = false;
			} else if (node_is_room_exit){
				//TODO should be able to have multiple room exits
				room_exit = processMesh(mesh, scene, false);
				node_is_room_exit = false;
			} else {
				meshes.push_back(processMesh(mesh, scene, this->animated));
			}
		}

		// then do the same for each of its children
		for(unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}

	}

	Mesh processMesh(aiMesh *mesh, const aiScene *scene, bool animated)
	{
		std::vector<Vertex>       vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture>      textures;

		for(unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
			vertex.Normal 	= glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

			glm::vec2 tex_coords = glm::vec2(0.0f, 0.0f);
			// use texture coordinates if mesh has any
			for (size_t tcIdx = 0; tcIdx < AI_MAX_NUMBER_OF_TEXTURECOORDS; tcIdx++){
				if (mesh->HasTextureCoords(tcIdx)) {
					tex_coords.x = mesh->mTextureCoords[tcIdx][i].x;
					tex_coords.y = mesh->mTextureCoords[tcIdx][i].y;
					break;
				}
			}
			vertex.TexCoords = tex_coords;

			//default bone datas
			for (int j = 0; j < MAX_BONE_INFLUENCE; j++)
			{
				vertex.boneIDs[j] = -1;
				vertex.weights[j] = 0.0f;
			}

			vertices.push_back(vertex);
		}

		// process indices
		for(unsigned int j = 0; j < mesh->mNumFaces; j++)
		{
			aiFace face = mesh->mFaces[j];
			for(unsigned int k = 0; k < face.mNumIndices; k++){
				indices.push_back(face.mIndices[k]);
			}
		}

		// process material
		if(mesh->mMaterialIndex >= 0)
		{
			aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
			std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(),	specularMaps.end());
		}

		ExtractBoneWeightForVertices(vertices, mesh, scene);

		return Mesh(vertices, indices, textures, animated);
	}

	std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
	{
		std::vector<Texture> textures;
		for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);

			// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
			bool skip = false;
			for(unsigned int j = 0; j < textures_loaded.size(); j++)
			{
				if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
				{
					textures.push_back(textures_loaded[j]);
					skip = true;
					break;
				}
			}
			if (!skip)
			{
				Texture texture;
				texture.id = TextureFromFile(str.C_Str(), directory);
				texture.type = typeName;
				texture.path = str.C_Str();
				textures.push_back(texture);
				textures_loaded.push_back(texture); // add to loaded textures
			}
		}
		return textures;
	}

	void SetVertexBoneData(Vertex& vertex, int boneID, float weight)
    {
		// funny results if set 1 instead of MAX_BONE_INFLUENCE
        for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
        {
            if (vertex.boneIDs[i] < 0)
            {
                vertex.weights[i] = weight;
                vertex.boneIDs[i] = boneID;
                break;
            }
        }
    }

    void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene)
    {
        for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
        {
			/*some Assimp bones have a structure like this:
				mesh->mBones[boneIndex]->mNumWeights == 1
			and
				aiVertexWeight vw = mesh->mBones[boneIndex]->mWeights[0];
				vw.mBoneId = 0;
				vw.mWeight = 0.0f;
			lets skip that thing.
			this causes triangle horrors apparantely.
			*/
			if (mesh->HasBones() && mesh->mBones[boneIndex]->mNumWeights == 1) {
				continue;
			}


            int boneID = -1;
            std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();


            if (boneInfoMap.find(boneName) == boneInfoMap.end())
            {
                BoneInfo newBoneInfo;
                newBoneInfo.id = boneCounter;
                newBoneInfo.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(
                    mesh->mBones[boneIndex]->mOffsetMatrix
				);
                boneInfoMap[boneName] = newBoneInfo;
                boneID = boneCounter;
                boneCounter++;
            }
            else
            {
                boneID = boneInfoMap[boneName].id;
            }
            assert(boneID != -1);
            auto weights = mesh->mBones[boneIndex]->mWeights;
            int numWeights = mesh->mBones[boneIndex]->mNumWeights;

            for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
            {
                int vertexId = weights[weightIndex].mVertexId;
                float weight = weights[weightIndex].mWeight;
                assert(vertexId <= vertices.size());
                SetVertexBoneData(vertices[vertexId], boneID, weight);
            }
        }
    }

};

//inline here cause
	// win64_gewuln.cpp → includes model.h → compiles to win64_gewuln.obj
    // resource_manager.cpp → includes model.h → compiles to resource_manager.obj
// without inline it causes linker error
inline unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma)
{
    std::string filename = std::string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(
		filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		//filtered
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// unfiltered
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

#endif
