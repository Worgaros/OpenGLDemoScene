#pragma once

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "shader.h"
#include "error.h"
#include "texture.h"

namespace gl {
	class VertexAssimp
	{
	public:
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texture;
		glm::vec3 tangents;
		glm::vec3 bitangents;
	};

	class MeshAssimp {
	public:
		// mesh data
		std::vector<VertexAssimp> vertices;
		std::vector<unsigned int> indices;
		std::vector<TextureAssimp> textures;
		VertexAssimp vertex;
		unsigned int VAO, VBO, EBO;
		unsigned int material_index_ = 0;
		std::vector<VertexAssimp> vertices_;
		std::vector<uint32_t> indices_;

		MeshAssimp(std::vector<VertexAssimp> vertices, std::vector<unsigned int> indices, std::vector<TextureAssimp> textures);
		void setupMesh();
		void BindTexture(Shader& shader);
		void Draw(Shader& shader);
		void DrawInstancing(Shader& shader, MeshAssimp meshInstancing, unsigned int VBO_instances_, std::vector<glm::mat4> modelMatrix_);
		void Bind();
		unsigned int GetVAO();
	};
}// End namespace gl.
