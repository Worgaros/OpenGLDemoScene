#pragma once

#include "mesh2.h"
#include "material.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

namespace gl
{
	class Model {
	public:
		Model(const std::string& filename) {
			tinyobj::ObjReader reader;
			if (!reader.ParseFromFile(filename))
			{
				throw std::runtime_error("cannot load file: " + filename);
			}
			auto& attrib = reader.GetAttrib();
			auto& shapes = reader.GetShapes();
			auto& materials = reader.GetMaterials();
			for (const auto& material : materials)
			{
				ParsMaterial(material);
			}
			for (const auto& shape : shapes)
			{
				ParsShape(shape, attrib);
			}
			
		}

		void ParsShape(const tinyobj::shape_t& shape, const tinyobj::attrib_t& attrib)
		{
			unsigned int material_id = 0;
			std::vector<Vertex> vertices;
			std::vector<std::uint32_t> indices;
			int index_offset = 0;
			for (std::size_t f = 0; f < shape.mesh.num_face_vertices.size(); ++f)
			{
				int fv = shape.mesh.num_face_vertices[f];
				if (fv != 3) throw std::runtime_error("Should be triangles ?");

				// Loop over vertices in the face.
				for (size_t v = 0; v < fv; v++) {
					Vertex vertex{};
					// access to vertex
					tinyobj::index_t idx =
					shape.mesh.indices[index_offset + v];
					vertex.position.x = attrib.vertices[3 * idx.vertex_index + 0];
					vertex.position.y = attrib.vertices[3 * idx.vertex_index + 1];
					vertex.position.z = attrib.vertices[3 * idx.vertex_index + 2];
					vertex.normal.x = attrib.normals[3 * idx.normal_index + 0];
					vertex.normal.y = attrib.normals[3 * idx.normal_index + 1];
					vertex.normal.z = attrib.normals[3 * idx.normal_index + 2];
					vertex.texture.x =
						attrib.texcoords[2 * idx.texcoord_index + 0];
					vertex.texture.y =
						attrib.texcoords[2 * idx.texcoord_index + 1];
					vertices.push_back(vertex);
					indices.push_back(static_cast<int>(indices.size()));
				}
				index_offset += fv;
			}
			material_id = shape.mesh.material_ids[0];
			meshes.emplace_back(vertices, indices, material_id);
		}

		void ParsMaterial(const tinyobj::material_t& material)
		{
			Material mat{};
			const std::string path = "../data/textures/scene_textures/";
			mat.color = Texture(path + material.diffuse_texname);
			mat.specular = Texture(path + material.roughness_texname);
			mat.specular_pow = material.shininess;
			mat.specular_strength = glm::vec3(material.specular[0], material.specular[1], material.specular[2]);
			materials.push_back(mat);
		}

		Mesh2 GetMesh(unsigned int i){
			return meshes[i];
		}
		
		std::vector<Mesh2> meshes;
		std::vector<Material> materials;
	};
}
