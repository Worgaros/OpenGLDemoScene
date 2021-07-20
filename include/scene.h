#pragma once
#include <string>
#include <vector>
#include <array>
#include <memory>

#include "engine.h"
#include "camera.h"
#include "shader.h"
#include "model.h"

namespace gl
{
	class HelloScene : public gl::Program
	{
	public:
		void Init() override;
		void Update(seconds dt) override;
		void OnEvent(SDL_Event& event) override;
		void DrawImGui() override;
		void Destroy() override;
		void SetModelMatrix();
		void SetRotationMatrix();
		void SetViewMatrix(seconds dt);
		void SetProjectionMatrix();
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

		std::string path = "../";
	};
}
