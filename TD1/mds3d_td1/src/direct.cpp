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
            for(uint i = 0; i < scene->lightList().size(); i++){
                Point3f inter(ray.origin + (ray.direction*hit.t()));
                float dist;
                Vector3f light = (scene->lightList().at(i)->direction(inter, &dist));
                light.normalize();
                //Vector3f view = ray.direction.normalized();
                Color3f p = hit.shape()->material()->brdf(ray.direction, -light, hit.normal());
                Ray visibility(inter + hit.normal() * 0.0001, light);
                Hit hitVis;
                scene->intersect(visibility , hitVis);
                if (!hitVis.foundIntersection() || hitVis.t() > dist){
                    float theta = light.dot(hit.normal());
                    if(theta < 0){
                        theta = 0;
                    }
                    color += p * theta * scene->lightList().at(i)->intensity(inter);
                }
            }
        }

        return color;
    }

    std::string toString() const {
        return "DirectIntegrator[]";
    }
};

REGISTER_CLASS(DirectIntegrator, "direct")