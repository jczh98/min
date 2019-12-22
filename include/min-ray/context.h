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

#include "object.h"
#include "assets.h"

namespace min::refl {
class Context : public Object {
 public:
 Context() {
   Access::key(this) = "$";
   RegisterAsRoot(this);
 }

 static Context& instance() {
   static Context instance;
   return instance;
 }

 Assets* assets() {
   return root_assets_.get();
 }

 Object* Underlying(const std::string &name) const override {
   if (name == "assets") {
     return root_assets_.get();
   }
   return nullptr;
 }

 void ForeachUnderlying(const Visitor& visitor) const override {
   visit(visitor, root_assets_);
 }

  void Reset() {
    root_assets_ = Create<Assets>("assets::default", MakeLoc("assets"));
  }

 private:
  bool initialized_ = false;
  Ptr<Assets> root_assets_;
};

template<typename T>
T* Load(const std::string& name, const std::string& impl_key, const json &json) {
  return dynamic_cast<T*>(Context::instance.assets()->LoadAsset(name, impl_key, json));
}
}