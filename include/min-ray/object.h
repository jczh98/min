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

#include <cereal/archives/json.hpp>
#include <cereal/cereal.hpp>
#include <cereal/details/static_object.hpp>
#include <nlohmann/json.hpp>

namespace min::ray {

class Object {
 public:
  using CreateFunc = std::function<Object*()>;
  using ReleaseFunc = std::function<void(Object*)>;
  virtual void Initialize(const nlohmann::json& json) {}
  virtual std::string GetType() const = 0;

 private:
  friend struct Access;
  std::string key_;
  std::string loc_;
  CreateFunc create_func_;
  ReleaseFunc release_func_;
};

struct Access {
  Access() = delete;
  static std::string& key(Object* p) { return p->key_; }
  static const std::string& key(const Object* p) { return p->key_; }
  static std::string& loc(Object* p) { return p->key_; }
  static const std::string& loc(const Object* p) { return p->loc_; }
  static Object::CreateFunc& create_func(Object* p) { return p->create_func_; }
  static const Object::CreateFunc& create_func(const Object* p) { return p->create_func_; }
  static Object::ReleaseFunc& release_func(Object* p) { return p->release_func_; }
  static const Object::ReleaseFunc& release_func(const Object* p) { return p->release_func_; }
};

Object* Create(const std::string& key);

void Register(const std::string& key, const Object::CreateFunc& create_func, const Object::ReleaseFunc& release_func);

void Unregister(const std::string& key);

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
}  // namespace min::ray