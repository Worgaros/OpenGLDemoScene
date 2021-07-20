#include <algorithm>
#include <SDL_main.h>
#include <glad/glad.h>
#include <string>
#include <iostream>
#include <vector>
#include <array>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include "engine.h"
#include "camera.h"
#include "meshi.h"
#include "texturi.h"
#include "shader.h"
#include "modeli.h"

namespace gl {

	class HelloScene : public Program
	{
	public:
		void Init() override;
		void Update(seconds dt) override;
		void OnEvent(SDL_Event& event) override;
		void DrawImGui() override;
		void Destroy() override;

	protected:
		void SetModelMatrix();
		void SetRotationMatrix();
		void SetViewMatrix(seconds dt);
		void SetProjectionMatrix();
		void IsError(const std::string& file, int line) const;
		void SetUniformMatrixNormal() const;
		void SetUniformMatrixInstancing() const;
		void SetUniformMatrixBlending() const;
		void SetUniformMatrixOutline() const;
		void SetUniformMatrixBasic() const;
		void SetModelMatrixInstancing(seconds dt, unsigned int i);
		glm::vec3 GetPositionInstancing(unsigned int i);

		unsigned int skybox_VAO_;
		unsigned int outline_VAO_;
		unsigned int VBO_;
		unsigned int skybox_VBO_;
		unsigned int EBO_;
		unsigned int vertex_shader_;
		unsigned int fragment_shader_;
		unsigned int program_;
		unsigned int cubemap_texture_;

		bool firstMouse = true;
		int lastX = 0;
		int lastY = 0;
		float fov_ = 45.0f;

		float time_ = 0.0f;
		float delta_time_ = 0.0f;

		std::vector<std::string> faces_
		{
			"../data/textures/skybox/right.jpg",
			"../data/textures/skybox/left.jpg",
			"../data/textures/skybox/top.jpg",
			"../data/textures/skybox/bottom.jpg",
			"../data/textures/skybox/front.jpg",
			"../data/textures/skybox/back.jpg"
		};

		std::array<float, 108> skybox_vertices_ = {
			// positions          
			-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
		};
		
		std::unique_ptr<Model> normal_obj = nullptr;
		std::unique_ptr<Model> outline_obj = nullptr;
		std::unique_ptr<Model> blending_obj = nullptr;
		std::unique_ptr<Camera> camera_ = nullptr;
		std::unique_ptr<Shader> default_shaders_ = nullptr;
		std::unique_ptr<Shader> normal_shaders_ = nullptr;
		std::unique_ptr<Shader> cubemaps_shaders_ = nullptr;
		std::unique_ptr<Shader> outline_shaders_ = nullptr;
		std::unique_ptr<Shader> instancing_shaders_ = nullptr;
		std::unique_ptr<Shader> blending_shaders_ = nullptr;
		
		glm::mat4 normal_model = glm::mat4(1.0f);
		glm::mat4 instancing_model = glm::mat4(1.0f);
		glm::mat4 outline_model = glm::mat4(1.0f);
		glm::mat4 blending_model = glm::mat4(1.0f);
		glm::mat4 view_ = glm::mat4(1.0f);
		glm::mat4 projection_ = glm::mat4(1.0f);

		float thickness_instances_x_ = 1000.0f;
		float thickness_instances_y_ = 1000.0f;
		float thickness_instances_z_ = 1000.0f;
		int density_instances_ = 100;
		unsigned int VBO_instances_;
		std::unique_ptr<Model> instancing_obj = nullptr;
		glm::vec3 transVec_ = glm::vec3(0.0f, 0.0f, 0.0f);
		std::vector<glm::mat4> modelMatrix_; // taille pos chaque model
		unsigned int nbr_instances_ = 3000;
		std::vector<float> initDistanceX_;
		std::vector<float> initDistanceY_;
		std::vector<float> initDistanceZ_;
	};

	glm::vec3 HelloScene::GetPositionInstancing(unsigned i) {
		glm::vec3 scale;
		glm::quat rotation;
		glm::vec3 translation;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(modelMatrix_[i], scale, rotation, translation, skew, perspective);

		return translation;
	}

	void HelloScene::SetModelMatrixInstancing(seconds dt, unsigned i) {
		modelMatrix_[i] = glm::mat4(1.0f);
		modelMatrix_[i] = glm::translate(modelMatrix_[i], transVec_ + glm::vec3(initDistanceX_[i], initDistanceY_[i], initDistanceZ_[i]));
	}

