#include "integrator.h"
#include "scene.h"

class NormalsIntegrator : public Integrator {
public:
    NormalsIntegrator(const PropertyList &props) {
        /* No parameters this time */
    }

    Color3f Li(const Scene *scene, const Ray &ray) const {
        Hit hit;
        Color3f color = scene->backgroundColor();
        scene->intersect(ray, hit);
        if(hit.foundIntersection()){
            float x = abs(hit.normal().x());
            float y = abs(hit.normal().y());
            float z = abs(hit.normal().z());
            Color3f newColor(x, y, z);
            color = newColor;
        }
        return color;
    }

    std::string toString() const {
        return "NormalsIntegrator[]";
    }
};

REGISTER_CLASS(NormalsIntegrator, "normals")