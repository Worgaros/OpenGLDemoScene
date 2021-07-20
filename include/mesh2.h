#pragma once

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>

namespace gl {
	class Vertex
	{
	public:
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texture;
	};

	class Mesh2 {
	public:
		Mesh2(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, const unsigned int material_id) : vertices_(vertices), indices_(indices), material_index_(material_id)
		{
			Init();
		}

		void Init()
		{
			nb_vertices_ = indices_.size();
			// VAO binding should be before VAO.
			glGenVertexArrays(1, &VAO_);
			IsError(__FILE__, __LINE__);
			glBindVertexArray(VAO_);
			IsError(__FILE__, __LINE__);

			// EBO.
			glGenBuffers(1, &EBO_);
			IsError(__FILE__, __LINE__);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
			IsError(__FILE__, __LINE__);
			glBufferData(
				GL_ELEMENT_ARRAY_BUFFER,
				indices_.size() * sizeof(float),
				indices_.data(),
				GL_STATIC_DRAW);
			IsError(__FILE__, __LINE__);

			// VBO.
			glGenBuffers(1, &VBO_);
			IsError(__FILE__, __LINE__);
			glBindBuffer(GL_ARRAY_BUFFER, VBO_);
			IsError(__FILE__, __LINE__);
			glBufferData(
				GL_ARRAY_BUFFER,
				vertices_.size() * sizeof(Vertex),
				vertices_.data(),
				GL_STATIC_DRAW);
			IsError(__FILE__, __LINE__);

			GLintptr vertex_color_offset = 3 * sizeof(float);
			GLintptr vertex_tex_offset = 6 * sizeof(float);
			glVertexAttribPointer(
				0,
				3,
				GL_FLOAT,
				GL_FALSE,
				8 * sizeof(float),
				0);
			IsError(__FILE__, __LINE__);
			glVertexAttribPointer(
				1,
				3,
				GL_FLOAT,
				GL_FALSE,
				8 * sizeof(float),
				(GLvoid*)vertex_color_offset);
			IsError(__FILE__, __LINE__);
			glVertexAttribPointer(
				2,
				2,
				GL_FLOAT,
				GL_FALSE,
				8 * sizeof(float),
				(GLvoid*)vertex_tex_offset);
			IsError(__FILE__, __LINE__);
			glEnableVertexAttribArray(0);
			IsError(__FILE__, __LINE__);
			glEnableVertexAttribArray(1);
			IsError(__FILE__, __LINE__);
			glEnableVertexAttribArray(2);
			IsError(__FILE__, __LINE__);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			IsError(__FILE__, __LINE__);
		}

		void IsError(const std::string& file, int line) const
		{
			auto error_code = glGetError();
			if (error_code != GL_NO_ERROR)
			{
				std::cerr
					<< error_code
					<< " in file: " << file
					<< " at line: " << line
					<< "\n";
			}
		}

		void Bind()
		{
			glBindVertexArray(VAO_);
			IsError(__FILE__, __LINE__);
		}

		unsigned int GetVAO()
		{
			return VAO_;
		}

		unsigned int material_index_ = 0;
		unsigned int nb_vertices_ = 0;
		std::vector<Vertex> vertices_;
		unsigned int VAO_ = 0;
		unsigned int VBO_ = 0;
		unsigned int EBO_ = 0;
		std::vector<uint32_t> indices_;
	};
} // End namespace gl.
