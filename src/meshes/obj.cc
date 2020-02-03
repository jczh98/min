#include <min-ray/mesh.h>
#include <min-ray/json.h>
#include <min-ray/timer.h>
#include <fstream>
#include <unordered_map>

namespace min::ray {

class WavefrontOBJ : public Mesh {
 public:
  void initialize(const json &json) override {
    typedef std::unordered_map<OBJVertex, uint32_t, OBJVertexHash> VertexMap;

    fs::path filename =
        GetFileResolver()->Resolve(json.at("filename").get<std::string>());

    std::ifstream is(filename.string());
    if (is.fail())
      MIN_ERROR("Unable to open OBJ file {}!", filename.string());
    Transform trafo = Transform();
    if (json.contains("transform")) {
      trafo = json.at("transform").get<Transform>();
    }
    MIN_DEBUG("Loading \"{}\ .. ", filename.string());
    Timer timer;

    std::vector<Vector3f> local_positions;
    std::vector<Vector2f> local_texcoords;
    std::vector<Vector3f> local_normals;
    std::vector<uint32_t> local_indices;
    std::vector<OBJVertex> local_vertices;
    VertexMap vertexMap;

    std::string line_str;
    while (std::getline(is, line_str)) {
      std::istringstream line(line_str);

      std::string prefix;
      line >> prefix;

      if (prefix == "v") {
        Point3f p;
        line >> p.x() >> p.y() >> p.z();
        p = trafo * p;
        bound_box.ExpandBy(p);
        local_positions.push_back(p);
      } else if (prefix == "vt") {
        Point2f tc;
        line >> tc.x() >> tc.y();
        local_texcoords.push_back(tc);
      } else if (prefix == "vn") {
        Normal3f n;
        line >> n.x() >> n.y() >> n.z();
        local_normals.push_back((trafo * n).normalized());
      } else if (prefix == "f") {
        std::string v1, v2, v3, v4;
        line >> v1 >> v2 >> v3 >> v4;
        OBJVertex verts[6];
        int nVertices = 3;

        verts[0] = OBJVertex(v1);
        verts[1] = OBJVertex(v2);
        verts[2] = OBJVertex(v3);

        if (!v4.empty()) {
          /* This is a quad, split into two triangles */
          verts[3] = OBJVertex(v4);
          verts[4] = verts[0];
          verts[5] = verts[2];
          nVertices = 6;
        }
        /* Convert to an indexed vertex list */
        for (int i = 0; i < nVertices; ++i) {
          const OBJVertex &v = verts[i];
          VertexMap::const_iterator it = vertexMap.find(v);
          if (it == vertexMap.end()) {
            vertexMap[v] = (uint32_t)local_vertices.size();
            local_indices.push_back((uint32_t)local_vertices.size());
            local_vertices.push_back(v);
          } else {
            local_indices.push_back(it->second);
          }
        }
      }
    }

    faces.resize(3, local_indices.size() / 3);
    memcpy(faces.data(), local_indices.data(), sizeof(uint32_t) * local_indices.size());

    positions.resize(3, local_vertices.size());
    for (uint32_t i = 0; i < local_vertices.size(); ++i)
      positions.col(i) = local_positions.at(local_vertices[i].p - 1);

    if (!local_normals.empty()) {
      normals.resize(3, local_vertices.size());
      for (uint32_t i = 0; i < local_vertices.size(); ++i)
        normals.col(i) = local_normals.at(local_vertices[i].n - 1);
    }

    if (!local_texcoords.empty()) {
      texcoords.resize(2, local_vertices.size());
      for (uint32_t i = 0; i < local_vertices.size(); ++i)
        texcoords.col(i) = local_texcoords.at(local_vertices[i].uv - 1);
    }

    name_val = filename.string();
    MIN_DEBUG("Done. (V={}, F={}, taken {} and {})", positions.cols(), faces.cols(), timer.ElapsedString(),
              MemString(faces.size() * sizeof(uint32_t) + sizeof(float) * (positions.size() + normals.size() + texcoords.size())));
    Mesh::initialize(json);
  }

 protected:
  // Vertex indices used by the OBJ format
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
};
MIN_IMPLEMENTATION(Mesh, WavefrontOBJ, "obj")

}  // namespace min::ray
