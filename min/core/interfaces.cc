#pragma once

#include <min/visual/filter.h>
#include <min/visual/film.h>
#include <min/visual/renderer.h>
#include <min/visual/sampler.h>
#include <min/visual/camera.h>
#include <min/visual/scene.h>
#include <min/visual/aggregate.h>
#include <min/visual/bsdf.h>
#include <min/visual/texture.h>
#include <min/visual/light.h>

namespace min {

MIN_INTERFACE_DEF(Filter, "filter")
MIN_INTERFACE_DEF(Film, "film")
MIN_INTERFACE_DEF(Renderer, "renderer")
MIN_INTERFACE_DEF(Sampler, "sampler")
MIN_INTERFACE_DEF(Camera, "camera")
MIN_INTERFACE_DEF(Scene, "scene")
MIN_INTERFACE_DEF(Aggregate, "aggregate")
MIN_INTERFACE_DEF(Accelerator, "accelerator")
MIN_INTERFACE_DEF(Texture, "texture")
MIN_INTERFACE_DEF(BSDF, "bsdf")
MIN_INTERFACE_DEF(Light, "light")

}