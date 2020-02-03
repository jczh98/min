#include <min-ray/bitmap.h>
#include <min-ray/block.h>
#include <min-ray/camera.h>
#include <min-ray/integrator.h>
#include <min-ray/preview_gui.h>
#include <min-ray/sampler.h>
#include <min-ray/scene.h>
#include <min-ray/timer.h>
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <thread>

namespace min::ray {

/// Progressive render mode
class Progressive : public RenderMode {
 public:
  //Progressive(const PropertyList &){}

  virtual ~Progressive() {}

  void Render(Scene *scene, const std::string &filename) {
    const Camera *camera = scene->camera.get();
    Sampler *sampler = scene->sampler.get();
    Vector2i outputSize = camera->GetOutputSize();
    scene->integrator->Preprocess(scene);

    /* Create a block generator (i.e. a work scheduler) */
    BlockGenerator blockGenerator(outputSize, NORI_BLOCK_SIZE);

    /* Allocate memory for the entire output image and clear it */
    ImageBlock result(outputSize, camera->filter.get());
    result.Clear();

    auto gui = std::make_unique<PreviewGUI>(result);
    gui->Init();

    /* Do the following in parallel and asynchronously */
    std::thread render_thread([&] {
      cout << "Rendering .. \n";
      cout.flush();
      Timer timer;

      tbb::blocked_range<int> range(0, outputSize.x() * outputSize.y());

      auto map = [&](const tbb::blocked_range<int> &range) {
        /* Allocate memory for a small image block to be rendered
        /* Create a clone of the sampler for the current thread */
        std::unique_ptr<Sampler> sampler(scene->sampler->Clone());
        sampler->Prepare(result);
        for (int i = range.begin(); i < range.end(); ++i) {

          //sampler->Prepare(result);

          const Camera *camera = scene->camera.get();
          const Integrator *integrator = scene->integrator.get();

          const int x = i % outputSize.x(), y = i / outputSize.x();
          Point2f pixelSample = Point2f((float) x, (float) y) + sampler->Next2D();
          Point2f apertureSample = sampler->Next2D();

          /* Sample a ray from the camera */
          Ray3f ray;
          Color3f value = camera->SampleRay(ray, pixelSample, apertureSample);

          /* Compute the incident radiance */
          value *= integrator->Li(scene, sampler.get(), ray);

          /* Store in the image block */
          result.Put(pixelSample, value);

        }
      };

      for (uint32_t i = 0; i < sampler->GetSampleCount(); ++i) {
        // Print and update current sample idx
        cout << "Current sample: " << i + 1 << "\r" << std::flush;

        /// Uncomment the following line for single threaded rendering
        // map(range);

        /// Default: parallel rendering
        tbb::parallel_for(range, map);
      }

      cout << "\n done. (took " << timer.ElapsedString() << ")" << endl;
    });

    gui->Mainloop();
    render_thread.join();
    gui->Shutdown();

    /* Now turn the rendered image block into
	       a properly normalized bitmap */
    std::unique_ptr<Bitmap> bitmap(result.ToBitmap());

    /* Determine the filename of the output bitmap */
    std::string outputName = filename;
    size_t lastdot = outputName.find_last_of(".");
    if (lastdot != std::string::npos)
      outputName.erase(lastdot, std::string::npos);
    outputName += ".png";

    /* Save using the OpenEXR format */
    bitmap->Save(outputName);
  }

  std::string ToString() const {
    return fmt::format("Progressive[]");
  }

 private:
};
MIN_IMPLEMENTATION(RenderMode, Progressive, "progressive")
//NORI_REGISTER_CLASS(Progressive, "progressive");
}  // namespace min::ray
