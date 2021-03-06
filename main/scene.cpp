#include <algorithm>
#include <SDL_main.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "scene.h"
#include "mesh.h"
#include "model.h"
#include "texture.h"
#include <string>
#include <vector>
#include <array>
#include <memory>

#include "engine.h"
#include "camera.h"
#include "shader.h"
#include "model.h"
#include "error.h"
#include "cubemap.h"

namespace gl {
	glm::vec3 HelloScene::GetPositionInstancing(unsigned i) {
		glm::vec3 scale;
		glm::quat rotation;
		glm::vec3 translation;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(model_matrix_[i], scale, rotation, translation, skew, perspective);

		return translation;
	}

	void HelloScene::SetModelMatrixInstancing(seconds dt, unsigned i) {
		model_matrix_[i] = glm::mat4(1.0f);
		model_matrix_[i] = glm::translate(model_matrix_[i], trans_vec_ + glm::vec3(init_distanceX_[i], init_distanceY_[i], init_distanceZ_[i]));
	}

	

	void HelloScene::Init()
	{
		// Init depth.
		glEnable(GL_DEPTH_TEST);
		IsError(__FILE__, __LINE__);
		glDepthFunc(GL_LESS);
		IsError(__FILE__, __LINE__);
		// Init blending.
		glEnable(GL_BLEND);
		IsError(__FILE__, __LINE__);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		IsError(__FILE__, __LINE__);
		// Init stencil.
		glEnable(GL_STENCIL_TEST);
		IsError(__FILE__, __LINE__);
		glStencilFunc(GL_NOTEQUAL, 1, 0xff);
		IsError(__FILE__, __LINE__);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		IsError(__FILE__, __LINE__);
		
		normal_obj = std::make_unique<Model>(path + "data/meshes/cubenormal.obj");
		instancing_obj = std::make_unique<Model>(path + "data/meshes/rock.obj");
		blending_obj = std::make_unique<Model>(path + "data/meshes/timer.obj");
		outline_obj = std::make_unique<Model>(path + "data/meshes/F1.obj");
		
		camera_ = std::make_unique<Camera>(glm::vec3(.0, 0.0, 800.0));

		normal_shaders_ = std::make_unique<Shader>(
			path + "data/shaders/normal/normal.vert",
			path + "data/shaders/normal/normal.frag");

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

		SetModelMatrix();

		cubemap_.Init(path);

		// instancing init
		init_distanceX_.resize(nbr_instances_);
		std::generate(init_distanceX_.begin(), init_distanceX_.end(), [&]() {return (std::rand() % density_instances_) / (density_instances_ / thickness_instances_x_) - (thickness_instances_x_ / 2.0f); });
		init_distanceY_.resize(nbr_instances_);
		std::generate(init_distanceY_.begin(), init_distanceY_.end(), [&]() {return (std::rand() % density_instances_) / (density_instances_ / thickness_instances_y_) - (thickness_instances_y_ / 2.0f); });
		init_distanceZ_.resize(nbr_instances_);
		std::generate(init_distanceZ_.begin(), init_distanceZ_.end(), [&]() {return (std::rand() % density_instances_) / (density_instances_ / thickness_instances_z_) - (thickness_instances_z_ / 2.0f); });
		model_matrix_.resize(nbr_instances_, glm::mat4(1.0f));
		instancing_obj = std::make_unique<Model>(path + "data/meshes/rock.obj");
		auto instancing_mesh_ = instancing_obj->GetMesh(0);
		glBindVertexArray(instancing_mesh_.GetVAO());
		IsError(__FILE__, __LINE__);
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
		for (unsigned int i = 0; i < model_matrix_.size(); i++)
		{
			SetModelMatrixInstancing(dt, i);
		}
		glBindBuffer(GL_ARRAY_BUFFER, VBO_instances_);
		IsError(__FILE__, __LINE__);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * model_matrix_.size(), &model_matrix_[0], GL_STATIC_DRAW);
		IsError(__FILE__, __LINE__);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		IsError(__FILE__, __LINE__);
		glBindVertexArray(meshInstancing.GetVAO());
		IsError(__FILE__, __LINE__);
		glDrawElementsInstanced(GL_TRIANGLES, meshInstancing.indices.size(), GL_UNSIGNED_INT, 0, model_matrix_.size());
		IsError(__FILE__, __LINE__);
		glBindVertexArray(0);
		IsError(__FILE__, __LINE__);
		
		// Remove translation from the view matrix.
		view_ = glm::mat4(glm::mat3(camera_->GetViewMatrix()));
		cubemap_.Draw(view_, projection_);
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
