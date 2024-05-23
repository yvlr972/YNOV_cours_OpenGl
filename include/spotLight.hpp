#ifndef SPOTLIGHT_HPP
#define SPOTLIGHT_HPP

#include <string>
#include <glm/glm.hpp>

#include "Vec3.hpp"

using namespace std;

class SpotLight
{
public:


float getCosCutOff() const { return glm::cos(glm::radians(cutOff)); }

float getCosOuterCutOff() const { return glm::cos(glm::radians(outerCutOff)); }

private:

Vec3 position;
Vec3 direction;
Vec3 ambient;
Vec3 diffuse;
Vec3 specular;
float constant;
float linear;
float quadratic;
float cutOff;
float outerCutOff;

};


#endif