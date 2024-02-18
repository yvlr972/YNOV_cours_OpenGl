#include "gameObject.hpp"

#include <iostream>

// Ajouter : option sans name, prend directement le nom du fichier, ou option faire passer direcement une matrice de model à la création
GameObject::GameObject(string name, string path, bool flipTextureVertically, Shader &shader) : shader(shader), graphicModel(path, flipTextureVertically)
{
    if (name.empty())
    {
        // On récupère juste le nom du fichier sans l'extension
        size_t lastSlash = path.find_last_of("/\\");
        size_t lastDot = path.find_last_of(".");
        this->name = path.substr(lastSlash + 1, lastDot - lastSlash - 1);
    }
    else
    {
        this->name = name;
    }
}

void GameObject::Draw()
{
    shader.use();
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
    graphicModel.Draw(shader);
}