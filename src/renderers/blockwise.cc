#include <min-ray/bitmap.h>
#include <min-ray/block.h>
#include <min-ray/camera.h>
#include <min-ray/preview_gui.h>
#include <min-ray/integrator.h>
#include <min-ray/sampler.h>
#include <min-ray/scene.h>
#include <min-ray/timer.h>
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <thread>

namespace min::ray {

/// Blockwise render mode
class Blockwise : public Renderer {
 public:

  void initialize(const json &json) override {
    integrator = CreateInstance<Integrator>(json["integrator"]["type"]);
    spp = json["spp"].get<int>();
  }

  void Render() override {
    auto camera = scene->camera;
    Vector2i outputSize = camera->GetOutputSize();
    integrator->Preprocess(scene);

    // Create a block generator (i.e. a work scheduler)
    BlockGenerator blockGenerator(outputSize, MIN_BLOCK_SIZE);

    // Allocate memory for the entire output image and clear it
    ImageBlock result(outputSize, camera->filter);
    result.Clear();

    // Create a window that visualizes the partially rendered result
    auto gui = std::make_unique<PreviewGUI>(result);
    gui->Init();

    // Do the following in parallel and asynchronously
    std::thread render_thread([&] {
      MIN_DEBUG("Rendering .. ");
      Timer timer;

      tbb::blocked_range<int> range(0, blockGenerator.GetBlockCount());

      auto map = [&](const tbb::blocked_range<int> &range) {
        // Allocate memory for a small image block to be rendered by the current thread
        ImageBlock block(Vector2i(MIN_BLOCK_SIZE),
                         camera->filter);

        // Create a clone of the sampler for the current thread
        std::unique_ptr<Sampler> sampler(scene->sampler->Clone());

        for (int i = range.begin(); i < range.end(); ++i) {
          // Request an image block from the block generator
          blockGenerator.Next(block);

          // Inform the sampler about the block to be rendered
          sampler->Prepare(block);

          // Render all contained pixels
          RenderBlock(scene, sampler.get(), block);

          // The image block has been processed. Now add it to the "big" block that represents the entire image
          result.Put(block);
        }
      };

      // Uncomment the following line for single threaded rendering
      // map(range);

      // Default: parallel rendering
      tbb::parallel_for(range, map);

      MIN_DEBUG("Done. (taken {})", timer.ElapsedString());
    });

    // Enter the application main loop
    gui->Mainloop();

    // Shut down the user interface
    render_thread.join();

    gui->Shutdown();

    // Now turn the rendered image block into a properly normalized bitmap
    std::unique_ptr<Bitmap> bitmap(result.ToBitmap());

    // Determine the filename of the output bitmap
    std::string outputName = filename;
    size_t lastdot = outputName.find_last_of(".");
    if (lastdot != std::string::npos)
      outputName.erase(lastdot, std::string::npos);
    outputName += ".png";

    // Save using the png format
    bitmap->Save(outputName);
  }

  void RenderBlock(const std::shared_ptr<Scene> scene, Sampler *sampler, ImageBlock &block) {
    const auto camera = scene->camera;

    Point2i offset = block.GetOffset();
    Vector2i size = block.GetSize();

    // Clear the block contents
    block.Clear();

    // For each pixel and pixel sample sample
    for (int y = 0; y < size.y(); ++y) {
      for (int x = 0; x < size.x(); ++x) {
        for (uint32_t i = 0; i < spp; ++i) {
          Point2f pixelSample = Point2f((float)(x + offset.x()), (float)(y + offset.y())) + sampler->Next2D();
          Point2f apertureSample = sampler->Next2D();

          // Sample a ray from the camera
          Ray3f ray;
          Color3f value = camera->SampleRay(ray, pixelSample, apertureSample);

          // Compute the incident radiance
          value *= integrator->Li(scene, sampler, ray);

          // Store in the image block
          block.Put(pixelSample, value);
        }
      }
    }
  }

  std::string ToString() const {
    return fmt::format("Blockwise[]");
  }

 private:
};
MIN_IMPLEMENTATION(Renderer, Blockwise, "blockwise")

}  // namespace min::ray
