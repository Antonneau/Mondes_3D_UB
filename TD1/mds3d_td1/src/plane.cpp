#include "plane.h"

Plane::Plane()
{
}

Plane::Plane(const PropertyList &propList)
{
    m_position = propList.getPoint("position",Point3f(0,0,0));
    m_normal = propList.getVector("normal",Point3f(0,0,1));
}

Plane::~Plane()
{
}

bool Plane::intersect(const Ray& ray, Hit& hit) const
{
    Point3f o = ray.origin;
    Vector3f d = ray.direction;
    Point3f cen = m_position;
    Point3f n = m_normal;

    float t = ((cen-o).dot(n))/(d.dot(n));

    bool res = false;
    if(t > 0){
        hit.setT(t);
        hit.setShape(this);
        hit.setNormal(m_normal);
        Vector2f uv(ray.at(hit.t()).x(), ray.at(hit.t()).y());
        hit.setUV(uv);
        res= true;
    }

    return res;
}

REGISTER_CLASS(Plane, "plane")
