#include "light.h"

class PointLight : public Light
{
public:
    PointLight(const PropertyList &propList)
        : Light(propList.getColor("intensity", Color3f(1.f)))
    {
        m_position = propList.getPoint("position", Point3f::UnitX());
    }

    Vector3f direction(const Point3f& x, float* dist = 0) const
    {
        if(dist)
            *dist = std::numeric_limits<float>::max();
        return m_position - x;
    }

    Color3f intensity(const Point3f& x) const
    {
        // w'(x)
        Vector3f dir = direction(x);
        // Intensity = i(l) / ||w'(x)||²
        Color3f Vx = m_intensity / (dir).dot(dir);

        return Vx;
    }

    std::string toString() const {
        return tfm::format(
            "PointLight[\n"
            "  intensity = %s\n"
            "  position = %s\n"
            "]", m_intensity.toString(),
                 ::toString(m_position));
    }

protected:
    Point3f m_position;
};

REGISTER_CLASS(PointLight, "pointLight")
