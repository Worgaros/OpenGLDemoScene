#pragma once

#include <memory>
#include "texture.h"

namespace gl
{
	class Material {
	public:
		Texture color;
		Texture specular;
		glm::vec3 specular_strength;
		int specular_pow;
	};
}