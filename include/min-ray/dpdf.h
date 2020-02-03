
#pragma once

#include <min-ray/common.h>

namespace min::ray {

struct DiscretePDF {
 public:

  explicit DiscretePDF(size_t entries = 0) {
    Reserve(entries);
    Clear();
  }

  void Clear() {
    cdf.clear();
    cdf.push_back(0.0f);
    normalized_val = false;
  }

  void Reserve(size_t nEntries) {
    cdf.reserve(nEntries + 1);
  }

  void Append(float pdfValue) {
    cdf.push_back(cdf[cdf.size() - 1] + pdfValue);
  }

  size_t size() const {
    return cdf.size() - 1;
  }

  float operator[](size_t entry) const {
    return cdf[entry + 1] - cdf[entry];
  }

  bool normalized() const {
    return normalized_val;
  }

  float sum() const {
    return sum_val;
  }

  float normalization() const {
    return normalization_val;
  }

  float Normalize() {
    sum_val = cdf[cdf.size() - 1];
    if (sum_val > 0) {
      normalization_val = 1.0f / sum_val;
      for (size_t i = 1; i < cdf.size(); ++i)
        cdf[i] *= normalization_val;
      cdf[cdf.size() - 1] = 1.0f;
      normalized_val = true;
    } else {
      normalization_val = 0.0f;
    }
    return sum_val;
  }

  size_t Sample(float sampleValue) const {
    auto entry =
        std::lower_bound(cdf.begin(), cdf.end(), sampleValue);
    size_t index = std::max<size_t>((ptrdiff_t)0, entry - cdf.begin() - 1);
    return std::min<size_t>(index, cdf.size() - 2);
  }

  size_t Sample(float sampleValue, float &pdf) const {
    size_t index = Sample(sampleValue);
    pdf = operator[](index);
    return index;
  }

  size_t SampleReuse(float &sampleValue) const {
    size_t index = Sample(sampleValue);
    sampleValue = (sampleValue - cdf[index]) / (cdf[index + 1] - cdf[index]);
    return index;
  }

  size_t SampleReuse(float &sampleValue, float &pdf) const {
    size_t index = Sample(sampleValue, pdf);
    sampleValue = (sampleValue - cdf[index]) / (cdf[index + 1] - cdf[index]);
    return index;
  }

  std::string ToString() const {
    std::string result = fmt::format(
        "DiscretePDF[sum={}, "
        "normalized={}",
        sum_val, normalized_val);

    for (size_t i = 0; i < cdf.size(); ++i) {
      result += std::to_string(operator[](i));
      if (i != cdf.size() - 1)
        result += ", ";
    }
    return result + "}]";
  }

 private:
  std::vector<float> cdf;
  float sum_val, normalization_val;
  bool normalized_val;
};

}  // namespace min::ray
