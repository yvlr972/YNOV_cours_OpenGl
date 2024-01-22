#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "constants.hpp"

// Une classe de caméra abstraite qui traite les entrées et calcule les angles d'Euler, les vecteurs et les matrices correspondants pour une utilisation dans OpenGL
class Camera
{
public:
    // Position de la caméra dans le monde
    glm::vec3 Position;
    // Vecteur de direction (regard) de la caméra 
    glm::vec3 Front;
    // Vecteur haut de la caméra
    glm::vec3 Up;
    // Vecteur droite de la caméra
    glm::vec3 Right;
    // Vecteur haut du monde
    glm::vec3 WorldUp;
    // Les angles d'Euler (en degrés, non en radians).
    float Yaw;
    float Pitch;
    // Options de la caméra
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // Constructeur avec des vecteurs
    Camera(glm::vec3 position = CAMERA_START_POSITION, glm::vec3 up = CAMERA_START_UP, float yaw = START_CAMERA_YAW, float pitch = 0.0);
    // Constructeur avec des scalaires (valeurs par défaut)
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

    // Renvoie la matrice de vue calculée à l'aide des angles d'Euler et de la matrice LookAt
    glm::mat4 GetViewMatrix();

    // Traite l'entrée reçue de tout système d'entrée de type clavier. Accepte le paramètre d'entrée sous la forme d'une énumération définie par la caméra (pour l'abstraire des systèmes de fenêtrage)
    void ProcessKeyboard(CameraMovement direction, float deltaTime);

    // Traite l'entrée reçue d'un système d'entrée de type souris. Attend la valeur de décalage dans les deux directions x et y.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);

    // Traite l'entrée reçue d'un événement de molette de défilement de souris. N'exige d'entrée que sur l'axe de la roue verticale
    void ProcessMouseScroll(float yoffset);

private:
    // Calcule le vecteur front de la caméra à partir des angles d'Euler (mis à jour) de la caméra
    void updateCameraVectors();
};

#endif