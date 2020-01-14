#include "sphere.h"
#include <iostream>

Sphere::Sphere(float radius)
    : m_radius(radius)
{
}

Sphere::Sphere(const PropertyList &propList)
{
    m_radius = propList.getFloat("radius",1.f);
    m_center = propList.getPoint("center",Point3f(0,0,0));
}

Sphere::~Sphere()
{
}

bool Sphere::intersect(const Ray& ray, Hit& hit) const
{
    // Getting variables
    Point3f o = ray.origin;
    Vector3f d = ray.direction;
    Point3f cen = m_center;
    float r = m_radius;

    // Calculating the second degree equation's parameters
    float a = d.dot(d);
    float b = 2*d.dot(o-cen);
    float c = (o-cen).dot(o-cen) - (r*r);

    // Calculating the delta
    float delta = (b*b) - 4*a*c;

    // Ray intersecting with two points of the sphere
    if(delta > 0){
        float root1 = (-b + sqrt(delta))/(2*a);
        float root2 = (-b - sqrt(delta))/(2*a);

        // If both of the points are behind the camera
        if(root1 < 0 && root2 < 0){
            return false;
        }

        // If one of the points are behind the camera, get the visible one
        if(root1 < 0){
            hit.setT(root2);
        } else if(root2 < 0) {
            hit.setT(root1);
        // Getting the nearest point
        } else {
            if(root1 < root2){
                hit.setT(root1);
            } else {
                hit.setT(root2);
            }
        }
        return true;

    // Ray touching the sphere (one point)
    } else if(delta == 0){
        float root = (-b)/(2*a);
        // If the point is behind the camera
        if(root < 0){
            return false;
        }
        hit.setT(root);
        return true;
    }

    // No roots (intersections) were found, return false.
    return false;
}

REGISTER_CLASS(Sphere, "sphere")
