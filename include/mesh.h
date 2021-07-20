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
		std::vector<textureAssimp> textures;
		VertexAssimp vertex;
		unsigned int VAO, VBO, EBO;
		unsigned int material_index_ = 0;
		std::vector<VertexAssimp> vertices_;
		std::vector<uint32_t> indices_;

		MeshAssimp(std::vector<VertexAssimp> vertices, std::vector<unsigned int> indices, std::vector<textureAssimp> textures)
		{
			this->vertices = vertices;
			this->indices = indices;
			this->textures = textures;

			setupMesh();
		}

		void setupMesh()
		{
			glGenVertexArrays(1, &VAO);
			IsError(__FILE__, __LINE__);
			glGenBuffers(1, &VBO);
			IsError(__FILE__, __LINE__);
			glGenBuffers(1, &EBO);
			IsError(__FILE__, __LINE__);

			glBindVertexArray(VAO);
			IsError(__FILE__, __LINE__);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			IsError(__FILE__, __LINE__);

			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexAssimp), &vertices[0], GL_STATIC_DRAW);
			IsError(__FILE__, __LINE__);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			IsError(__FILE__, __LINE__);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
				&indices[0], GL_STATIC_DRAW);
			IsError(__FILE__, __LINE__);

			// vertex positions
			glEnableVertexAttribArray(1);
			IsError(__FILE__, __LINE__);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAssimp), (void*)0);
			IsError(__FILE__, __LINE__);
			// vertex normals
			glEnableVertexAttribArray(2);
			IsError(__FILE__, __LINE__);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAssimp), (void*)offsetof(VertexAssimp, normal));
			IsError(__FILE__, __LINE__);
			// vertex texture coords
			glEnableVertexAttribArray(3);
			IsError(__FILE__, __LINE__);
			glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(VertexAssimp), (void*)offsetof(VertexAssimp, texture));
			IsError(__FILE__, __LINE__);
			// vertex tangents
			glEnableVertexAttribArray(4);
			IsError(__FILE__, __LINE__);
			glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAssimp), (void*)offsetof(VertexAssimp, tangents));
			IsError(__FILE__, __LINE__);
			// vertex bitangents
			glEnableVertexAttribArray(5);
			IsError(__FILE__, __LINE__);
			glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAssimp), (void*)offsetof(VertexAssimp, bitangents));
			IsError(__FILE__, __LINE__);

			glBindVertexArray(0);
			IsError(__FILE__, __LINE__);
		}

		void BindTexture(Shader& shader)
		{
			// bind appropriate textures
			unsigned int diffuseNr = 1;
			unsigned int specularNr = 1;
			unsigned int normalNr = 1;
			for (unsigned int i = 0; i < textures.size(); i++)
			{
				glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
				IsError(__FILE__, __LINE__);
				// retrieve texture number (the N in diffuse_textureN)
				std::string number;
				std::string name = textures[i].type;
				if (name == "Diffuse")
					number = std::to_string(diffuseNr++);
				else if (name == "Specular")
					number = std::to_string(specularNr++); // transfer unsigned int to stream
				else if (name == "Normal")
					number = std::to_string(normalNr++); // transfer unsigned int to stream

				// now set the sampler to the correct texture unit
				glUniform1i(glGetUniformLocation(shader.id, (name + number).c_str()), i);
				IsError(__FILE__, __LINE__);
				// and finally bind the texture
				glBindTexture(GL_TEXTURE_2D, textures[i].id);
				IsError(__FILE__, __LINE__);
			}
		}
		
		void Draw(Shader& shader)
		{
			BindTexture(shader);
			// draw mesh
			glBindVertexArray(VAO);
			IsError(__FILE__, __LINE__);
			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
			IsError(__FILE__, __LINE__);
			glBindVertexArray(0);
			IsError(__FILE__, __LINE__);

			// always good practice to set everything back to defaults once configured.
			glActiveTexture(GL_TEXTURE0);
			IsError(__FILE__, __LINE__);
		}

		void DrawInstancing(Shader& shader, MeshAssimp meshInstancing, unsigned int VBO_instances_, std::vector<glm::mat4> modelMatrix_)
		{
			glBindVertexArray(meshInstancing.GetVAO());
			IsError(__FILE__, __LINE__);
			glBindBuffer(GL_ARRAY_BUFFER, VBO_instances_);
			IsError(__FILE__, __LINE__);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * modelMatrix_.size(), &modelMatrix_[0], GL_STATIC_DRAW);
			IsError(__FILE__, __LINE__);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			IsError(__FILE__, __LINE__);
			glDrawElementsInstanced(GL_TRIANGLES, vertices.size(), GL_UNSIGNED_INT, 0, modelMatrix_.size());
			IsError(__FILE__, __LINE__);
			glBindVertexArray(0);
			IsError(__FILE__, __LINE__);
		}
		
		void Bind()
		{
			glBindVertexArray(VAO);
			IsError(__FILE__, __LINE__);
		}
		
		unsigned int GetVAO()
		{
			return VAO;
		}
	};
}// End namespace gl.
