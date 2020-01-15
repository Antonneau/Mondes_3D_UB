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
    float b = 2.f*d.dot(o-cen);
    float c = (o-cen).dot(o-cen) - (r*r);

    // Calculating the delta
    float delta = (b*b) - 4*a*c;

    // If there is at least on intersection, set it to true.
    bool res = false;
    // Ray intersecting with one or two points of the sphere
    if(delta >= 0){
        // Ray touching the sphere (one point)
        if(delta == 0){
            float root = (-b)/(2*a);
            hit.setT(root);
            hit.setShape(this);
        } else {
            float root1 = (-b + sqrt(delta))/(2*a);
            float root2 = (-b - sqrt(delta))/(2*a);

            // If one of the points are behind the camera, get the visible one
            if(root2 < 0){
                hit.setT(root1);
                hit.setShape(this);
            } else {
                hit.setT(root2);
                hit.setShape(this);
            }
        }
        res = true;
    } 
    
    return res;
}

REGISTER_CLASS(Sphere, "sphere")