	unsigned int loadCubemap(std::vector<std::string> faces)
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

	void HelloScene::IsError(const std::string& file, int line) const
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

	void HelloScene::Init()
	{
		glEnable(GL_DEPTH_TEST);
		IsError(__FILE__, __LINE__);
		glDepthFunc(GL_LESS);
		glEnable(GL_BLEND);
		IsError(__FILE__, __LINE__);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		IsError(__FILE__, __LINE__);
		std::string path = "../";

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
		
		normal_obj = std::make_unique<Model>(path + "data/meshes/cubenormal.obj");
		instancing_obj = std::make_unique<Model>(path + "data/meshes/rock.obj");
		blending_obj = std::make_unique<Model>(path + "data/meshes/timer.obj");
		outline_obj = std::make_unique<Model>(path + "data/meshes/F1.obj");
		
		camera_ = std::make_unique<Camera>(glm::vec3(.0, 0.0, 800.0));

		normal_shaders_ = std::make_unique<Shader>(
			path + "data/shaders/normal/normal.vert",
			path + "data/shaders/normal/normal.frag");

		cubemaps_shaders_ = std::make_unique<Shader>(
			path + "data/shaders/cubemaps/cubemaps.vert",
			path + "data/shaders/cubemaps/cubemaps.frag");

		blending_shaders_ = std::make_unique<Shader>(
			path + "data/shaders/blending/blending.vert",
			path + "data/shaders/blending/blending.frag");

		instancing_shaders_ = std::make_unique<Shader>(
			path + "data/shaders/instancing/instancing.vert",
			path + "data/shaders/instancing/instancing.frag");

		default_shaders_ = std::make_unique<Shader>(
			path + "data/shaders/default/default.vert",
			path + "data/shaders/default/default.frag");

		outline_shaders_ = std::make_unique<Shader>(
			path + "data/shaders/outline/outline.vert",
			path + "data/shaders/outline/outline.frag");

		cubemap_texture_ = loadCubemap(faces_);

		SetModelMatrix();

		// instancing init
		initDistanceX_.resize(nbr_instances_);
		std::generate(initDistanceX_.begin(), initDistanceX_.end(), [&]() {return (std::rand() % density_instances_) / (density_instances_ / thickness_instances_x_) - (thickness_instances_x_ / 2.0f); });
		initDistanceY_.resize(nbr_instances_);
		std::generate(initDistanceY_.begin(), initDistanceY_.end(), [&]() {return (std::rand() % density_instances_) / (density_instances_ / thickness_instances_y_) - (thickness_instances_y_ / 2.0f); });
		initDistanceZ_.resize(nbr_instances_);
		std::generate(initDistanceZ_.begin(), initDistanceZ_.end(), [&]() {return (std::rand() % density_instances_) / (density_instances_ / thickness_instances_z_) - (thickness_instances_z_ / 2.0f); });
		modelMatrix_.resize(nbr_instances_, glm::mat4(1.0f));
		instancing_obj = std::make_unique<Model>(path + "data/meshes/rock.obj");
		auto instancing_mesh_ = instancing_obj->GetMesh(0);
		glBindVertexArray(instancing_mesh_.GetVAO());
		IsError(__FILE__, __LINE__);

		//VBO
		glGenBuffers(1, &VBO_instances_);
		IsError(__FILE__, __LINE__);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_instances_);
		IsError(__FILE__, __LINE__);
		glEnableVertexAttribArray(6);
		IsError(__FILE__, __LINE__);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)0);
		IsError(__FILE__, __LINE__);
		glEnableVertexAttribArray(7);
		IsError(__FILE__, __LINE__);
		glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(1 * sizeof(glm::vec4)));
		IsError(__FILE__, __LINE__);
		glEnableVertexAttribArray(8);
		IsError(__FILE__, __LINE__);
		glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(2 * sizeof(glm::vec4)));
		IsError(__FILE__, __LINE__);
		glEnableVertexAttribArray(9);
		IsError(__FILE__, __LINE__);
		glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(3 * sizeof(glm::vec4)));
		IsError(__FILE__, __LINE__);

		glVertexAttribDivisor(6, 1);
		IsError(__FILE__, __LINE__);
		glVertexAttribDivisor(7, 1);
		IsError(__FILE__, __LINE__);
		glVertexAttribDivisor(8, 1);
		IsError(__FILE__, __LINE__);
		glVertexAttribDivisor(9, 1);
		IsError(__FILE__, __LINE__);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		IsError(__FILE__, __LINE__);
		glBindVertexArray(0);
		IsError(__FILE__, __LINE__);

		// init stencil
		glEnable(GL_STENCIL_TEST);
		IsError(__FILE__, __LINE__);
		glStencilFunc(GL_NOTEQUAL, 1, 0xff);
		IsError(__FILE__, __LINE__);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		IsError(__FILE__, __LINE__);
	}

	void HelloScene::SetModelMatrix()
	{
		normal_model = glm::translate(normal_model, glm::vec3(0.0f, 0.0f, -5.0f));
		outline_model = glm::translate(outline_model, glm::vec3(5.0f, 0.0f, -2.0f));
		outline_model = glm::scale(outline_model, glm::vec3(0.05f));
		blending_model = glm::translate(blending_model, glm::vec3(-5.0f, -0.5f, -5.0f));
		blending_model = glm::scale(blending_model, glm::vec3(5.0f));
	}

	void HelloScene::SetRotationMatrix()
	{
		normal_model = glm::rotate(normal_model, glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		outline_model = glm::rotate(outline_model, glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		blending_model = glm::rotate(blending_model, glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	void HelloScene::SetViewMatrix(seconds dt)
	{
		view_ = camera_->GetViewMatrix();
	}

	void HelloScene::SetProjectionMatrix()
	{
		projection_ = glm::perspective(glm::radians(fov_), 4.0f / 3.0f, 0.1f, 1000.f);
	}

	void HelloScene::SetUniformMatrixBasic() const
	{
		default_shaders_->Use();
		default_shaders_->SetMat4("view", view_);
		default_shaders_->SetMat4("model", outline_model);
		default_shaders_->SetMat4("projection", projection_);
		default_shaders_->SetVec3("viewPos", camera_->position);
	}

	void HelloScene::SetUniformMatrixNormal() const
	{
		normal_shaders_->Use();
		normal_shaders_->SetMat4("view", view_);
		normal_shaders_->SetMat4("model", normal_model);
		normal_shaders_->SetMat4("projection", projection_);
		normal_shaders_->SetVec3("viewPos", camera_->position);
	}

	void HelloScene::SetUniformMatrixInstancing() const
	{
		instancing_shaders_->Use();
		instancing_shaders_->SetMat4("view", view_);
		instancing_shaders_->SetMat4("model", instancing_model);
		instancing_shaders_->SetMat4("projection", projection_);
		instancing_shaders_->SetVec3("viewPos", camera_->position);
	}

	void HelloScene::SetUniformMatrixBlending() const
	{
		blending_shaders_->Use();
		blending_shaders_->SetMat4("view", view_);
		blending_shaders_->SetMat4("model", blending_model);
		blending_shaders_->SetMat4("projection", projection_);
		blending_shaders_->SetVec3("viewPos", camera_->position);
	}

	void HelloScene::Update(seconds dt)
	{
		delta_time_ = dt.count();
		time_ += delta_time_;
		if (time_ >= 10.0f && time_ < 20.0f)
			camera_->SetState(glm::vec3(-5.0f, 0.2f, 0.0f), camera_->pitch, camera_->yaw);
		else if (time_ >= 20.0f && time_ < 30.0f)
			camera_->SetState(glm::vec3(0.0f, 0.2f, 0.0f), camera_->pitch, camera_->yaw);
		else if (time_ >= 30.0f && time_ < 40.0f)
			camera_->SetState(glm::vec3(5.0f, 0.2f, 0.0f), camera_->pitch, camera_->yaw);
		else if (time_ >= 40.0f && time_ < 50.0f)
			camera_->SetState(glm::vec3(0.0f, 0.0f, 800.0f), camera_->pitch, camera_->yaw);
		else if (time_ >= 50.0f)
			time_ = 10.0f;
		SetViewMatrix(dt);
		SetProjectionMatrix();
		SetRotationMatrix();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		SetUniformMatrixNormal();
		for (MeshAssimp& mesh : normal_obj->meshes)
		{
			mesh.Draw(*normal_shaders_);
		}
		
		// INSTANCING
		SetUniformMatrixInstancing();
		MeshAssimp meshInstancing = instancing_obj->GetMesh(0);
		meshInstancing.Bind();
		meshInstancing.BindTexture(*instancing_shaders_);
		for (unsigned int i = 0; i < modelMatrix_.size(); i++)
		{
			SetModelMatrixInstancing(dt, i);
		}
		glBindBuffer(GL_ARRAY_BUFFER, VBO_instances_);
		IsError(__FILE__, __LINE__);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * modelMatrix_.size(), &modelMatrix_[0], GL_STATIC_DRAW);
		IsError(__FILE__, __LINE__);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		IsError(__FILE__, __LINE__);
		glBindVertexArray(meshInstancing.GetVAO());
		IsError(__FILE__, __LINE__);
		glDrawElementsInstanced(GL_TRIANGLES, meshInstancing.indices.size(), GL_UNSIGNED_INT, 0, modelMatrix_.size());
		IsError(__FILE__, __LINE__);
		glBindVertexArray(0);
		IsError(__FILE__, __LINE__);
		
		// SKYBOX
		glDepthFunc(GL_LEQUAL);
		IsError(__FILE__, __LINE__);
		cubemaps_shaders_->Use();
		cubemaps_shaders_->SetInt("skybox", 0);
		// Remove translation from the view matrix.
		view_ = glm::mat4(glm::mat3(camera_->GetViewMatrix()));
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
		SetViewMatrix(dt);
		// OUTLINE
		glStencilFunc(GL_ALWAYS, 1, 0xff);
		IsError(__FILE__, __LINE__);
		glStencilMask(0xff);
		IsError(__FILE__, __LINE__);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, outline_obj->GetMesh(0).EBO);
		IsError(__FILE__, __LINE__);
		glBindVertexArray(outline_obj->GetMesh(0).VAO);
		IsError(__FILE__, __LINE__);
		SetUniformMatrixBasic();
		outline_obj->GetMesh(0).BindTexture(*default_shaders_);
		glDrawElements(GL_TRIANGLES, outline_obj->GetMesh(0).indices.size(), GL_UNSIGNED_INT, 0);
		IsError(__FILE__, __LINE__);
		glStencilFunc(GL_NOTEQUAL, 1, 0xff);
		IsError(__FILE__, __LINE__);
		glStencilMask(0x00);
		IsError(__FILE__, __LINE__);
		glDisable(GL_DEPTH_TEST);
		IsError(__FILE__, __LINE__);
		float scale = 1.1f;
		outline_shaders_->Use();
		glm::mat4 model = outline_model;
		model = glm::scale(model, glm::vec3(scale, scale, scale));
		outline_shaders_->SetMat4("model", model);
		outline_shaders_->SetMat4("view", view_);
		outline_shaders_->SetMat4("projection", projection_);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, outline_obj->GetMesh(0).EBO);
		IsError(__FILE__, __LINE__);
		glBindVertexArray(outline_obj->GetMesh(0).VAO);
		IsError(__FILE__, __LINE__);
		glDrawElements(GL_TRIANGLES, outline_obj->GetMesh(0).indices.size(), GL_UNSIGNED_INT, 0);
		IsError(__FILE__, __LINE__);
		glStencilMask(0xff);
		IsError(__FILE__, __LINE__);
		glStencilFunc(GL_ALWAYS, 0, 0xff);
		IsError(__FILE__, __LINE__);
		glEnable(GL_DEPTH_TEST);
		IsError(__FILE__, __LINE__);
		// BLENDING
		glEnable(GL_BLEND);
		IsError(__FILE__, __LINE__);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		IsError(__FILE__, __LINE__);
		SetUniformMatrixBlending();
		for (MeshAssimp& mesh : blending_obj->meshes)
		{
			mesh.Draw(*blending_shaders_);
		}
	}

	void HelloScene::OnEvent(SDL_Event& event)
	{
		if (event.type == SDL_KEYDOWN)
		{
			if (event.key.keysym.sym == SDLK_ESCAPE)
				exit(0);
		}
	}

	void HelloScene::Destroy()
	{
	}

	void HelloScene::DrawImGui()
	{
	}

} // End namespace gl.

int main(int argc, char** argv)
{
	gl::HelloScene program;
	gl::Engine engine(program);
	engine.Run();
	return EXIT_SUCCESS;
}
