#include "model.h"
#include <stb/stb_image.h> 
#include <iostream> 

Model::Model(): animated(false){}

Model::Model(const aiScene *scene, std::string directory, bool animated) : animated(animated)
{
	this->directory = directory;
	processNode(scene->mRootNode, scene);
}

void Model::Draw(Shader &shader)
{
	for(unsigned int i = 0; i < meshes.size(); i++){
		meshes[i].Draw(shader);
	}
}

std::map<std::string, BoneInfo>& Model::GetBoneInfoMap() { return m_BoneInfoMap; }
int& Model::GetBoneCount() { return m_BoneCounter; }
bool Model::IsAnimated() const {return animated;}

void Model::processNode(aiNode *node, const aiScene *scene)
{
	// process all the node’s meshes (if any)
	for(unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	
	// then do the same for each of its children
	for(unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
	std::vector<Vertex>       vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture>      textures;
	
	for(unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		// process vertex positions, normals and texture coordinates
		glm::vec3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;
		
		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.Normal = vector;
		
		
		if(mesh->mTextureCoords[0]) { // does the mesh contain texture coordinates?
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		} else {
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		}
		
		setVertexBoneDataToDefault(vertex);
		
		vertices.push_back(vertex);
	}
	
	// process indices
	for(unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for(unsigned int j = 0; j < face.mNumIndices; j++){
			indices.push_back(face.mIndices[j]);
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
	
	extractBoneWeightForVertices(vertices,mesh,scene);
	
	return Mesh(vertices, indices, textures, animated);
}

void Model::setVertexBoneDataToDefault(Vertex& vertex)
{
	for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
	{
		vertex.boneIDs[i] = -1;
		vertex.weights[i] = 0.0f;
	}
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
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
			bool equal = std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0;
			if(equal)
			{
				textures.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}
		if (!skip)
		{
			std::string filename = std::string(str.C_Str());
			Texture2D tex = ResourceManager::LoadTexture((directory + '/' + filename).c_str(), true, filename);
			
			Texture texture;
			texture.id = tex.ID;
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture); 
			textures_loaded.push_back(texture); // add to loaded textures
		}
	}
	return textures;
}

void Model::extractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene)
{
	for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
	{
		int boneID = -1;
		std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
		if (m_BoneInfoMap.find(boneName) == m_BoneInfoMap.end())
		{
			BoneInfo newBoneInfo;
			newBoneInfo.id = m_BoneCounter;
			newBoneInfo.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(
				mesh->mBones[boneIndex]->mOffsetMatrix);
			m_BoneInfoMap[boneName] = newBoneInfo;
			boneID = m_BoneCounter;
			m_BoneCounter++;
		}
		else
		{
			boneID = m_BoneInfoMap[boneName].id;
		}
		assert(boneID != -1);
		auto weights = mesh->mBones[boneIndex]->mWeights;
		int numWeights = mesh->mBones[boneIndex]->mNumWeights;

		for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
		{
			int vertexId = weights[weightIndex].mVertexId;
			float weight = weights[weightIndex].mWeight;
			assert(vertexId <= vertices.size());
			setVertexBoneData(vertices[vertexId], boneID, weight);
		}
	}
}


void Model::setVertexBoneData(Vertex& vertex, int boneID, float weight)
{
	for (int i = 0; i < MAX_BONE_INFLUENCE; ++i) {
		if (vertex.boneIDs[i] < 0) 
		{
			vertex.weights[i] = weight;
			vertex.boneIDs[i] = boneID;
			break;
		}
	}
}
