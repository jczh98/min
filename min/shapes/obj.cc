#include <min/visual/aggregate.h>
#include <min/visual/material.h>
#include <min/visual/light.h>
#include <tiny_obj_loader.h>
#include <fstream>
#include <sstream>
#include "triangle.h"

namespace min {

class Obj : public Aggregate {
 protected:
  // Vertex indices used by the OBJ format
  static std::vector<std::string> Tokenize(const std::string &string, const std::string &delim, bool includeEmpty) {
    std::string::size_type lastPos = 0, pos = string.find_first_of(delim, lastPos);
    std::vector<std::string> tokens;

    while (lastPos != std::string::npos) {
      if (pos != lastPos || includeEmpty)
        tokens.push_back(string.substr(lastPos, pos - lastPos));
      lastPos = pos;
      if (lastPos != std::string::npos) {
        lastPos += 1;
        pos = string.find_first_of(delim, lastPos);
      }
    }

    return tokens;
  }

  static unsigned int ToUInt(const std::string &str) {
    char *end_ptr = nullptr;
    unsigned int result = (int)strtoul(str.c_str(), &end_ptr, 10);
    if (*end_ptr != '\0')
    MIN_ERROR("Could not parse integer value \"{}\"", str);
    return result;
  }

  struct OBJVertex {
    uint32_t p = (uint32_t)-1;
    uint32_t n = (uint32_t)-1;
    uint32_t uv = (uint32_t)-1;

    inline OBJVertex() {}

    inline OBJVertex(const std::string &string) {
      std::vector<std::string> tokens = Tokenize(string, "/", true);

      if (tokens.size() < 1 || tokens.size() > 3)
      MIN_ERROR("Invalid vertex data: {}", string);

      p = ToUInt(tokens[0]);

      if (tokens.size() >= 2 && !tokens[1].empty())
        uv = ToUInt(tokens[1]);

      if (tokens.size() >= 3 && !tokens[2].empty())
        n = ToUInt(tokens[2]);
    }

    inline bool operator==(const OBJVertex &v) const {
      return v.p == p && v.n == n && v.uv == uv;
    }
  };

  template <typename ArgumentType, typename ResultType>
  struct unary_function{
    using argument_type = ArgumentType;
    using result_type = ResultType;
  };

  // Hash function for OBJVertex
  struct OBJVertexHash : unary_function<OBJVertex, size_t> {
    std::size_t operator()(const OBJVertex &v) const {
      size_t hash = std::hash<uint32_t>()(v.p);
      hash = hash * 37 + std::hash<uint32_t>()(v.uv);
      hash = hash * 37 + std::hash<uint32_t>()(v.n);
      return hash;
    }
  };
 public:
  void initialize(const Json &json) override {
    typedef std::unordered_map<OBJVertex, uint32_t, OBJVertexHash> VertexMap;
    fs::path filename = GetFileResolver()->Resolve(json.at("filename").get<std::string>());
    Transform transform = Transform();
    if (json.contains("transform")) {
      transform = json.at("transform").get<Transform>();
    }
//    std::ifstream is(filename.string());
//    if (is.fail())
//    MIN_ERROR("Unable to open OBJ file {}!", filename.string());
//    Transform trafo = Transform();
//    if (json.contains("transform")) {
//      trafo = json.at("transform").get<Transform>();
//    }
//    MIN_DEBUG("Loading \"{}\" .. ", filename.string());
//
//    std::vector<Vector3f> local_positions;
//    std::vector<Vector2f> local_texcoords;
//    std::vector<Vector3f> local_normals;
//    std::vector<uint32_t> local_indices;
//    std::vector<OBJVertex> local_vertices;
//    VertexMap vertexMap;
//
//    std::string line_str;
//    while (std::getline(is, line_str)) {
//      std::istringstream line(line_str);
//
//      std::string prefix;
//      line >> prefix;
//
//      if (prefix == "v") {
//        Point3f p;
//        line >> p.x >> p.y >> p.z;
//        p = trafo.ToPoint(p);
//        local_positions.push_back (p);
//      } else if (prefix == "vt") {
//        Point2f tc;
//        line >> tc.x >> tc.y;
//        local_texcoords.push_back(tc);
//      } else if (prefix == "vn") {
//        Normal3f n;
//        line >> n.x >> n.y >> n.z;
//        local_normals.push_back(Normalize(trafo.ToNormal(n)));
//      } else if (prefix == "f") {
//        std::string v1, v2, v3, v4;
//        line >> v1 >> v2 >> v3 >> v4;
//        OBJVertex verts[6];
//        int nVertices = 3;
//
//        verts[0] = OBJVertex(v1);
//        verts[1] = OBJVertex(v2);
//        verts[2] = OBJVertex(v3);
//
//        if (!v4.empty()) {
//          /* This is a quad, split into two triangles */
//          verts[3] = OBJVertex(v4);
//          verts[4] = verts[0];
//          verts[5] = verts[2];
//          nVertices = 6;
//        }
//        /* Convert to an indexed vertex list */
//        for (int i = 0; i < nVertices; ++i) {
//          const OBJVertex &v = verts[i];
//          VertexMap::const_iterator it = vertexMap.find(v);
//          if (it == vertexMap.end()) {
//            vertexMap[v] = (uint32_t)local_vertices.size();
//            local_indices.push_back((uint32_t)local_vertices.size());
//            local_vertices.push_back(v);
//          } else {
//            local_indices.push_back(it->second);
//          }
//        }
//      }
//    }
//    int n_tri = local_indices.size() / 3;
//    int n_vex = local_vertices.size();
//    //int *faces = new int[local_indices.size() / 3];
//    int *vertex_indices = new int[local_indices.size()];
//    //memcpy(faces, local_indices.data(), sizeof(uint32_t) * local_indices.size());
//    for (uint32 i = 0; i < local_indices.size(); ++i) {
//      vertex_indices[i] = local_indices[i];
//    }
//
//
//    Point3f *positions = new Point3f[local_vertices.size()];
//    for (uint32_t i = 0; i < local_vertices.size(); ++i)
//      positions[i] = local_positions.at(local_vertices[i].p - 1);
//
//    Normal3f *normals = nullptr;
//    if (!local_normals.empty()) {
//      normals = new Normal3f[(int)local_vertices.size()];
//      for (uint32_t i = 0; i < local_vertices.size(); ++i)
//        normals[i] = local_normals.at(local_vertices[i].n - 1);
//    }
//
//    Point2f *texcoords = nullptr;
//    if (!local_texcoords.empty()) {
//      texcoords = new Point2f[(int)local_vertices.size()];
//      for (uint32_t i = 0; i < local_vertices.size(); ++i)
//        texcoords[i] = local_texcoords.at(local_vertices[i].uv - 1);
//    }
    MIN_DEBUG("Loading \"{}\" .. ", filename.string());
    Point3 *positions = nullptr;
    Normal3 *normals = nullptr;
    Point2 *texcoords = nullptr;
    int *vertex_indices = nullptr;
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;

    std::string warn;
    std::string err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, nullptr, &warn, &err, filename.string().c_str());
    if (!warn.empty()) {
      MIN_WARN("Warn: {}", warn);
    }

