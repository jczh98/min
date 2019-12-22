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

namespace min::refl {
class Assets : public Object {
 public:
  Object* Underlying(const std::string& name) const override {
    auto it = asset_index_map_.find(name);
    if (it == asset_index_map_.end()) {
      return nullptr;
    }
    return assets_.at(it->second).get();
  }
  void ForeachUnderlying(const Visitor& v) override {
    for (auto& asset : assets_) {
      visit(v, asset);
    }
  }
  virtual Object* LoadAsset(const std::string& name, const std::string& impl_key, const json& json);

 private:
  std::vector<Ptr<Object>> assets_;
  std::unordered_map<std::string, int> asset_index_map_;
};

MIN_IMPL(Assets, "assets::default")

}  // namespace min::refl