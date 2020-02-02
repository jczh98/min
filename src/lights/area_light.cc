#include <min-ray/emitter.h>
#include <min-ray/json.h>

namespace min::ray {

class AreaEmitter : public Emitter {
 public:
//  AreaEmitter(const PropertyList &props) {
//    radiance = props.getColor("radiance");
//  }

  void initialize(const json &json) override {

    radiance = json.at("radiance").get<Color3f>();
  }
//  virtual std::string ToString() const override {
//    return tfm::format(
//        "AreaLight[\n"
//        "  radiance = %s,\n"
//        "]",
//        radiance.ToString());
//  }

  virtual Color3f Evaluate(const LightRayQueryRecord & lRec) const override {
    if(!mesh)
      throw NoriException("There is no shape attached to this Area light!");

    if ((-lRec.wi).dot(lRec.n) >= 0)
      return radiance;
    else
      return 0;
  }

  virtual Color3f SampleLe(LightRayQueryRecord & lRec, const Point2f & sample) const override {
    if(!mesh)
      throw NoriException("There is no shape attached to this Area light!");

    SurfaceSample sRec;
    mesh->Sample(sample, sRec);

    // set the emitter query record
    lRec.p = sRec.p;
    lRec.n = sRec.n;
    lRec.wi = (lRec.p - lRec.ref).normalized();
    lRec.shadowRay = Ray3f(lRec.ref, lRec.wi, Epsilon, (lRec.p - lRec.ref).norm() - Epsilon);

    float pdf_val = Pdf(lRec);
    if (pdf_val == 0)
      return 0;

    return Evaluate(lRec) / pdf_val;
  }

  virtual float Pdf(const LightRayQueryRecord &lRec) const override {
    if(!mesh)
      throw NoriException("There is no shape attached to this Area light!");

    SurfaceSample sRec(lRec.ref, lRec.p);
    sRec.n = lRec.n;
    float cosTheta = (-lRec.wi).dot(lRec.n);

    if (cosTheta <= 0)
      return 0;

    return  mesh->Pdf(sRec) * (lRec.p - lRec.ref).squaredNorm() / cosTheta;
  }

 protected:
  Color3f radiance;
};
MIN_IMPLEMENTATION(Emitter, AreaEmitter, "area")
//NORI_REGISTER_CLASS(AreaEmitter, "area")
}

