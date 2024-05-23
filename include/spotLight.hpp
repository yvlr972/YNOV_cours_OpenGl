#ifndef SPOTLIGHT_HPP
#define SPOTLIGHT_HPP

#include <glm/glm.hpp>

class SpotLight
{
public:

// Constructeur principal
SpotLight(glm::vec3 position, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float constant, float linear, float quadratic, float cutOff, float outerCutOff)
: mPosition(position), mAmbient(ambient), mDirection(direction), mDiffuse(diffuse), mSpecular(specular), mConstant(constant), mLinear(linear), mQuadratic(quadratic), mCutOff(cutOff), mOuterCutOff(outerCutOff) {}

float getCosCutOff() const { return glm::cos(glm::radians(mCutOff)); }

float getCosOuterCutOff() const { return glm::cos(glm::radians(mOuterCutOff)); }

private:

glm::vec3 mPosition;
glm::vec3 mDirection;
glm::vec3 mAmbient;
glm::vec3 mDiffuse;
glm::vec3 mSpecular;
float mConstant;
float mLinear;
float mQuadratic;
float mCutOff;
float mOuterCutOff;

};


#endif