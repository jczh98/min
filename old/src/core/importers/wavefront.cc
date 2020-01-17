#include "wavefront.h"
#include <min-ray/mesh.h>
#include <tiny_obj_loader.h>
#include "../bsdfs/diffuse.h"
#include "../textures/constant.h"

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
  // auto ks = Vector3(mat.specular[0], mat.specular[1], mat.specular[2]);
  // std::shared_ptr<Shader> kdMap = nullptr, ksMap = nullptr;
  // if (!mat.diffuse_texname.empty()) {
  //   kdMap = std::make_shared<ImageTextureShader>(mat.diffuse_texname);
  //   kd = Vector3(1);
  // }
  // if (!mat.specular_texname.empty()) {
  //   ksMap = std::make_shared<ImageTextureShader>(mat.specular_texname);
  //   ks = Vector3(1);
  // }

  //auto frac = std::clamp((maxComp(kd) + maxComp(ks)) == 0 ? 1.0 : maxComp(ks) / (maxComp(kd) + maxComp(ks)), 0.0, 1.0);
  auto emission = Vector3(mat.emission[0], mat.emission[1], mat.emission[2]);
  auto strength = MaxComp(emission) == 0.0 ? 0.0 : MaxComp(emission);
  emission = strength == 0.0 ? Vector3(0) : emission / MaxComp(emission);
  auto diffuse = std::make_shared<DiffuseBSDF>(std::make_shared<ConstantTexture>(kd));
  // auto roughness = std::sqrt(2.0 / (2.0 + mat.shininess));
  // auto specular = std::make_shared<MicrofacetBSDF>(ksMap ? ksMap : std::make_shared<RGBShader>(ks),
  //                                                  std::make_shared<FloatShader>(roughness));
  //auto mixed = std::make_shared<MixBSDF>(std::make_shared<FloatShader>(frac), diffuse, specular);
  material->bsdf = diffuse;
  material->emission = std::make_shared<ConstantTexture>(emission);
  material->emissionStrength = std::make_shared<ConstantTexture>(strength);
  return material;
}

MeshImportResult WavefrontImporter::Import(const fs::path &path) {
  auto parent = fs::absolute(path).parent_path();
  auto file = fs::absolute(path);
  MIN_DEBUG("Importing {}\n", file.string());
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;

  std::string warn;
  std::string err;

  MeshImportResult result;
  bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, file.string().c_str(), parent.string().c_str());
  if (!warn.empty()) MIN_WARN("warn loading {}\n", warn);
  if (!ret) {
    MIN_ERROR("error loading {}\n", file.string());
    return result;
  }
  auto mesh = std::make_shared<Mesh>();
  mesh->filename = path.string();
  mesh->vertex_data.position.reserve(attrib.vertices.size() / 3);
  mesh->vertex_data.normal.reserve(attrib.normals.size() / 3);
  mesh->vertex_data.texcoord.reserve(attrib.texcoords.size() / 2);

  for (size_t i = 0; i < attrib.vertices.size(); i += 3) {
    mesh->vertex_data.position.emplace_back(
        Vector3(attrib.vertices[i + 0], attrib.vertices[i + 1], attrib.vertices[i + 2]));
  }
  for (size_t i = 0; i < attrib.normals.size(); i += 3) {
    mesh->vertex_data.normal.emplace_back(
        Vector3(attrib.normals[i + 0], attrib.normals[i + 1], attrib.normals[i + 2]));
  }
  for (size_t i = 0; i < attrib.texcoords.size(); i += 2) {
    mesh->vertex_data.texcoord.emplace_back(
        Point2(attrib.texcoords[i + 0], attrib.texcoords[i + 1]));
  }
  for (auto &i : materials) {
    auto mat = ConvertFromMTL(i);
    mesh->materials_map[i.name] = mat;
  }
  std::unordered_map<std::string, size_t> name_to_id;
  std::unordered_map<size_t, std::string> id_to_name;

  // Loop over shapes
  for (size_t s = 0; s < shapes.size(); s++) {
    // Loop over faces(polygon)
    size_t index_offset = 0;
    for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
      MeshTriangle primitive;
      VertexIndices indices;
      if (materials.size()) {
        auto mat = materials[shapes[s].mesh.material_ids[f]].name;
        if (mat.empty()) {
          primitive.name_id = -1;
        } else {
          if (name_to_id.find(mat) == name_to_id.end()) {
            name_to_id[mat] = name_to_id.size();
            id_to_name[name_to_id[mat]] = mat;
          }
          primitive.name_id = name_to_id.at(mat);
        }
      }

      int fv = shapes[s].mesh.num_face_vertices[f];
      assert(fv == 3);
      for (size_t v = 0; v < fv; v++) {
        // access to vertex
        tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
        indices.position[v] = idx.vertex_index;
        if (idx.normal_index >= 0) {
          assert(3 * idx.normal_index + 2 < attrib.normals.size());
          indices.normal[v] = idx.normal_index;
        } else {
          indices.normal[v] = -1;
        }
        if (idx.texcoord_index >= 0) {
          assert(2 * idx.texcoord_index + 1 < attrib.texcoords.size());
          indices.texcoord[v] = idx.texcoord_index;
        } else {
          indices.texcoord[v] = -1;
        }
      }
      index_offset += fv;

      // per-face material
      // shapes[s].mesh.material_ids[f];

      primitive.mesh = mesh.get();
      primitive.indices = indices;
      mesh->triangles.push_back(primitive);
    }
  }

  for (size_t i = 0; i < name_to_id.size(); i++) {
    mesh->names.emplace_back(id_to_name[i]);
  }

  MIN_DEBUG("Loaded {} vertices, {} normals, {} tex coords, {} primitives\n", mesh->vertex_data.position.size(),
            mesh->vertex_data.normal.size(), mesh->vertex_data.texcoord.size(), mesh->triangles.size());
  mesh->loaded = true;
  result.mesh = mesh;
  return result;
}
}  // namespace min::ray
