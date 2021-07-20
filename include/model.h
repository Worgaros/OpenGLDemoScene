#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "texture.h"
#include "mesh.h"

namespace gl
{
	class Model {
	public:
		std::string directory = "../data/textures/scene_textures/";
		Model(const std::string& filename);
		void processNode(aiNode* node, const aiScene* scene);
		MeshAssimp processMesh(aiMesh* mesh, const aiScene* scene);
		std::vector<textureAssimp> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
		unsigned int TextureFromFile(const char* file_name, const std::string& directory);
		MeshAssimp GetMesh(unsigned int i);
		
		std::string path;
		std::vector<textureAssimp> textures_loaded;
		std::vector<MeshAssimp> meshes;
	};
}
