#ifndef SPOTLIGHT_HPP
#define SPOTLIGHT_HPP

#include <glm/glm.hpp>

class SpotLight
{
public:

// Constructeur principal
SpotLight() {}

SpotLight(glm::vec3 position, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float constant, float linear, float quadratic, float cutOff, float outerCutOff)
: mPosition(position), mAmbient(ambient), mDirection(direction), mDiffuse(diffuse), mSpecular(specular), mConstant(constant), mLinear(linear), mQuadratic(quadratic), mCutOff(cutOff), mOuterCutOff(outerCutOff) {}

float getCosCutOff() const { return glm::cos(glm::radians(mCutOff)); }
float getCosOuterCutOff() const { return glm::cos(glm::radians(mOuterCutOff)); }

void setPosition(glm::vec3 position) { mPosition = position; }
void setDirection(glm::vec3 direction) { mDirection = direction; }
void setAmbient(glm::vec3 ambient) { mAmbient = ambient; }
void setDiffuse(glm::vec3 diffuse) { mDiffuse = diffuse; }
void setSpecular(glm::vec3 specular) { mSpecular = specular; }
void setConstant(float constant) { mConstant = constant; }
void setLinear(float linear) { mLinear = linear; }
void setQuadratic(float quadratic) { mQuadratic = quadratic; }
void setCutOff(float cutOff) { mCutOff = cutOff; }
void setOuterCutOff(float outerCutOff) { mOuterCutOff = outerCutOff; }

glm::vec3 getPosition() { return mPosition; }
glm::vec3 getDirection() { return mDirection; }
glm::vec3 getAmbient() { return mAmbient; }
glm::vec3 getDiffuse() { return mDiffuse; }
glm::vec3 getSpecular() { return mSpecular; }
float getConstant() { return mConstant; }
float getLinear() { return mLinear; }
float getQuadratic() { return mQuadratic; }
float getCutOff() { return mCutOff; }
float getOuterCutOff() { return mOuterCutOff; }

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