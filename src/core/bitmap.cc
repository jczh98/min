#include <lodepng.h>
#include <min-ray/bitmap.h>

namespace min::ray {

Bitmap::Bitmap(const std::string &filename) {
}

void Bitmap::Save(const std::string &filename) {
    std::vector<unsigned char> pixelBuffer;
    auto toInt = [=](float x){
        return std::max<uint32_t>(0, std::min<uint32_t>(255, std::lroundf(std::pow(x, 1.0f / 2.2f) * 255)));
    };
    uint32_t width = cols();
    uint32_t height = rows();
    for (int i = 0; i < width * height; i++) {
        pixelBuffer.emplace_back(toInt(data()[i][0]));
        pixelBuffer.emplace_back(toInt(data()[i][1]));
        pixelBuffer.emplace_back(toInt(data()[i][2]));
        pixelBuffer.emplace_back(255);
    }
    auto error = lodepng::encode(filename, pixelBuffer, width, height);
    if (error) {
        MIN_ERROR("Error saving {}: {}\n", filename, lodepng_error_text(error));
    } else {
        MIN_DEBUG("Saved to {}\n", filename);
    }
}

}  // namespace min::ray
