#include <min-ray/interface.h>
#include <min-ray/bsdf.h>
#include <min-ray/camera.h>
#include <min-ray/light.h>
#include <min-ray/integrator.h>
#include <min-ray/accel.h>
#include <min-ray/mesh.h>
#include <min-ray/renderer.h>
#include <min-ray/filter.h>
#include <min-ray/sampler.h>
#include <min-ray/scene.h>
#include <functional>

namespace min::ray {

MIN_INTERFACE_DEF(Accelerator, "accelerator")
MIN_INTERFACE_DEF(BSDF, "bsdf")
MIN_INTERFACE_DEF(Camera, "camera")
MIN_INTERFACE_DEF(Light, "light")
MIN_INTERFACE_DEF(Integrator, "integrator")
MIN_INTERFACE_DEF(Renderer, "renderer")
MIN_INTERFACE_DEF(ReconstructionFilter, "filter")
MIN_INTERFACE_DEF(Sampler, "sampler")
MIN_INTERFACE_DEF(Mesh, "mesh")
MIN_INTERFACE_DEF(Scene, "scene")

}