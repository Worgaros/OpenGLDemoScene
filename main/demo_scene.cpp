#include <SDL_main.h>
#include <glad/glad.h>
#include <string>
#include <iostream>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "engine.h"
#include "camera.h"
#include "mesh2.h"
#include "texture.h"
#include "shader.h"
#include "model.h"

namespace gl {

	class HelloScene : public Program
	{
	public:
		void Init() override;
		void Update(seconds dt) override;
		void Destroy() override;
		void OnEvent(SDL_Event& event) override;
		void DrawImGui() override;

	protected:
		void SetModelMatrix(seconds dt);
		void SetViewMatrix(seconds dt);
		void SetProjectionMatrix();
		void IsError(const std::string& file, int line) const;
		void SetUniformMatrix() const;

		unsigned int VAO_;
		unsigned int VBO_;
		unsigned int EBO_;
		unsigned int vertex_shader_;
		unsigned int fragment_shader_;
		unsigned int program_;

		bool firstMouse = true;
		int lastX = 0;
		int lastY = 0;
		float fov_ = 45.0f;

		float time_ = 0.0f;
		float delta_time_ = 0.0f;

		std::unique_ptr<Model> modelObj_ = nullptr;
		std::unique_ptr<Camera> camera_ = nullptr;
		std::unique_ptr<Texture> texture_diffuse_ = nullptr;
		std::unique_ptr<Texture> texture_specular_ = nullptr;
		std::unique_ptr<Shader> shaders_ = nullptr;
		std::unique_ptr<Shader> outline_shaders_ = nullptr;

		glm::mat4 model_ = glm::mat4(1.0f);
		glm::mat4 inv_model_ = glm::mat4(1.0f);
		glm::mat4 view_ = glm::mat4(1.0f);
		glm::mat4 projection_ = glm::mat4(1.0f);
	};

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

		camera_ = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 20.0f));

		std::string path = "../";

		modelObj_ = std::make_unique<Model>(path + "data/meshes/scene_models/game.obj");
		
		shaders_ = std::make_unique<Shader>(
			path + "data/shaders/hello_scene/scene.vert",
			path + "data/shaders/hello_scene/scene.frag");

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		IsError(__FILE__, __LINE__);
	}

	void HelloScene::SetModelMatrix(seconds dt)
	{
		model_ = glm::rotate(glm::mat4(1.0f), time_, glm::vec3(0.f, 1.f, 0.f));
		inv_model_ = glm::transpose(glm::inverse(model_));
	}

	void HelloScene::SetViewMatrix(seconds dt)
	{
		view_ = camera_->GetViewMatrix();
	}

	void HelloScene::SetProjectionMatrix()
	{
		projection_ = glm::perspective(glm::radians(fov_), 4.0f / 3.0f, 0.1f, 100.f);
	}

	void HelloScene::SetUniformMatrix() const
	{
		shaders_->Use();
		shaders_->SetMat4("model", model_);
		shaders_->SetMat4("view", view_);
		shaders_->SetMat4("projection", projection_);
		shaders_->SetMat4("inv_model", inv_model_);
		shaders_->SetVec3("camera_pos", camera_->position);
	}

	void HelloScene::Update(seconds dt)
	{
		delta_time_ = dt.count();
		time_ += delta_time_;
		SetViewMatrix(dt);
		SetModelMatrix(dt);
		SetProjectionMatrix();
		SetUniformMatrix();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		IsError(__FILE__, __LINE__);
		for (Mesh2& mesh : modelObj_->meshes)
		{
			mesh.Bind();
			auto& material = modelObj_->materials[mesh.material_index_];
			shaders_->Use();
			material.color.Bind(0);
			shaders_->SetInt("Diffuse", 0);
			material.specular.Bind(1);
			shaders_->SetInt("Specular", 1);
			glDrawElements(GL_TRIANGLES, mesh.nb_vertices_, GL_UNSIGNED_INT, 0);
		}
	}

	void HelloScene::Destroy()
	{
	}

	void HelloScene::OnEvent(SDL_Event& event)
	{
		if (event.type == SDL_KEYDOWN)
		{
			if (event.key.keysym.sym == SDLK_ESCAPE)
				exit(0);
			if (event.key.keysym.sym == SDLK_w)
			{
				camera_->ProcessKeyboard(CameraMovementEnum::UP, delta_time_);
			}
			if (event.key.keysym.sym == SDLK_s)
			{
				camera_->ProcessKeyboard(CameraMovementEnum::DOWN, delta_time_);
			}
			if (event.key.keysym.sym == SDLK_a)
			{
				camera_->ProcessKeyboard(CameraMovementEnum::LEFT, delta_time_);
			}
			if (event.key.keysym.sym == SDLK_d)
			{
				camera_->ProcessKeyboard(CameraMovementEnum::RIGHT, delta_time_);
			}
		}
		if (event.type == SDL_MOUSEMOTION)
		{
			int x;
			int y;
			SDL_GetMouseState(&x, &y);
			if (firstMouse)
			{
				lastX = x;
				lastY = y;
				firstMouse = false;
			}
			float currX = event.motion.x;
			float currY = event.motion.y;
			float xOffset = currX - lastX;
			float yOffset = lastY - currY;
			lastX = currX;
			lastY = currY;
			camera_->ProcessMouseMovement(xOffset, yOffset, time_);
		}
		if (event.type == SDL_MOUSEWHEEL)
		{
			fov_ -= event.wheel.y;
			if (fov_ < 1.0f)
				fov_ = 1.0f;
			if (fov_ > 45.0f)
				fov_ = 45.0f;
		}
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
