#pragma once

#include <filesystem>

namespace min::ray {

namespace fs = std::filesystem;

class Resolver {
 public:
  using iterator = std::vector<fs::path>::iterator;
  using const_iterator = std::vector<fs::path>::const_iterator;

  Resolver() {
    paths.emplace_back(fs::current_path());
  }
  size_t Size() const { return paths.size(); }

  iterator Begin() { return paths.begin(); }
  iterator End() { return paths.end(); }

  const_iterator Begin() const { return paths.begin(); }
  const_iterator End() const { return paths.end(); }

  void Erase(iterator it) { paths.erase(it); }
  void Prepend(const fs::path &path) { paths.insert(paths.begin(), path); }
  void Append(const fs::path &path) { paths.emplace_back(path); }

  const fs::path &operator[](size_t index) const { return paths[index]; }
  fs::path &operator[](size_t index) { return paths[index]; }

  fs::path Resolve(const fs::path &path) const {
    for (const_iterator it = paths.begin(); it != paths.end(); it++) {
      fs::path combined = *it / path;
      if (fs::exists(combined))
        return combined;
    }
    return path;
  }

  friend std::ostream &operator<<(std::ostream &os, const Resolver &r) {
    os << "Resolver[" << std::endl;
    for (size_t i = 0; i < r.paths.size(); ++i) {
      os << "  \"" << r.paths[i] << "\"";
      if (i + 1 < r.paths.size())
        os << ",";
      os << std::endl;
    }
    os << "]";
    return os;
  }
 private:
  std::vector<fs::path> paths;
};
}