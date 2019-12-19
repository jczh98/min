#include "wavefront.h"
#include <tiny_obj_loader.h>
#include "../bsdfs/lambertian.h"
#include "../materials/matte.h"
#include "../shaders/common.h"
#include "../shapes/triangle.h"

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
  auto kd = Vector3(mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]);
  auto ks = Vector3(mat.specular[0], mat.specular[1], mat.specular[2]);
  auto emission = Vector3(mat.emission[0], mat.emission[1], mat.emission[2]);
  auto material = std::make_shared<Matte>(std::make_shared<RGBShader>(kd));
  //auto diffuse = std::make_shared<DiffuseBSDF>(std::make_shared<RGBShader>(kd));
  //auto specular = std::make_shared<DiffuseBSDF>(std::make_shared<RGBShader>(ks));
  //auto mixed = std::make_shared<MixBSDF>(std::make_shared<FloatShader>(0.5f), diffuse, specular);
  //material->bsdf = mixed;
  //material->emission = std::make_shared<RGBShader>(emission);
  return material;
}

MeshImportResult WavefrontImporter::Import(const fs::path &path) {
  fmt::print("Importing {}...\n", path.string());
  fs::path parent_path = fs::absolute(path).parent_path();
  fs::path file = path.filename();
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn;
  std::string err;
  MeshImportResult result;
  bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.string().c_str());
  if (!ret) {
    fmt::print("Error loading {} : {}", path.string(), err);
    return result;
  }
  printf("# of vertices  = %d\n", (int)(attrib.vertices.size()) / 3);
  printf("# of normals   = %d\n", (int)(attrib.normals.size()) / 3);
  printf("# of texcoords = %d\n", (int)(attrib.texcoords.size()) / 2);
  printf("# of materials = %d\n", (int)materials.size());
  printf("# of shapes    = %d\n", (int)shapes.size());
  auto mesh = std::make_shared<MeshTriangle>();
  int face_count = 0;
  for (size_t s = 0; s < shapes.size(); s++) {
    size_t index_offset = 0;
    for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++, face_count++) {
      int fv = shapes[s].mesh.num_face_vertices[f];
      for (size_t v = 0; v < fv; v++) {
        tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
        mesh->positions.emplace_back(Vector3(attrib.vertices[3 * idx.vertex_index + 0],
                                             attrib.vertices[3 * idx.vertex_index + 1],
                                             attrib.vertices[3 * idx.vertex_index + 2]));
        mesh->normals.emplace_back(Vector3(attrib.normals[3 * idx.normal_index + 0],
                                           attrib.normals[3 * idx.normal_index + 1],
                                           attrib.normals[3 * idx.normal_index + 2]));
        mesh->tex_coords.emplace_back(Vector2(attrib.texcoords[2 * idx.texcoord_index + 0],
                                           attrib.texcoords[2 * idx.texcoord_index + 1]));
        mesh->vertex_indices.emplace_back(3 * face_count + 0);
        mesh->vertex_indices.emplace_back(3 * face_count + 1);
        mesh->vertex_indices.emplace_back(3 * face_count + 2);
        mesh->face_indices.emplace_back(face_count);
      }
      index_offset += fv;
      //auto primitive = std::make_shared<Triangle>(mesh, face_count);
      //result.triangles.emplace_back(primitive);
    }
  }
  for (size_t f = 0; f < face_count; f++) {
    auto primitive = std::make_shared<Triangle>(mesh, f);
    result.triangles.emplace_back(primitive);
  }
  fmt::print("Loaded {} vertices, {} normals, {} primitives\n", mesh->positions.size(),
             mesh->normals.size(), mesh->face_indices.size());
  return result;
}
}  // namespace min::ray
