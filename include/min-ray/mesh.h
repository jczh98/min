#include "interface.h"
#include "math.h"

namespace min::ray {

class Mesh {
	public:
	struct VertexData {
		Point3 pos;
		Normal3 normal;
		Point2 uv;
	};

	struct MeshTriangle {
		VertexData p1, p2, p3;
	};

};
}  // namespace min::ray