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

#include <nlohmann/json.hpp>

namespace min::unit {

using namespace nlohmann;

template <typename T>
std::shared_ptr<T> CreateInstance(const std::string& alias);

template <typename T>
std::shared_ptr<T> CreateInstance(const std::string& alias,
                                  const json& json);

template <typename T>
std::unique_ptr<T> CreateInstanceUnique(const std::string& alias);

template <typename T>
std::unique_ptr<T> CreateInstanceUnique(const std::string& alias,
                                        const json& json);

template <typename T>
T* CreateInstanceRaw(const std::string& alias);

template <typename T>
T* CreateInstanceRaw(const std::string& alias,
                     const json& json);

template <typename T>
T* CreateInstancePlacement(const std::string& alias,
                           void* place);

template <typename T>
T* CreateInstancePlacement(const std::string& alias,
                           void* place,
                           const json& json);

template <typename T>
std::vector<std::string> GetImplementationNames();

class Unit {
 public:
  Unit() {}
  virtual void initialize(const json& json) {}

  virtual std::string name() const {
    return "";
  }

  virtual std::string GeneralAction(const json& json) {
    return "";
  }
  virtual ~Unit() {}
};

#define MIN_IMPLEMENTATION_HOLDER_NAME(T) ImplementationHolder_##T
#define MIN_IMPLEMENTATION_HOLDER_PTR(T) instance_ImplementationHolder_##T

class ImplementationHolderBase {
 public:
  std::string name;
  virtual bool has(const std::string& alias) const = 0;
  virtual bool remove(const std::string& alias) = 0;
  virtual std::vector<std::string> GetImplementationNames() const = 0;
};

class InterfaceHolder {
 public:
  typedef std::function<void(void*)> RegistrationMethod;
  std::map<std::string, RegistrationMethod> methods;
  std::map<std::string, ImplementationHolderBase*> interfaces;

  void RegisterRegistrationMethod(const std::string& name,
                                  const RegisterRegistrationMethod& method) {
    methods[name] = method;
  }

  void RegisterInterface(cosnt std::string& name,
                         ImplementationHolderBase* interface) {
    interfaces[name] = interface;
  }

  static InterfaceHolder* instance() {
    static InterfaceHolder holder;
    return &holder;
  }
};

#define MIN_INTERFACE(T)                                                                               \
  extern void* GetImplementationHolderInstance_##T();                                                  \
  class MIN_IMPLEMENTATION_HOLDER_NAME(T) final                                                        \
      : public ImplementationHolderBase {                                                              \
   public:                                                                                             \
    MIN_IMPLEMENTATION_HOLDER_NAME(T)                                                                  \
    (const std::string& name) {                                                                        \
      this->name = name;                                                                               \
    }                                                                                                  \
    using FactoryMethod = std::function<std::shared_ptr<T>()>;                                         \
    using FactoryUniqueMethod = std::function<std::unique_ptr<T>()>;                                   \
    using FactoryRawMethod = std::function<T*()>;                                                      \
    using FactoryPlacementMethod = std::function<T*(void*)>;                                           \
    std::map<std::string, FactoryMethod> implementation_factories;                                     \
    std::map<std::string, FactoryUniqueMethod> implementation_unique_factories;                        \
    std::map<std::string, FactoryRawMethod> implementation_raw_factories;                              \
    std::map<std::string, FactoryPlacementMethod> implementation_placement_factories;                  \
    std::vector<std::string> GetImplementationNames() const override {                                 \
      std::vector<std::string> names;                                                                  \
      for (auto& kv : implementation_factories) {                                                      \
        names.emplace_back(kv.first);                                                                  \
      }                                                                                                \
      return names;                                                                                    \
    }                                                                                                  \
    template <typename G>                                                                              \
    void insert(const std::string& alias) {                                                            \
      implementation_factories.insert(std::make_pair(alias, [&]() { return std::make_shared<G>(); })); \
      implementation_unique_factories.insert(                                                          \
          std::make_pair(alias, [&]() { return std::make_unique<G>(); }));                             \
      implementation_raw_factories.insert(                                                             \
          std::make_pair(alias, [&]() { return new G(); }));                                           \
      implementation_placement_factories.insert(                                                       \
          std::make_pair(alias, [&]() { return new (placement) G(); }));                               \
    }                                                                                                  \
    void insert(const std::string& alias, const FactoryMethod& f) {                                    \
      implementation_factories.insert(std::make_pair(alias, f));                                       \
    }                                                                                                  \
    bool has(const std::string& alias) const override {                                                \
      return implementation_factories.find(alias) != implementation_factories.end();                   \
    }                                                                                                  \
    void remove(const std::string& alias) override {                                                   \
      implementation_factories.erase(alias);                                                           \
    }                                                                                                  \
    void update(const std::string& alias, const FactoryMethod& f) {                                    \
      if (has(alias)) {                                                                                \
        remove(alias);                                                                                 \
      }                                                                                                \
      insert(alias, f);                                                                                \
    }                                                                                                  \
    template <typename G>                                                                              \
    void update(const std::string& alias) {                                                            \
      if (has(alias)) {                                                                                \
        remove(alias);                                                                                 \
      }                                                                                                \
      insert<G>(alias);                                                                                \
    }                                                                                                  \
    std::shared_ptr<T> Create(const std::string& alias) {                                              \
      auto factory = implementation_factories.find(alias);                                             \
      return (factory->second)();                                                                      \
    }                                                                                                  \
    std::unique_ptr<T> CreateUnique(const std::string& alias) {                                        \
      auto factory = implementation_unique_factories.find(alias);                                      \
      return (factory->second)();                                                                      \
    }                                                                                                  \
    T* CreateRaw(const std::string& alias) {                                                           \
      auto factory = implementation_raw_factories.find(alias);                                         \
      return (factory->second)();                                                                      \
    }                                                                                                  \
    T* CreatePlacement(const std::string& alias, void* place) {                                        \
      auto factory = implementation_placement_factories.find(alias);                                   \
      return (factory->second)(placement);                                                             \
    }                                                                                                  \
    static MIN_IMPLEMENTATION_HOLDER_NAME(T) * instance() {                                            \
      return static_cast<MIN_IMPLEMENTATION_HOLDER_NAME(T)*>(                                          \
          GetImplementationHolderInstance_##T());                                                      \
    }                                                                                                  \
  };                                                                                                   \
  extern MIN_IMPLEMENTATION_HOLDER_NAME(T) * MIN_IMPLEMENTATION_HOLDER_PTR(T);
}  // namespace min::unit