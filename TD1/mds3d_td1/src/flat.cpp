#include "integrator.h"
#include "scene.h"

class FlatIntegrator : public Integrator {
public:
    FlatIntegrator(const PropertyList &props) {
        /* No parameters this time */
    }

    Color3f Li(const Scene *scene, const Ray &ray) const {
        Hit hit;
        Color3f color = scene->backgroundColor();
        scene->intersect(ray, hit);
        if(hit.foundIntersection()){
            color = hit.shape()->material()->ambientColor();
        }
        return color;
    }

    std::string toString() const {
        return "FlatIntegrator[]";
    }
};

REGISTER_CLASS(FlatIntegrator, "flat")
