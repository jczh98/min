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

#include <any>
#include <memory>
#include <nlohmann/json.hpp>
#include <iostream>

namespace min::refl {

using namespace nlohmann;

struct ObjectDeleter;

class Object {
 public:
  using CreateFunc = std::function<Object*()>;
  using ReleaseFunc = std::function<void(Object*)>;
  template <typename Interface>
  using Ptr = std::unique_ptr<Interface, ObjectDeleter>;
  using Visitor = std::function<void(Object*& p, bool weak)>;

  virtual void Initialize(const json& json) {}
  virtual Object* Underlying(const std::string& name) const { return nullptr; }
  virtual void ForeachUnderlying(const Visitor& visitor) {}

  const std::string& key() const { return key_; }
  const std::string& loc() const { return loc_; }
  const std::string name() const {
    const auto i = loc().find_last_of('.');
    return loc().substr(i + 1);
  }
  const std::string MakeLoc(const std::string& base, const std::string& child) const {
    return base + '.' + child;
  }
  const std::string MakeLoc(const std::string& child) const {
    return MakeLoc(loc(), child);
  }

 private:
  friend struct Access;
  friend struct ObjectDeleter;
  std::string key_;
  std::string loc_;
  CreateFunc create_func_;
  ReleaseFunc release_func_;
  std::any owner_ref;
};

struct ObjectDeleter {
  ObjectDeleter() = default;
  void operator()(Object* p) const noexcept {
    if (auto release_func = p->release_func_; release_func) {
      release_func(p);
    }
  }
};

struct Access {
  Access() = delete;
  static std::string& key(Object* p) { return p->key_; }
  static const std::string& key(const Object* p) { return p->key_; }
  static std::string& loc(Object* p) { return p->loc_; }
  static const std::string& loc(const Object* p) { return p->loc_; }
  static Object::CreateFunc& create_func(Object* p) { return p->create_func_; }
  static const Object::CreateFunc& create_func(const Object* p) { return p->create_func_; }
  static Object::ReleaseFunc& release_func(Object* p) { return p->release_func_; }
  static const Object::ReleaseFunc& release_func(const Object* p) { return p->release_func_; }
};

// Type holder
template <typename... Ts>
struct TypeHolder {
};

// Makes key for create function
template <typename T>
struct KeyGen {
  static std::string gen(const std::string& s) {
    // For ordinary type, use the given key as it is.
    return s;
  }
};
template <template <typename...> class T, typename... Ts>
struct KeyGen<T<Ts...>> {
  static std::string gen(const std::string& s) {
    // For template type, decorate the type with type list.
    return s + "<" + std::string(typeid(TypeHolder<Ts...>).name()) + ">";
  }
};

// Global functions
Object* CreateObject(const std::string& key);

void Register(const std::string& key, const Object::CreateFunc& create_func, const Object::ReleaseFunc& release_func);

void Unregister(const std::string& key);

void RegisterAsRoot(Object* p);

Object* get(const std::string& loc);

template <typename T>
T* get(const std::string& loc) {
  return dynamic_cast<T*>(refl::get(loc));
}

template <typename T>
std::enable_if_t<std::is_base_of_v<Object, T>, void>
UpdateWeakRef(T*& p) {
  if (!p) {
    return;
  }
  const auto loc = p->loc();
  if (loc.empty()) {
    return;
  }
  p = get<T>(loc);
}

template <typename T>
std::enable_if_t<std::is_base_of_v<Object, T>, void>
visit(const Object::Visitor& visitor, T*& p) {
  Object* tmp = p;
  visitor(tmp, true);
  p = dynamic_cast<T*>(tmp);
}

template <typename T>
std::enable_if_t<std::is_base_of_v<Object, T>, void>
visit(const Object::Visitor& visitor, std::shared_ptr<T>& p) {
  Object* tmp = p.get();
  visitor(tmp, true);
  p = std::shared_ptr<T>(dynamic_cast<T*>(tmp));
}

template <typename T>
std::enable_if_t<std::is_base_of_v<Object, T>, void>
visit(const Object::Visitor& visitor, Object::Ptr<T>& p) {
  Object* tmp = p.get();
  visitor(tmp, false);
}

template <typename Interface>
Object::Ptr<Interface> CreateWithoutInitialize(const std::string& key, const std::string& loc) {
  auto* inst = CreateObject(KeyGen<Interface>::gen(std::move(key)).c_str());
  if (!inst) {
    return {};
  }
  Access::loc(inst) = loc;
  return Object::Ptr<Interface>(dynamic_cast<Interface*>(inst));
}
template <typename Interface>
Object::Ptr<Interface> Create(const std::string& key, const std::string& loc, const json& json = {}) {
  auto inst = CreateWithoutInitialize<Interface>(key, loc);
  if (!inst) {
    return {};
  }
  inst->Initialize(json);
  return inst;
}

template <typename ImplType>
class Registry {
 public:
  static Registry<ImplType>& instance(std::string key) {
    static Registry<ImplType> instance(KeyGen<ImplType>::gen(std::move(key)));
    return instance;
  }
  Registry(const std::string& key) : key_(key) {
    Register(key_,
             []() -> Object* {
               return new ImplType;
             },
             [](Object* p) {
               if (p != nullptr) {
                 delete p;
                 p = nullptr;
               }
             });
  }
  ~Registry() {
    Unregister(key_);
  }

 private:
  std::string key_;
};

#define MIN_IMPL(Impl, Key)                         \
  namespace {                                       \
  template <typename T>                             \
  class Registry_Init_;                             \
  template <>                                       \
  class Registry_Init_<Impl> {                      \
    static const min::refl::Registry<Impl>& reg_;   \
  };                                                \
  const min::refl::Registry<Impl>&                  \
      Registry_Init_<Impl>::reg_ =                  \
          min::refl::Registry<Impl>::instance(Key); \
  }
}  // namespace min::refl