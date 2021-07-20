#pragma once

#include <string>
#include <glad/glad.h>
#include <stdexcept>

namespace gl {
	struct TextureAssimp
	{
		unsigned int id;
		std::string type;
		std::string path;
	};

} // End namespace gl.
