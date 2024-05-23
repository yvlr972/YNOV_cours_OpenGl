#ifndef POINTLIGHT_HPP
#define POINTLIGHT_HPP

#include <glm/vec3.hpp>


class PointLight
{
public:

// Constructeur principal
PointLight() {}

PointLight(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float constant, float linear, float quadratic, glm::vec3 cubeRGB)
 : mPosition(position), mAmbient(ambient), mDiffuse(diffuse), mSpecular(specular), mConstant(constant), mLinear(linear), mQuadratic(quadratic), mCubeRGB(cubeRGB) {}


PointLight(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float constant, float linear, float quadratic)
 : mPosition(position), mAmbient(ambient), mDiffuse(diffuse), mSpecular(specular), mConstant(constant), mLinear(linear), mQuadratic(quadratic), mCubeRGB(diffuse) {}


void setPosition(glm::vec3 position) { mPosition = position; }
void setAmbient(glm::vec3 ambient) { mAmbient = ambient; }
void setDiffuse(glm::vec3 diffuse) { mDiffuse = diffuse; }
void setSpecular(glm::vec3 specular) { mSpecular = specular; }
void setConstant(float constant) { mConstant = constant; }
void setLinear(float linear) { mLinear = linear; }
void setQuadratic(float quadratic) { mQuadratic = quadratic; }
void setCubeRGB(glm::vec3 cubeRGB) { mCubeRGB = cubeRGB; }

glm::vec3 getPosition() { return mPosition; }
glm::vec3 getAmbient() { return mAmbient; }
glm::vec3 getDiffuse() { return mDiffuse; }
glm::vec3 getSpecular() { return mSpecular; }
float getConstant() { return mConstant; }
float getLinear() { return mLinear; }
float getQuadratic() { return mQuadratic; }
glm::vec3 getCubeRGB() { return mCubeRGB; }

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