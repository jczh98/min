
#pragma once

#include <min-ray/common.h>

namespace min::ray {

/**
 * \brief Discrete probability distribution
 * 
 * This data structure can be used to transform uniformly distributed
 * samples to a stored discrete probability distribution.
 * 
 * \ingroup libcore
 */
struct DiscretePDF {
 public:
  /// Allocate memory for a distribution with the given number of entries
  explicit DiscretePDF(size_t entries = 0) {
    Reserve(entries);
    Clear();
  }

  /// Clear all entries
  void Clear() {
    cdf.clear();
    cdf.push_back(0.0f);
    normalized_val = false;
  }

  /// Reserve memory for a certain number of entries
  void Reserve(size_t nEntries) {
    cdf.reserve(nEntries + 1);
  }

  /// Append an entry with the specified discrete probability
  void Append(float pdfValue) {
    cdf.push_back(cdf[cdf.size() - 1] + pdfValue);
  }

  /// Return the number of entries so far
  size_t size() const {
    return cdf.size() - 1;
  }

  /// Access an entry by its index
  float operator[](size_t entry) const {
    return cdf[entry + 1] - cdf[entry];
  }

  /// Have the probability densities been normalized?
  bool normalized() const {
    return normalized_val;
  }

  /**
     * \brief Return the original (unnormalized) sum of all PDF entries
     *
     * This assumes that \ref normalize() has previously been called
     */
  float sum() const {
    return sum_val;
  }

  /**
     * \brief Return the normalization factor (i.e. the inverse of \ref getSum())
     *
     * This assumes that \ref normalize() has previously been called
     */
  float normalization() const {
    return normalization_val;
  }

  /**
     * \brief Normalize the distribution
     *
     * \return Sum of the (previously unnormalized) entries
     */
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

  /**
     * \brief %Transform a uniformly distributed sample to the stored distribution
     * 
     * \param[in] sampleValue
     *     An uniformly distributed sample on [0,1]
     * \return
     *     The discrete index associated with the sample
     */
  size_t Sample(float sampleValue) const {
    std::vector<float>::const_iterator entry =
        std::lower_bound(cdf.begin(), cdf.end(), sampleValue);
    size_t index = (size_t)std::max((ptrdiff_t)0, entry - cdf.begin() - 1);
    return std::min(index, cdf.size() - 2);
  }

  /**
     * \brief %Transform a uniformly distributed sample to the stored distribution
     * 
     * \param[in] sampleValue
     *     An uniformly distributed sample on [0,1]
     * \param[out] pdf
     *     Probability value of the sample
     * \return
     *     The discrete index associated with the sample
     */
  size_t Sample(float sampleValue, float &pdf) const {
    size_t index = Sample(sampleValue);
    pdf = operator[](index);
    return index;
  }

  /**
     * \brief %Transform a uniformly distributed sample to the stored distribution
     * 
     * The original sample is value adjusted so that it can be "reused".
     *
     * \param[in, out] sampleValue
     *     An uniformly distributed sample on [0,1]
     * \return
     *     The discrete index associated with the sample
     */
  size_t SampleReuse(float &sampleValue) const {
    size_t index = Sample(sampleValue);
    sampleValue = (sampleValue - cdf[index]) / (cdf[index + 1] - cdf[index]);
    return index;
  }

  /**
     * \brief %Transform a uniformly distributed sample. 
     * 
     * The original sample is value adjusted so that it can be "reused".
     *
     * \param[in,out]
     *     An uniformly distributed sample on [0,1]
     * \param[out] pdf
     *     Probability value of the sample
     * \return
     *     The discrete index associated with the sample
     */
  size_t SampleReuse(float &sampleValue, float &pdf) const {
    size_t index = Sample(sampleValue, pdf);
    sampleValue = (sampleValue - cdf[index]) / (cdf[index + 1] - cdf[index]);
    return index;
  }

  /**
     * \brief Turn the underlying distribution into a
     * human-readable string format
     */
  std::string ToString() const {
    std::string result = tfm::format(
        "DiscretePDF[sum=%f, "
        "normalized=%f, pdf = {",
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
