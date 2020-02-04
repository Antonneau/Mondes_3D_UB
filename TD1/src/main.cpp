#include "viewer.h"

#include <filesystem/resolver.h>

void render(Scene* scene, ImageBlock* result, std::string outputName, bool* done)
{
    if(!scene)
        return;

    clock_t t = clock();

    const Camera *camera = scene->camera();
    const Integrator* integrator = scene->integrator();
    integrator->preprocess(scene);

    float tanfovy2 = tan(camera->fovY()*0.5);
    Vector3f camX = camera->right() * tanfovy2 * camera->nearDist() * float(camera->vpWidth())/float(camera->vpHeight());
    Vector3f camY = -camera->up() * tanfovy2 * camera->nearDist();
    Vector3f camF = camera->direction() * camera->nearDist();

    uint camWidth = camera->vpWidth();
    uint camHeight = camera->vpHeight();
    // iterate over the image pixels
    for(uint y = 0; y < camHeight; y++){
        for(uint x = 0; x < camWidth; x++){
            // Getting the origin and the direction depending on the pixel
            Vector3f pixDir = camF + 2.f*(x/(float)camWidth - 0.5)*camX + 2.f*(y/(float)camHeight - 0.5)*camY;
            pixDir.normalize();

            // generate a primary ray
            Ray pixRay(camera->position(), pixDir);

            // call the integartor to compute the color along this ray
            Color3f pixCol = integrator->Li(scene, pixRay);

            // write this color in the result image
            Vector2f pixel(x, y);
            result->put(pixel, pixCol);
        }
    }

    t = clock() - t;
    std::cout << "Raytracing time : " << float(t)/CLOCKS_PER_SEC << "s"<<std::endl;

    *done = true;
}

int main(int argc, char *argv[])
{
    getFileResolver()->prepend(DATA_DIR);

    try {
        nanogui::init();
        Viewer *screen = new Viewer();;

        if (argc == 2) {
            /* load file from the command line */
            filesystem::path path(argv[1]);

            if(path.extension() == "scn") { // load scene file
                screen->loadScene(argv[1]);
            }else if(path.extension() == "exr") { // load OpenEXR image
                screen->loadImage(argv[1]);
            }
        }

        /* Enter the application main loop */
        nanogui::mainloop();

        delete screen;
        nanogui::shutdown();
    } catch (const std::exception &e) {
        cerr << "Fatal error: " << e.what() << endl;
        return -1;
    }
    return 0;
}
