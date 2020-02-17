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
    float b = 2.*ray.direction.dot(ray.origin - m_center);
    float c = (ray.origin - m_center).squaredNorm() - m_radius*m_radius;
    float discr = b*b - 4.*c;
    if(discr>=0)
    {
        discr = std::sqrt(discr);
        float t = 0.5*(-b - discr);
        if(t<Epsilon)
            t = 0.5*(-b + discr);
        if(t<Epsilon)
            return false;

        hit.setT(t);
        Point3f point = (ray.at(t) - m_center);
        hit.setNormal(point.normalized());

        // Texture coordinates
        float x = point[1];
        float y = point[2];
        float z = point[0];
        float longitude = atan2(x, z); // rotation autour de Y
        float latitude = atan2(y, sqrt(x*x + z*z)); // Hauteur sur Y

        longitude = 1.f - (longitude + M_PI_2) / (2.f*M_PI);
        latitude = fabs((latitude + M_PI_2) / M_PI);

        hit.setTexcoord(Eigen::Vector2f(longitude - floor(longitude),latitude));

        return true;
    }
    return false;
}

REGISTER_CLASS(Sphere, "sphere")
