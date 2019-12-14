#include "wavefront.h"
#include <fmt/format.h>
#include <min-ray/mesh.h>
#include <tiny_obj_loader.h>
#include "../shaders/common.h"

namespace min::ray {

void split(const std::string &src, const char delim, std::vector<std::string> &result) {
  std::string s;
  for (size_t i = 0; i < src.length();) {
    if (src[i] == delim) {
      result.push_back(s);
      s.clear();
      while (i < src.length() && src[i] == delim) {
        i++;
      }
    } else {
      s += src[i];
      i++;
    }
  }
  if (!s.empty()) {
    result.push_back(s);
  }
}

std::shared_ptr<Material> ConvertFromMTL(const tinyobj::material_t &mat) {
  auto material = std::make_shared<Material>();
  auto kd = Vector3(mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]);
  auto ks = Vector3(mat.specular[0], mat.specular[1], mat.specular[2]);
  auto emission = Vector3(mat.emission[0], mat.emission[1], mat.emission[2]);
  //auto diffuse = std::make_shared<DiffuseBSDF>(std::make_shared<RGBShader>(kd));
  //auto specular = std::make_shared<DiffuseBSDF>(std::make_shared<RGBShader>(ks));
  //auto mixed = std::make_shared<MixBSDF>(std::make_shared<FloatShader>(0.5f), diffuse, specular);
  //material->bsdf = mixed;
  material->emission = std::make_shared<RGBShader>(emission);
  return material;
}

MeshImportResult WavefrontImporter::Import(const fs::path &path) {
  fmt::print("Importing {}...\n", path.string());
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn;
  std::string err;
  MeshImportResult result;
  bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str());
  if (!ret) {
    fmt::print("Error loading {} : {}", path.string(), err);
    return result;
  }
  auto mesh = std::make_shared<Mesh>();
  mesh->filename = path.string();
  mesh->vertex_data.positions.reserve(attrib.vertices.size());
  for (size_t i = 0; i < attrib.vertices.size(); i++) {
    mesh->vertex_data.positions.emplace_back(
        Vector3(attrib.vertices[i], attrib.vertices[i + 1], attrib.vertices[i + 2]));
  }
  mesh->vertex_data.normals.reserve(attrib.vertices.size());
  mesh->vertex_data.tex_coords.reserve(attrib.vertices.size());
  std::unordered_map<std::string, size_t> name_to_id;
  std::unordered_map<size_t, std::string> id_to_name;
  std::unordered_map<std::string, std::unordered_map<int, std::string>> to_mangled_name;
  for (size_t i = 0; i < shapes.size(); i++) {
    const auto &name = shapes[i].name;
    auto iter = to_mangled_name.find(name);

    if (iter == to_mangled_name.end()) {
      to_mangled_name[name] = {};
    }
    auto &m = to_mangled_name[name];
    size_t count = 0;
    for (size_t f = 0; f < shapes[i].mesh.num_face_vertices.size(); f++) {
      bool new_name = false;
      std::string final_name;

      auto mat_id = shapes[i].mesh.material_ids[f];
      if (m.find(mat_id) == m.end() || mat_id == -1) {
        new_name = true;
        if (mat_id >= 0) {
          final_name = fmt::format("{}:{}", name, materials[mat_id].name);
        } else {
          final_name = fmt::format("{}:part{}", name, count + 1);
        }
      }
      if (new_name) {
        count++;
        m[mat_id] = final_name;
        name_to_id[final_name] = name_to_id.size();
        auto mat = ConvertFromMTL(materials[mat_id]);
        result.materials.emplace_back(mat);
        mesh->named_materials[final_name] = mat;
        id_to_name[name_to_id[final_name]] = final_name;
        fmt::print("Generated {}\n", final_name);
      }
    }
  }
  for (size_t i = 0; i < name_to_id.size(); i++) {
    mesh->names.emplace_back(id_to_name[i]);
  }
  // Loop over shapes
  for (size_t s = 0; s < shapes.size(); s++) {
    // Loop over faces(polygon)
    size_t index_offset = 0;
    for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
      MeshTriangle primitive;
      auto mat_id = shapes[s].mesh.material_ids[f];
      primitive.name_id = name_to_id.at(to_mangled_name.at(shapes[s].name).at(mat_id));
      int fv = shapes[s].mesh.num_face_vertices[f];
      Point3i vertex_indices;
      // Loop over vertices in the face.
      for (size_t v = 0; v < fv; v++) {
        tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
        vertex_indices[v] = idx.vertex_index;
      }
      for (size_t v = 0; v < fv; v++) {
        // access to vertex
        tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
        if (idx.normal_index >= 0) {
          mesh->vertex_data.normals[idx.vertex_index] =
              Vector3(attrib.normals[3 * idx.normal_index + 0],
                      attrib.normals[3 * idx.normal_index + 1],
                      attrib.normals[3 * idx.normal_index + 2]);
        } else {
          // use the last normal (Ng)

          Vector3 e1 = mesh->vertex_data.positions[vertex_indices[2]] -
                       mesh->vertex_data.positions[vertex_indices[0]];
          Vector3 e2 = mesh->vertex_data.positions[vertex_indices[1]] -
                       mesh->vertex_data.positions[vertex_indices[0]];
          mesh->vertex_data.normals[idx.vertex_index] = glm::normalize(cross(e1, e2));
        }
        if (idx.texcoord_index >= 0) {
          mesh->vertex_data.tex_coords[idx.vertex_index] = Point2(
              attrib.texcoords[2 * idx.texcoord_index + 0],
              attrib.texcoords[2 * idx.texcoord_index + 1]);
        } else {
          mesh->vertex_data.tex_coords[idx.vertex_index] = Point2(v > 0, v > 1);
        }
      }
      index_offset += fv;

      // per-face material
      // shapes[s].mesh.material_ids[f];

      primitive.mesh = mesh.get();
      mesh->triangles.emplace_back(primitive);
      mesh->indices.emplace_back(vertex_indices);
    }
  }
  fmt::print("Loaded {} vertices, {} normals, {} primitives\n", mesh->vertex_data.positions.size(),
             mesh->vertex_data.normals.size(), mesh->triangles.size());
  mesh->loaded = true;
  result.mesh = mesh;
}
}  // namespace min::ray