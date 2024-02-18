#ifndef GAMEOBJECT_HPP
#define GAMEOBJECT_HPP

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "model.hpp"
#include "shader.hpp"

using namespace std;

class GameObject
{
public:
    GameObject(string name, string path, bool flipTextureVertically, Shader &shader);
    GameObject(string path, bool flipTextureVertically, Shader &shader) : GameObject("", path, flipTextureVertically, shader)
    {}
    GameObject(string name, string path, bool flipTextureVertically, Shader &shader, glm::mat4 modelMatrix) : GameObject("", path, flipTextureVertically, shader)
    {this->modelMatrix = modelMatrix;}
    ~GameObject() { graphicModel.CleanUp(); }
    void Draw();

    glm::mat4 getModelMatrix() { return modelMatrix; }
    void modelMatrixTranslate(glm::vec3 translation) { modelMatrix = glm::translate(modelMatrix, translation); }
    void modelMatrixRotate(float angle, glm::vec3 axis) { modelMatrix = glm::rotate(modelMatrix, angle, axis); }
    void modelMatrixScale(glm::vec3 scale) { modelMatrix = glm::scale(modelMatrix, scale); }

private:
    string name;
    Model graphicModel;
    Shader &shader;
    glm::mat4 modelMatrix = glm::mat4(1.0f);
};

#endif