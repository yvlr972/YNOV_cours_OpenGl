#ifndef POINTLIGHT_HPP
#define POINTLIGHT_HPP

#include "Vec3.hpp"

using namespace std;

class PointLight
{
public:

// Constructeur principal
PointLight::PointLight(Vec3 position, Vec3 ambient, Vec3 diffuse, Vec3 specular, float constant, float linear, float quadratic)
 : mPosition(position), mAmbient(ambient), mDiffuse(diffuse), mSpecular(specular), mConstant(constant), mLinear(linear), mQuadratic(quadratic) {}


PointLight::PointLight(Vec3 position, Vec3 ambient, Vec3 diffuse, Vec3 specular, float constant, float linear, float quadratic, Vec3 cubeRGB) {}


private:

Vec3 mPosition;
Vec3 mAmbient;
Vec3 mDiffuse;
Vec3 mSpecular;
float mConstant;
float mLinear;
float mQuadratic;

Vec3 mCubeRGB;

};


#endif