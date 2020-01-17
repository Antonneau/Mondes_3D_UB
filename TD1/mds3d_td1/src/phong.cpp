#include "material.h"

Phong::Phong(const PropertyList &propList)
    : Diffuse(propList.getColor("diffuse",Color3f(0.2)))
{
    m_specularColor = propList.getColor("specular",Color3f(0.9));
    m_reflectivity = propList.getColor("reflectivity",Color3f(0.0));
    m_exponent = propList.getFloat("exponent",0.2);

    std::string texturePath = propList.getString("texture","");
    if(texturePath.size()>0){
        filesystem::path filepath = getFileResolver()->resolve(texturePath);
        loadTextureFromFile(filepath.str());
        setTextureScale(propList.getFloat("scale",1));
        setTextureMode(TextureMode(propList.getInteger("mode",0)));
    }
}

Color3f Phong::brdf(const Vector3f& viewDir, const Vector3f& lightDir, const Normal3f& normal, const Vector2f& uv) const
{
    // R = 2(N • L) N-L
    Vector3f reflect = 2*(normal.dot(lightDir)) * normal - lightDir;
    // m_specularColor for ms
    // m_exponent for s

    // (r.v)
    float alpha = reflect.dot(viewDir);
    // Getting max(r.v, 0)
    if(alpha < 0){
        alpha = 0;
    }

    // max(r.v, 0)pow(s)
    float beta = pow(alpha, m_exponent);

    // ps = ms(max(r.v, 0))pow(s)
    Color3f ps = m_specularColor*beta;

    // return p = ps + pd
    return ps + diffuseColor(uv);
}

std::string Phong::toString() const {
    return tfm::format(
        "Phong[\n"
        "  diffuse color = %s\n"
        "  specular color = %s\n"
        "  exponent = %f\n"
        "]", m_diffuseColor.toString(),
             m_specularColor.toString(),
             m_exponent);
}

REGISTER_CLASS(Phong, "phong")
