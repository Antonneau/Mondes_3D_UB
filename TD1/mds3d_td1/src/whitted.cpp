#include "integrator.h"
#include "scene.h"

class WhittedIntegrator : public Integrator {
public:
    int maxRecursion;

    WhittedIntegrator(const PropertyList &props) {
        maxRecursion = props.getInteger("maxRecursion");
    }

    Color3f Li(const Scene *scene, const Ray &ray) const {
        Hit hit;
        Color3f color(0, 0, 0);
        scene->intersect(ray, hit);
        if(hit.foundIntersection()){
            color.setZero();
            Point3f inter(ray.origin + (ray.direction*hit.t()));
            for(uint i = 0; i < scene->lightList().size(); i++){
                float dist;
                Vector3f light = (scene->lightList().at(i)->direction(inter, &dist));
                light.normalize();
                //Vector3f view = ray.direction.normalized();
                Color3f p = hit.shape()->material()->brdf(ray.direction, -light, hit.normal(), hit.uv());
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
            // Calculer le vecteur refracté
            float nA = hit.shape()->material()->etaA();
            float nB = hit.shape()->material()->etaB();
            // cos(theta1)
            float theta1 = hit.normal().dot(-ray.direction);
            float theta2 = sqrt(1 - pow(nA/nB,2) * (1 - pow(theta1, 2)));

            Vector3f trans;
            if (theta1 >= 0){
                trans = (nA/nB)*ray.direction + ((nA/nB)*theta1 - theta2) * hit.normal();
            } else {
                trans = (nA/nB)*ray.direction + ((nA/nB)*theta1 + theta2) * hit.normal();
            }

            Ray transmissivness(inter + hit.normal() * 0.0001, trans);
            Hit transHit;
            scene->intersect(transmissivness, transHit);
            if (transHit.foundIntersection()){
                color += Li(scene, transmissivness) * hit.shape()->material()->transmissivness();
            }

            if (ray.recursionLevel < maxRecursion){
                Vector3f recDir = 2*(hit.normal().dot(-ray.direction)) * hit.normal() + ray.direction;
                recDir.normalize();

                Ray recRay(inter + hit.normal() * 0.0001, recDir);
                recRay.recursionLevel = ray.recursionLevel + 1;

                color += Li(scene, recRay) * hit.shape()->material()->reflectivity() * recDir.dot(hit.normal());
            }
        } else if (ray.recursionLevel == 0){
            color = scene->backgroundColor();
        }
        
        return color;
    }

    std::string toString() const {
        return "WhittedIntegrator[]";
    }
};

REGISTER_CLASS(WhittedIntegrator, "whitted")
