#include "camera.hpp"

// Constructeur avec des vecteurs
Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) : Front(CAMERA_START_FRONT), MovementSpeed(CAMERA_SPEED), MouseSensitivity(MOUSE_SENSITIVITY), Zoom(START_FOV)
{
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

// Constructeur avec des scalaires (valeurs par défaut)
Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(CAMERA_START_FRONT), MovementSpeed(CAMERA_SPEED), MouseSensitivity(MOUSE_SENSITIVITY), Zoom(START_FOV)
{
    Position = glm::vec3(posX, posY, posZ);
    WorldUp = glm::vec3(upX, upY, upZ);
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

// Renvoie la matrice de vue calculée à l'aide des angles d'Euler et de la matrice LookAt
glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(Position, Position + Front, Up);
}

// Traite l'entrée reçue de tout système d'entrée de type clavier. Accepte le paramètre d'entrée sous la forme d'une énumération définie par la caméra (pour l'abstraire des systèmes de fenêtrage)
void Camera::ProcessKeyboard(CameraMovement direction, float deltaTime)
{
    float velocity = MovementSpeed * deltaTime;
    if (direction == FORWARD)
        Position += Front * velocity;
    if (direction == BACKWARD)
        Position -= Front * velocity;
    if (direction == LEFT)
        Position -= Right * velocity;
    if (direction == RIGHT)
        Position += Right * velocity;
}

// Traite l'entrée reçue d'un système d'entrée de type souris. Attend la valeur de décalage dans les deux directions x et y.
void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    // Lorsque le pitch est hors limites, l'écran ne se retourne pas.
    if (constrainPitch)
    {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    // Mettre à jour les vecteurs Front, Right et Up à l'aide des angles d'Euler mis à jour
    updateCameraVectors();
}

// Traite l'entrée reçue d'un événement de molette de défilement de souris. N'exige d'entrée que sur l'axe de la roue verticale
void Camera::ProcessMouseScroll(float yoffset)
{
    Zoom -= (float)yoffset;
    if (Zoom < 1.0f)
        Zoom = 1.0f;
    if (Zoom > 45.0f)
        Zoom = 45.0f;
}

// Calcule le vecteur front de la caméra à partir des angles d'Euler (mis à jour) de la caméra
void Camera::updateCameraVectors()
{
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    // Calculer le vecteur Right et Up à partir du vecteur Front et du vecteur WorldUp
    Right = glm::normalize(glm::cross(Front, WorldUp)); // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    Up = glm::normalize(glm::cross(Right, Front));
}