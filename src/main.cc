
#include <min-ray/parser.h>
#include <min-ray/scene.h>
#include <min-ray/camera.h>
#include <min-ray/block.h>
#include <min-ray/timer.h>
#include <min-ray/bitmap.h>
#include <min-ray/sampler.h>
#include <min-ray/integrator.h>
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <filesystem/resolver.h>
#include <thread>

using namespace min::ray;

int main(int argc, char **argv) {
    //if (argc != 2) {
    //    cerr << "Syntax: " << argv[0] << " <scene.xml>" << endl;
    //    return -1;
    //}
    std::string tmp = "E:\\work\\min-ray\\assets\\cornell_box\\cornell_box.xml";
    filesystem::path path(tmp);
    try {
        if (path.extension() == "xml") {
            /* Add the parent directory of the scene file to the
               file resolver. That way, the XML file can reference
               resources (OBJ files, textures) using relative paths */
            getFileResolver()->prepend(path.parent_path());

            std::unique_ptr<NoriObject> root(loadFromXML(tmp));

            /* When the XML root object is a scene, start rendering it .. */
            if (root->getClassType() == NoriObject::EScene) {
            	Scene *scene = static_cast<Scene *>(root.get());
            	RenderMode *rendermode = scene->GetRenderMode();
            	rendermode->Render(scene, tmp);
            }
        } else if (path.extension() == "exr") {
            ///* Alternatively, provide a basic OpenEXR image viewer */
            //Bitmap bitmap(argv[1]);
            //ImageBlock block(Vector2i((int) bitmap.cols(), (int) bitmap.rows()), nullptr);
            //block.FromBitmap(bitmap);
            //nanogui::init();
            //NoriScreen *screen = new NoriScreen(block);
            //nanogui::mainloop();
            //delete screen;
            //nanogui::shutdown();
        } else {
            cerr << "Fatal error: unknown file \"" << argv[1]
                 << "\", expected an extension of type .xml or .exr" << endl;
        }
    } catch (const std::exception &e) {
        cerr << "Fatal error: " << e.what() << endl;
        return -1;
    }
    return 0;
}