    if (!err.empty()) {
      MIN_ERROR("Unable to open OBJ file {}: {}", err);
    }

    if (!ret) {
      MIN_ERROR("Unable to open OBJ file {}!", filename.string());
    }
    int vertex_index = 0, num_triangles = 0, num_vertexs = 0;
    bool count_tex = false, count_normal = false;
    for (size_t s = 0; s < shapes.size(); s++) {
      size_t index_offset = 0;
      for (size_t f= 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
        for (size_t v = 0; v < shapes[s].mesh.num_face_vertices[f]; v++) {
          tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
          if (idx.normal_index != -1) { count_normal = true; }
          if (idx.texcoord_index != -1) { count_tex = true; }
          num_vertexs++;
        }
        index_offset += shapes[s].mesh.num_face_vertices[f];
        num_triangles++;
      }
    }
    vertex_indices = new int[num_vertexs];
    positions = new Point3[num_vertexs];
    if (count_tex) texcoords = new Point2[num_vertexs];
    if (count_normal) normals = new Normal3[num_vertexs];
    // Loop over shapes
    for (size_t s = 0; s < shapes.size(); s++) {
      // Loop over faces(polygon)
      size_t index_offset = 0;
      for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
        int fv = shapes[s].mesh.num_face_vertices[f];

        // Loop over vertices in the face.
        for (size_t v = 0; v < fv; v++) {
          // access to vertex
          vertex_indices[vertex_index] = vertex_index;
          tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
          tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
          tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
          tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];
          positions[vertex_index] = transform.ToPoint(Point3(vx, vy, vz));
          if (idx.normal_index != -1) {
            tinyobj::real_t nx = attrib.normals[3 * idx.normal_index + 0];
            tinyobj::real_t ny = attrib.normals[3 * idx.normal_index + 1];
            tinyobj::real_t nz = attrib.normals[3 * idx.normal_index + 2];
            normals[vertex_index] = transform.ToNormal(Normal3(nx, ny, nz));
          }
          if (idx.texcoord_index != -1) {
            tinyobj::real_t tx = attrib.texcoords[2 * idx.texcoord_index + 0];
            tinyobj::real_t ty = attrib.texcoords[2 * idx.texcoord_index + 1];
            texcoords[vertex_index] = Point2(tx, ty);
          }
          vertex_index++;
        }
        index_offset += fv;

        // per-face material
        shapes[s].mesh.material_ids[f];
      }
    }
    auto tri_shapes = CreateTriangleMesh(Transform(), Transform(), num_triangles, vertex_indices, num_vertexs,
        positions, nullptr, normals, texcoords);
    MIN_DEBUG("Done. (V={}, F={})", num_vertexs, num_triangles);
    std::shared_ptr<Material> material = nullptr;
    if (json.contains("material")) {
      material = CreateInstance<Material>(json["material"]["type"], GetProps(json["material"]));
    } else {
      material = CreateInstance<Material>("diffuse", {});
    }
    for (auto &shape : tri_shapes) {
      shape->material = material;
      if (json.contains("light")) {
        auto light = CreateInstance<Light>(json["light"]["type"], GetProps(json["light"]));
        shape->area_light = light;
        light->SetShape(shape);
        lights.emplace_back(light);
      }
    }
    this->shapes = tri_shapes;
  }
};
MIN_IMPLEMENTATION(Aggregate, Obj, "obj")

}

