
#include <min-ray/camera.h>
#include <min-ray/filter.h>
#include <min-ray/warp.h>
#include <min-ray/json.h>
#include <Eigen/Geometry>

namespace min::ray {

class PerspectiveCamera : public Camera {
 public:
  void initialize(const Json &json) override {
    output_size.x() = json.at("width").get<int>();
    output_size.y() = json.at("height").get<int>();
    m_invOutputSize = output_size.cast<float>().cwiseInverse();
    m_cameraToWorld = json.at("transform").get<Transform>();
    m_fov = json.at("fov").get<float>();
    m_nearClip = 1e-4f;
    m_farClip = 1e4f;
    float aspect = output_size.x() / (float)output_size.y();
    float recip = 1.0f / (m_farClip - m_nearClip),
        cot = 1.0f / std::tan(DegToRad(m_fov / 2.0f));

    Eigen::Matrix4f perspective;
    perspective << cot, 0, 0, 0,
        0, cot, 0, 0,
        0, 0, m_farClip * recip, -m_nearClip * m_farClip * recip,
        0, 0, 1, 0;
    m_sampleToCamera = Transform(
        Eigen::DiagonalMatrix<float, 3>(Vector3f(-0.5f, -0.5f * aspect, 1.0f)) *
            Eigen::Translation<float, 3>(-1.0f, -1.0f / aspect, 0.0f) * perspective)
        .Inverse();
    auto filter_json = rjson::AtOrEmpty(json, "filter");
    filter = CreateInstance<ReconstructionFilter>(rjson::GetOrDefault<std::string>(filter_json, "type", "guassian"),
        rjson::GetProps(filter_json));
  }

  Color3f SampleRay(Ray3f &ray,
                    const Point2f &samplePosition,
                    const Point2f &apertureSample) const {
    // Compute the corresponding position on the near plane (in local camera space)
    Point3f nearP = m_sampleToCamera * Point3f(
                                           samplePosition.x() * m_invOutputSize.x(),
                                           samplePosition.y() * m_invOutputSize.y(), 0.0f);

    Vector3f d = nearP.normalized();
    float invZ = 1.0f / d.z();

    ray.o = m_cameraToWorld * Point3f(0, 0, 0);
    ray.d = m_cameraToWorld * d;
    ray.mint = m_nearClip * invZ;
    ray.maxt = m_farClip * invZ;
    ray.update();

    return Color3f(1.0f);
  }

 private:
  Vector2f m_invOutputSize;
  Transform m_sampleToCamera;
  Transform m_cameraToWorld;
  float m_fov;
  float m_nearClip;
  float m_farClip;
};
MIN_IMPLEMENTATION(Camera, PerspectiveCamera, "perspective")

}  // namespace min::ray
