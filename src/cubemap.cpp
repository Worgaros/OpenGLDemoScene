#include "cubemap.h"


#include <stb_image.h>
#include <glad/glad.h>

#include "error.h"

namespace gl
{
	unsigned int Cubemap::LoadCubemap(std::vector<std::string> faces)
	{
		unsigned int textureID;
		glGenTextures(1, &textureID);
		IsError(__FILE__, __LINE__);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
		IsError(__FILE__, __LINE__);

		int width, height, nrChannels;
		for (unsigned int i = 0; i < faces.size(); i++)
		{
			unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				IsError(__FILE__, __LINE__);
				stbi_image_free(data);
			}
			else
			{
				std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
				stbi_image_free(data);
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		IsError(__FILE__, __LINE__);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		IsError(__FILE__, __LINE__);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		IsError(__FILE__, __LINE__);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		IsError(__FILE__, __LINE__);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		IsError(__FILE__, __LINE__);

		return textureID;
	}

	void Cubemap::Init(std::string path)
	{
		cubemap_texture_ = LoadCubemap(faces_);
		cubemaps_shaders_ = std::make_unique<Shader>(
			path + "data/shaders/cubemaps/cubemaps.vert",
			path + "data/shaders/cubemaps/cubemaps.frag");
		// skybox init
		glGenVertexArrays(1, &skybox_VAO_);
		IsError(__FILE__, __LINE__);
		glBindVertexArray(skybox_VAO_);
		IsError(__FILE__, __LINE__);
		glGenBuffers(1, &skybox_VBO_);
		IsError(__FILE__, __LINE__);
		glBindBuffer(GL_ARRAY_BUFFER, skybox_VBO_);
		IsError(__FILE__, __LINE__);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices_), &skybox_vertices_, GL_STATIC_DRAW);
		IsError(__FILE__, __LINE__);
		glEnableVertexAttribArray(10);
		IsError(__FILE__, __LINE__);
		glVertexAttribPointer(10, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		IsError(__FILE__, __LINE__);
	}

	void Cubemap::Draw(glm::mat4 view_, glm::mat4 projection_)
	{
		// SKYBOX
		glDepthFunc(GL_LEQUAL);
		IsError(__FILE__, __LINE__);
		cubemaps_shaders_->Use();
		cubemaps_shaders_->SetInt("skybox", 0);
		
		cubemaps_shaders_->SetMat4("view", view_);
		cubemaps_shaders_->SetMat4("projection", projection_);
		glBindVertexArray(skybox_VAO_);
		IsError(__FILE__, __LINE__);
		glActiveTexture(GL_TEXTURE0);
		IsError(__FILE__, __LINE__);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_texture_);
		IsError(__FILE__, __LINE__);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		IsError(__FILE__, __LINE__);
		glBindVertexArray(0);
		IsError(__FILE__, __LINE__);
		glDepthFunc(GL_LESS); // set depth function back to default
		IsError(__FILE__, __LINE__);
		
	}
}
