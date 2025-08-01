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
#include <unordered_map>
#include <format>

unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);

/*
For static models all their transformations should be "applied" in Blender, i.e. Location, Rotation and should be reseted to (0, 0, 0)
and Scale to (1, 1, 1). This is achieved through `Ctrl + A -> All Transforms` in Blender. No position, rotation or scale data is stored
once model loading process is finished. Blender's description: https://docs.blender.org/manual/en/latest/scene_layout/object/editing/apply.html
*/
class Model
{
public:
	enum class BlenderCustomProps {
		Regular,
		Collider,
		Interactable,
		Walkable,
		RoomExit
	};

    static const std::unordered_map<std::string, BlenderCustomProps>& get_blender_custom_props() {
        static const std::unordered_map<std::string, BlenderCustomProps> mapping = {
            {"is_collider",      BlenderCustomProps::Collider},
            {"is_interactable",  BlenderCustomProps::Interactable},
            {"is_walkable",		 BlenderCustomProps::Walkable},
            {"is_walkable_area", BlenderCustomProps::Walkable},
            {"is_room_exit",     BlenderCustomProps::RoomExit}
        };
        return mapping;
    }

	glm::vec3			position;
	std::vector<Mesh>	meshes;

	//optional meshes that are imported from blender using custom properties
	std::optional<Mesh>	collider_mesh;
	std::optional<Mesh>	walkable_area;
	std::vector<Mesh> 	interactiable_meshes;
	std::vector<Mesh> 	room_exit_meshes;

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

		process_node_recursively(scene->mRootNode, scene);
	}

	void process_node_recursively(aiNode *node, const aiScene *scene)
	{
		//looking for a collider
		//Usage: in Blender create a cube, select the Cube object, create custom property "is_collider".

		BlenderCustomProps cur_custom_prop = BlenderCustomProps::Regular;

		bool has_metadata = node->mMetaData != nullptr && node->mMetaData->mNumProperties > 0;
		if (has_metadata) {
			for (unsigned int i = 0; i < node->mMetaData->mNumProperties; i++)
			{
				const aiString& key = node->mMetaData->mKeys[i];
				const auto &strings_as_blender_custom_types = Model::get_blender_custom_props();	//what is going on?
        		auto it = strings_as_blender_custom_types.find(key.C_Str());
				if (it != strings_as_blender_custom_types.end()){
					cur_custom_prop = it->second;
				}

				switch (cur_custom_prop)
				{
					case BlenderCustomProps::Regular:
					case BlenderCustomProps::Walkable:
					case BlenderCustomProps::RoomExit:
						break;
					case BlenderCustomProps::Collider:
						assert(node->mNumMeshes == 1 && "COLLIDER NODE SHOULD HAVE ONE MESH");
						break;
					case BlenderCustomProps::Interactable:
						assert(node->mNumMeshes == 1 && "INTERACTABLE NODE SHOULD HAVE ONE MESH");
						break;
					default:
						assert(false && "!Should not be reachable");
						break;
				}
			}
		}
		
		node->mTransformation = scene->mRootNode == node// node->mParent == nullptr 
			? aiMatrix4x4()
			: node->mParent->mTransformation * node->mTransformation;
			
		glm::mat4 trm = Assimp_GLM_Helpers::ai_mat_to_glm(node->mTransformation);
			
		// process all the node’s meshes (if any)
		for(unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
			switch (cur_custom_prop)
			{
				case BlenderCustomProps::Regular:{
					auto m = processMesh(mesh, scene, this->animated, trm);
					if (!this->animated){
						m.transformation = Assimp_GLM_Helpers::ai_mat_to_glm(node->mTransformation);
						m.parent = node->mParent;
					}
					meshes.push_back(std::move(m));
				}break;
				case BlenderCustomProps::Collider:{
					auto m = processMesh(mesh, scene, false, trm);
					m.transformation = Assimp_GLM_Helpers::ai_mat_to_glm(node->mTransformation);
					m.parent = node->mParent;
					collider_mesh = std::move(m);
					cur_custom_prop = BlenderCustomProps::Regular;
				}break;
				case BlenderCustomProps::Interactable:{
					auto m = processMesh(mesh, scene, false, trm);
					m.transformation = Assimp_GLM_Helpers::ai_mat_to_glm(node->mTransformation);
					m.parent = node->mParent;
					interactiable_meshes.push_back(std::move(m));
					cur_custom_prop = BlenderCustomProps::Regular;
				}break;
				case BlenderCustomProps::Walkable:{
					auto m = processMesh(mesh, scene, false, trm);
					m.transformation = Assimp_GLM_Helpers::ai_mat_to_glm(node->mTransformation);
					m.parent = node->mParent;
					walkable_area = std::move(m);
					cur_custom_prop = BlenderCustomProps::Regular;
				}break;
				case BlenderCustomProps::RoomExit:{
					auto m = processMesh(mesh, scene, false, trm);
					m.transformation = Assimp_GLM_Helpers::ai_mat_to_glm(node->mTransformation);
					m.parent = node->mParent;
					room_exit_meshes.push_back(std::move(m));
					cur_custom_prop = BlenderCustomProps::Regular;
				}break;
				default:
					assert(false && "!Should not be reachable");
					break;
			}
		}

		// then do the same for each of its children
		for(unsigned int i = 0; i < node->mNumChildren; i++)
		{
			process_node_recursively(node->mChildren[i], scene);
		}
	}

	Mesh processMesh(aiMesh *mesh, const aiScene *scene, bool animated, const glm::mat4 &transform)
	{
		std::vector<Vertex>       vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture>      textures;

		glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(transform)));	//wtf??
		
		for(unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			// vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
			glm::vec4 pos = glm::vec4(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z, 1.0f);
			if (!animated){
				pos = transform * pos;
			}
			vertex.Position = glm::vec3(pos);
			
			
			// vertex.Normal 	= glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
			if (mesh->HasNormals()) {
				glm::vec3 norm = normalMatrix * glm::vec3( mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
				vertex.Normal = glm::normalize(norm);
			}
			

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
                newBoneInfo.offset = Assimp_GLM_Helpers::ai_mat_to_glm(
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
