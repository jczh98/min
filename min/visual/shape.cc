#include <min/visual/shape.h>
#include <min/visual/intersection.h>

namespace min {

Float Shape::Pdf(const Intersection &ref, const Vector3 &wi) const {
    Ray ray = ref.SpawnRay(wi);
    SurfaceIntersection isect_light;
    if (!Intersect(ray, isect_light)) {
      return 0;
    }
    Float pdf = (ref.p - isect_light.p).LengthSquared() / (std::abs(Dot(isect_light.geo_frame.n, -wi)) * Area());
    if (std::isinf(pdf)) pdf = 0.0f;
    return pdf;
}

}

