#include "integrator.h"
#include "scene.h"

class DirectIntegrator : public Integrator {
public:
    DirectIntegrator(const PropertyList &props) {
        /* No parameters this time */
    }

    Color3f Li(const Scene *scene, const Ray &ray) const {

        Hit hit;
        Color3f color = scene->backgroundColor();
        scene->intersect(ray, hit);

        if(hit.foundIntersection()){
            color.setZero();
            Vector2f uv(hit.shape()->material()->textureScaleU(), hit.shape()->material()->textureScaleV());
            for(uint i = 0; i < scene->lightList().size(); i++){
                Point3f inter(ray.origin + (ray.direction*hit.t()));
                Color3f p = hit.shape()->material()->brdf(ray.direction, scene->lightList().at(i)->direction(inter), hit.normal(), uv);
                float theta = (scene->lightList().at(i)->direction(inter)).dot(hit.normal());
                if (theta < 0){
                    theta = 0;
                }

                color += p * theta * scene->lightList().at(i)->intensity(inter);
            }
        }

        return color;
    }

    std::string toString() const {
        return "DirectIntegrator[]";
    }
};

REGISTER_CLASS(DirectIntegrator, "direct")