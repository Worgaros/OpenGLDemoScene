#pragma once

#include "mesh2.h"
#include "material.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "texturi.h"
#include "meshi.h"

namespace gl
{
	class Model {
	public:
		std::string directory = "../data/textures/scene_textures/";
		Model(const std::string& filename) {
			Assimp::Importer importer;
			
			const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
			if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
			{
				std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
				return;
			}

			processNode(scene->mRootNode, scene);
			
		}

		void processNode(aiNode* node, const aiScene* scene)
		{
			// process all the node's meshes (if any)
			for (unsigned int i = 0; i < node->mNumMeshes; i++)
			{
				aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
				meshes.push_back(processMesh(mesh, scene));
			}
			// then do the same for each of its children
			for (unsigned int i = 0; i < node->mNumChildren; i++)
			{
				processNode(node->mChildren[i], scene);
			}
		}

		MeshAssimp processMesh(aiMesh* mesh, const aiScene* scene)
		{
			std::vector<VertexAssimp> vertices;
			std::vector<unsigned int> indices;
			std::vector<textureAssimp> textures;

			for (unsigned int i = 0; i < mesh->mNumVertices; i++)
			{
				VertexAssimp vertex;
				// process vertex positions, normals and texture coordinates
				glm::vec3 vector;
				vector.x = mesh->mVertices[i].x;
				vector.y = mesh->mVertices[i].y;
				vector.z = mesh->mVertices[i].z;
				vertex.position = vector;
				vector.x = mesh->mNormals[i].x;
				vector.y = mesh->mNormals[i].y;
				vector.z = mesh->mNormals[i].z;
				vertex.normal = vector;
				if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
				{
					glm::vec2 vec;
					vec.x = mesh->mTextureCoords[0][i].x;
					vec.y = mesh->mTextureCoords[0][i].y;
					vertex.texture = vec;
				}
				else
					vertex.texture = glm::vec2(0.0f, 0.0f);
				
				vector.x = mesh->mTangents[i].x;
				vector.y = mesh->mTangents[i].y;
				vector.z = mesh->mTangents[i].z;
				vertex.tangents = vector;

				vector.x = mesh->mBitangents[i].x;
				vector.y = mesh->mBitangents[i].y;
				vector.z = mesh->mBitangents[i].z;
				vertex.bitangents = vector;
				
				vertices.push_back(vertex);
			}
			// process indices
			for (unsigned int i = 0; i < mesh->mNumFaces; i++)
			{
				aiFace face = mesh->mFaces[i];
				for (unsigned int j = 0; j < face.mNumIndices; j++)
					indices.push_back(face.mIndices[j]);
			}
			// process material
			if (mesh->mMaterialIndex >= 0)
			{
				aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
				std::vector<textureAssimp> diffuseMaps = loadMaterialTextures(material,
					aiTextureType_DIFFUSE, "Diffuse");
				textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
				std::vector<textureAssimp> specularMaps = loadMaterialTextures(material,
					aiTextureType_SPECULAR, "Specular");
				textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
				std::vector<textureAssimp> normalMaps = loadMaterialTextures(material,
					aiTextureType_HEIGHT, "Normal");
				textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
			}

			return MeshAssimp(vertices, indices, textures);
		}

		std::vector<textureAssimp> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
		{
			std::vector<textureAssimp> textures;
			for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
			{
				aiString str;
				mat->GetTexture(type, i, &str);
				bool skip = false;
				for (unsigned int j = 0; j < textures_loaded.size(); j++)
				{
					if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
					{
						textures.push_back(textures_loaded[j]);
						skip = true;
						break;
					}
				}
				if (!skip)
				{   // if texture hasn't been loaded already, load it
					textureAssimp texture;
					texture.id = TextureFromFile(str.C_Str(), directory);
					texture.type = typeName;
					texture.path = str.C_Str();
					textures.push_back(texture);
					textures_loaded.push_back(texture); // add to loaded textures
				}
			}
			return textures;
		}

		unsigned int TextureFromFile(const char* file_name, const std::string& directory)
		{
			unsigned int id;
			int width, height, nrChannels;
			std::string file_name_2 = directory + std::string(file_name);
			//stbi_set_flip_vertically_on_load(true);
			unsigned char* dataDiffuse = stbi_load(
				file_name_2.c_str(),
				&width,
				&height,
				&nrChannels,
				0);
			assert(dataDiffuse);

			glGenTextures(1, &id);
			IsError(__FILE__, __LINE__);
			glBindTexture(GL_TEXTURE_2D, id);
			IsError(__FILE__, __LINE__);
			if (nrChannels == 1)
			{
				glTexImage2D(
					GL_TEXTURE_2D,
					0,
					GL_RED,
					width,
					height,
					0,
					GL_RED,
					GL_UNSIGNED_BYTE,
					dataDiffuse);
				IsError(__FILE__, __LINE__);
			}
			if (nrChannels == 3)
			{
				glTexImage2D(
					GL_TEXTURE_2D,
					0,
					GL_RGB,
					width,
					height,
					0,
					GL_RGB,
					GL_UNSIGNED_BYTE,
					dataDiffuse);
				IsError(__FILE__, __LINE__);
			}
			if (nrChannels == 4)
			{
				glTexImage2D(
					GL_TEXTURE_2D,
					0,
					GL_RGBA,
					width,
					height,
					0,
					GL_RGBA,
					GL_UNSIGNED_BYTE,
					dataDiffuse);
				IsError(__FILE__, __LINE__);
			}
			glTexParameteri(
				GL_TEXTURE_2D,
				GL_TEXTURE_WRAP_S,
				GL_MIRRORED_REPEAT);
			IsError(__FILE__, __LINE__);
			glTexParameteri(
				GL_TEXTURE_2D,
				GL_TEXTURE_WRAP_T,
				GL_MIRRORED_REPEAT);
			IsError(__FILE__, __LINE__);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			IsError(__FILE__, __LINE__);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			IsError(__FILE__, __LINE__);
			glGenerateMipmap(GL_TEXTURE_2D);
			IsError(__FILE__, __LINE__);
			glBindTexture(GL_TEXTURE_2D, 0);
			IsError(__FILE__, __LINE__);

			return id;
		}

		MeshAssimp GetMesh(unsigned int i){
			return meshes[i];
		}
		std::string path;
		std::vector<textureAssimp> textures_loaded;
		std::vector<MeshAssimp> meshes;
		std::vector<Material> materials;
	};
}
