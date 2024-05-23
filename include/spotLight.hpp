#ifndef SPOTLIGHT_HPP
#define SPOTLIGHT_HPP

#include <string>
#include <glm/glm.hpp>

#include "Vec3.hpp"

using namespace std;

class SpotLight
{
public:

// Constructeur principal
SpotLight(Vec3 position, Vec3 direction, Vec3 ambient, Vec3 diffuse, Vec3 specular, float constant, float linear, float quadratic, float cutOff, float outerCutOff)
: mPosition(position), mAmbient(ambient), mDirection(direction), mDiffuse(diffuse), mSpecular(specular), mConstant(constant), mLinear(linear), mQuadratic(quadratic), mCutOff(cutOff), mOuterCutOff(outerCutOff) {}

float getCosCutOff() const { return glm::cos(glm::radians(mCutOff)); }

float getCosOuterCutOff() const { return glm::cos(glm::radians(mOuterCutOff)); }

private:

Vec3 mPosition;
Vec3 mDirection;
Vec3 mAmbient;
Vec3 mDiffuse;
Vec3 mSpecular;
float mConstant;
float mLinear;
float mQuadratic;
float mCutOff;
float mOuterCutOff;

};


#endif