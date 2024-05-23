#ifndef POINTLIGHT_HPP
#define POINTLIGHT_HPP

#include <glm/vec3.hpp>


class PointLight
{
public:

// Constructeur principal
PointLight::PointLight(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float constant, float linear, float quadratic)
 : mPosition(position), mAmbient(ambient), mDiffuse(diffuse), mSpecular(specular), mConstant(constant), mLinear(linear), mQuadratic(quadratic) {}


PointLight::PointLight(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float constant, float linear, float quadratic, glm::vec3 cubeRGB) {}


private:

glm::vec3 mPosition;
glm::vec3 mAmbient;
glm::vec3 mDiffuse;
glm::vec3 mSpecular;
float mConstant;
float mLinear;
float mQuadratic;

glm::vec3 mCubeRGB;

};


#endif