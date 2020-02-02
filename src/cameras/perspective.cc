
#include <min-ray/camera.h>
#include <min-ray/rfilter.h>
#include <min-ray/warp.h>
#include <min-ray/json.h>
#include <Eigen/Geometry>

namespace min::ray {

/**
 * \brief Perspective camera with depth of field
 *
 * This class implements a simple perspective camera model. It uses an
 * infinitesimally small aperture, creating an infinite depth of field.
 */
class PerspectiveCamera : public Camera {
 public:
  void initialize(const json &json) override {
    output_size.x() = json.at("width").get<int>();
    output_size.y() = json.at("height").get<int>();
    m_invOutputSize = output_size.cast<float>().cwiseInverse();
    m_cameraToWorld = json.at("transform").get<Transform>();
    m_fov = json.at("fov").get<float>();
    m_nearClip = 1e-4f;
    m_farClip = 1e4f;
    float aspect = output_size.x() / (float)output_size.y();
    float recip = 1.0f / (m_farClip - m_nearClip),
        cot = 1.0f / std::tan(degToRad(m_fov / 2.0f));

    Eigen::Matrix4f perspective;
    perspective << cot, 0, 0, 0,
        0, cot, 0, 0,
        0, 0, m_farClip * recip, -m_nearClip * m_farClip * recip,
        0, 0, 1, 0;
    m_sampleToCamera = Transform(
        Eigen::DiagonalMatrix<float, 3>(Vector3f(-0.5f, -0.5f * aspect, 1.0f)) *
            Eigen::Translation<float, 3>(-1.0f, -1.0f / aspect, 0.0f) * perspective)
        .Inverse();
    filter = CreateInstance<ReconstructionFilter>("gaussian");
  }
//  PerspectiveCamera(const PropertyList &propList) {
//    /* Width and height in pixels. Default: 720p */
//    output_size.x() = propList.getInteger("width", 1280);
//    output_size.y() = propList.getInteger("height", 720);
//    m_invOutputSize = output_size.cast<float>().cwiseInverse();
//
//    /* Specifies an optional camera-to-world transformation. Default: none */
//    m_cameraToWorld = propList.getTransform("toWorld", Transform());
//
//    /* Horizontal field of view in degrees */
//    m_fov = propList.getFloat("fov", 30.0f);
//
//    /* Near and far clipping planes in world-space units */
//    m_nearClip = propList.getFloat("nearClip", 1e-4f);
//    m_farClip = propList.getFloat("farClip", 1e4f);
//
//    rfilter = NULL;
//  }
//
//  void activate() {
//    float aspect = output_size.x() / (float)output_size.y();
//
//    /* Project vectors in camera space onto a plane at z=1:
//         *
//         *  xProj = cot * x / z
//         *  yProj = cot * y / z
//         *  zProj = (far * (z - near)) / (z * (far-near))
//         *  The cotangent factor ensures that the field of view is
//         *  mapped to the interval [-1, 1].
//         */
//    float recip = 1.0f / (m_farClip - m_nearClip),
//          cot = 1.0f / std::tan(degToRad(m_fov / 2.0f));
//
//    Eigen::Matrix4f perspective;
//    perspective << cot, 0, 0, 0,
//        0, cot, 0, 0,
//        0, 0, m_farClip * recip, -m_nearClip * m_farClip * recip,
//        0, 0, 1, 0;
//
//    /**
//         * Translation and scaling to shift the clip coordinates into the
//         * range from zero to one. Also takes the aspect ratio into account.
//         */
//    m_sampleToCamera = Transform(
//                           Eigen::DiagonalMatrix<float, 3>(Vector3f(-0.5f, -0.5f * aspect, 1.0f)) *
//                           Eigen::Translation<float, 3>(-1.0f, -1.0f / aspect, 0.0f) * perspective)
//                           .Inverse();
//
//    /* If no reconstruction filter was assigned, instantiate a Gaussian filter */
//    if (!rfilter)
//      rfilter = static_cast<ReconstructionFilter *>(
//          NoriObjectFactory::createInstance("gaussian", PropertyList()));
//  }

  Color3f SampleRay(Ray3f &ray,
                    const Point2f &samplePosition,
                    const Point2f &apertureSample) const {
    /* Compute the corresponding position on the 
           near plane (in local camera space) */
    Point3f nearP = m_sampleToCamera * Point3f(
                                           samplePosition.x() * m_invOutputSize.x(),
                                           samplePosition.y() * m_invOutputSize.y(), 0.0f);

    /* Turn into a normalized ray direction, and
           adjust the ray interval accordingly */
    Vector3f d = nearP.normalized();
    float invZ = 1.0f / d.z();

    ray.o = m_cameraToWorld * Point3f(0, 0, 0);
    ray.d = m_cameraToWorld * d;
    ray.mint = m_nearClip * invZ;
    ray.maxt = m_farClip * invZ;
    ray.update();

    return Color3f(1.0f);
  }

//  void addChild(NoriObject *obj) {
//    switch (obj->getClassType()) {
//      case EReconstructionFilter:
//        if (rfilter)
//          throw NoriException("Camera: tried to register multiple reconstruction filters!");
//        rfilter = static_cast<ReconstructionFilter *>(obj);
//        break;
//
//      default:
//        throw NoriException("Camera::addChild(<%s>) is not supported!",
//                            classTypeName(obj->getClassType()));
//    }
//  }

  /// Return a human-readable summary
//  std::string ToString() const {
//    return tfm::format(
//        "PerspectiveCamera[\n"
//        "  cameraToWorld = %s,\n"
//        "  outputSize = %s,\n"
//        "  fov = %f,\n"
//        "  clip = [%f, %f],\n"
//        "  rfilter = %s\n"
//        "]",
//        indent(m_cameraToWorld.ToString(), 18),
//        output_size.ToString(),
//        m_fov,
//        m_nearClip,
//        m_farClip,
//        indent(rfilter->ToString()));
//  }

 private:
  Vector2f m_invOutputSize;
  Transform m_sampleToCamera;
  Transform m_cameraToWorld;
  float m_fov;
  float m_nearClip;
  float m_farClip;
};
MIN_IMPLEMENTATION(Camera, PerspectiveCamera, "perspective")
//NORI_REGISTER_CLASS(PerspectiveCamera, "perspective");
}  // namespace min::ray
