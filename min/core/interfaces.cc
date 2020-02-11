#pragma once

#include <min/visual/filter.h>
#include <min/visual/film.h>
#include <min/visual/renderer.h>
#include <min/visual/sampler.h>
#include <min/visual/camera.h>
#include <min/visual/scene.h>

namespace min {

MIN_INTERFACE_DEF(Filter, "filter")
MIN_INTERFACE_DEF(Film, "film")
MIN_INTERFACE_DEF(Renderer, "renderer")
MIN_INTERFACE_DEF(Sampler, "sampler")
MIN_INTERFACE_DEF(Camera, "camera")
MIN_INTERFACE_DEF(Scene, "scene")

}