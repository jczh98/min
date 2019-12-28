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

#include <fmt/format.h>
#include <min-ray/object.h>
#include <tuple>
#include <unordered_map>

namespace min::refl {
class ObjectFactory final {
 public:
  static ObjectFactory& instance() {
    static ObjectFactory instance;
    return instance;
  }

  Object* Create(const std::string& key) {
    auto it = func_map_.find(key);
    if (it == func_map_.end()) {
      fmt::print("Missing key for {}.\n", key);
      return nullptr;
    }
    auto* p = it->second.create_func();
    Access::key(p) = key;
    Access::create_func(p) = it->second.create_func;
    Access::release_func(p) = it->second.release_func;
    return p;
  }

  void Register(const std::string& key, const Object::CreateFunc& create_func, const Object::ReleaseFunc& release_func) {
    if (func_map_.find(key) != func_map_.end()) {
      fmt::print("Object is already registerd [key='{}'], overridng\n", key);
    }
    func_map_[key] = BundledFunctions{create_func, release_func};
  }

  void Unregister(const std::string& key) {
    func_map_.erase(key);
  }

  void RegisterAsRoot(Object* p) {
    if (Access::loc(p) != "$") {
      return;
    }
    root_ = p;
  }

  Object* get(const std::string& loc) {
    if (!root_) {
      return nullptr;
    }
    if (loc.empty()) {
      return nullptr;
    }

    const auto split_first = [&](const std::string& s) -> std::tuple<std::string, std::string> {
      const auto i = s.find_first_of('.', 0);
      if (i == std::string::npos) {
        return {s, ""};
      }
      return {s.substr(0, i), s.substr(i + 1)};
    };

    const auto [s0, r0] = split_first(loc);
    if (s0 != "$") {
      return nullptr;
    }
    auto remain = r0;
    auto* cur = root_;
    while (cur && !remain.empty()) {
      const auto [s, r] = split_first(remain);
      cur = cur->Underlying(s);
      //fmt::print("{}\n", cur->loc());
      remain = r;
    }
    if (!cur) {
      fmt::print("Failed to find a component with locator [loc='{}']", loc);
    }
    return cur;
  }

 private:
  struct BundledFunctions {
    Object::CreateFunc create_func;
    Object::ReleaseFunc release_func;
  };

  std::unordered_map<std::string, BundledFunctions> func_map_;
  Object* root_ = nullptr;
};

Object* CreateObject(const std::string& key) {
  return ObjectFactory::instance().Create(key);
}

void Register(const std::string& key, const Object::CreateFunc& create_func, const Object::ReleaseFunc& release_func) {
  ObjectFactory::instance().Register(key, create_func, release_func);
}

void Unregister(const std::string& key) {
  ObjectFactory::instance().Unregister(key);
}

void RegisterAsRoot(Object* p) {
  ObjectFactory::instance().RegisterAsRoot(p);
}

Object* get(const std::string& loc) {
  return ObjectFactory::instance().get(loc);
}

}  // namespace min::refl