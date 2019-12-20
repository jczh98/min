// MIT License
//
// Copyright (c) 2018-2019, neverfelly <neverfelly@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
#pragma once

#include "intersection.h"
#include "light.h"
#include "material.h"
#include "shape.h"

namespace min::ray {

class Shape;

class Primitive {
 public:
  virtual bool Intersect(const Ray &ray, Intersection &isect) const = 0;
  virtual BoundingBox3 GetBoundingBox() const = 0;
  virtual AreaLight *GetAreaLight() const { return nullptr; }
  virtual Material *GetMaterial() const { return nullptr; }
  virtual void ComputeScatteringFunctions(Intersection *isect, const ShadingPoint &sp) const = 0;
};

class GeometricPrimitive : public Primitive {
 public:
  GeometricPrimitive(const std::shared_ptr<Shape> &shape,
                     const std::shared_ptr<Material> &material,
                     const std::shared_ptr<AreaLight> &area_light = nullptr)
      : shape(shape), material(material), area_light(area_light) {
        if (area_light) {
          area_light->SetShape(shape.get());
        }
      }

  virtual bool Intersect(const Ray &ray, Intersection &isect) const {
    if (!shape->Intersect(ray, isect)) return false;
    isect.primitive = this;
    return true;
  }
  virtual BoundingBox3 GetBoundingBox() const {
    return shape->GetBoundingBox();
  }
  virtual AreaLight *GetAreaLight() const {
    return area_light.get();
  }
  virtual Material *GetMaterial() const {
    return material.get();
  }
  void ComputeScatteringFunctions(Intersection *isect, const ShadingPoint& sp) const {
    if (material) {
      material->ComputeScatteringFunctions(isect, sp);
    }
  }

 private:
  std::shared_ptr<Shape> shape;
  std::shared_ptr<Material> material;
  std::shared_ptr<AreaLight> area_light;
};

}  // namespace min::ray