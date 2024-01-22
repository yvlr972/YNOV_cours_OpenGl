#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "shader.hpp"
#include "constants.hpp"
#include "color.hpp"

// Vecteurs pour construire la matrice de vue de la caméra
glm::vec3 cameraPos = CAMERA_START_POSITION;
glm::vec3 cameraFront = CAMERA_START_FRONT;
glm::vec3 cameraUp = CAMERA_START_UP;

// Temps pour une itération de la boucle de rendu
float deltaTime = 0.0f;
// Temps du dernier appel de la boucle de rendu
float lastFrame = 0.0f;

// Variables pour la gestion de la souris
bool firstMouse = true;
float yaw = START_CAMERA_YAW;
float pitch = START_CAMERA_PITCH;
float lastX = WINDOW_WIDTH / 2.0;
float lastY = WINDOW_HEIGHT / 2.0;

// Champ de vision de la caméra
float fov = START_FOV;

// Fonction appelée lors du redimensionnement de la fenêtre
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// Fonction appelée lors de l'appui sur une touche du clavier
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Gestion des déplacements de la caméra avec les touches du clavier
    float cameraSpeed = CAMERA_SPEED * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

// Fonction appelée lors du déplacement de la souris
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
{
    // On convertit les coordonnées de la souris en float
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    // Si c'est la première fois qu'on appelle la fonction, on initialise les coordonnées de la souris
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    // On calcule le déplacement de la souris depuis le dernier appel de la fonction
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // Inversé car les coordonnées vont de bas en haut
    lastX = xpos;
    lastY = ypos;

    // On multiplie le déplacement par un facteur pour le rendre moins sensible
    float sensitivity = MOUSE_SENSITIVITY; // sensibilité du mouvement de la souris
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    // On met à jour les angles d'Euler de la caméra en fonction du déplacement de la souris
    yaw += xoffset;
    pitch += yoffset;

    // On limite l'angle de rotation de la caméra sur l'axe des X
    if (pitch > PITCH_LIMIT)
        pitch = PITCH_LIMIT;
    if (pitch < -PITCH_LIMIT)
        pitch = -PITCH_LIMIT;

    // On calcule le vecteur direction de la caméra
    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    // On met à jour le vecteur front de la caméra
    cameraFront = glm::normalize(direction);
    std::cout << "Camera Front - X: " << cameraFront.x << ", Y: " << cameraFront.y << ", Z: " << cameraFront.z << std::endl;
}

// Fonction appelée lors du défilement de la molette de la souris
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    // On limite le champ de vision de la caméra
    if (fov >= 1.0f && fov <= START_FOV)
        fov -= yoffset;
    if (fov <= 1.0f)
        fov = 1.0f;
    if (fov >= START_FOV)
        fov = START_FOV;
}

int main()
{
    // Initialisation de GLFW
    glfwInit();
    // On dit à GLFW qu'on veut utiliser OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // On dit à GLFW qu'on veut utiliser le core profile qui ne fournit que les fonctionnalités modernes d'OpenGL
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Création de la fenêtre GLFW
    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME, NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    // On rend le contexte de la fenêtre actif pour les appels OpenGL
    glfwMakeContextCurrent(window);

    // Initialisation de GLAD, ce qui configure les pointeurs de fonctions OpenGL pour qu'on puisse les utiliser
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // On dit à OpenGL la taille de la fenêtre pour le viewport
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    // On appelle la fonction framebuffer_size_callback à chaque fois que la fenêtre est redimensionnée
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    Shader shaderProgram(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);

    // On définit les vertices des triangles qui forment le cube
    float vertices[] = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f};

    // On crée un VA0 (Vertex Array Object) qui va contenir les paramètres de configuration des vertex buffer objects
    unsigned int VAO;
    // OpenGL va générer un identifiant pour le VAO et on va pouvoir s'y référer via la variable VAO
    glGenVertexArrays(1, &VAO);

    // On active le VAO pour que les prochains appels à glVertexAttribPointer et glEnableVertexAttribArray enregistrent les paramètres pour ce VAO
    glBindVertexArray(VAO);

    // On crée un VBO (Vertex Buffer Object) qui va contenir les coordonnées des vertices du triangle
    unsigned int VBO;
    // OpenGL va générer un identifiant pour le VBO et on va pouvoir s'y référer via la variable VBO
    glGenBuffers(1, &VBO);

    // On active le VBO pour y écrire les données des vertices du triangle
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // On copie les données des vertices dans le VBO activé
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // On configure la manière dont OpenGL doit interpréter les données du VBO activé, ici les coordonnées des vertices :
    // Ici les positions des vertices
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // Ici les coordonnées de texture des vertices
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int wallTexture, smileyTexture;

    // On précise que l'on veut inverser l'image verticalement lors du chargement des textures
    stbi_set_flip_vertically_on_load(true);

    // texture1
    glGenTextures(1, &wallTexture);
    glBindTexture(GL_TEXTURE_2D, wallTexture);
    // On définit les paramètres de la texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // On charge l'image de la texture et on la convertit en un format exploitable par OpenGL
    int width, height, nrChannels;
    unsigned char *data = stbi_load(TEXTURE_1_PATH, &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // texture2
    glGenTextures(1, &smileyTexture);
    glBindTexture(GL_TEXTURE_2D, smileyTexture);
    // On définit les paramètres de la texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // On charge l'image de la texture et on la convertit en un format exploitable par OpenGL
    data = stbi_load(TEXTURE_2_PATH, &width, &height, &nrChannels, 0);
    if (data)
    {
        // Notez que smiley.png a une transparence et donc un canal alpha, on doit donc dire à OpenGL que le type de données est de GL_RGBA
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // On active le shader program et on attribue leurs unités aux textures du shader
    shaderProgram.use();
    glUniform1i(glGetUniformLocation(shaderProgram.ID, "texture1"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram.ID, "texture2"), 1);

    glEnable(GL_DEPTH_TEST);

    // On définit les positions des cubes dans la scène
    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f, 2.0f, -2.5f),
        glm::vec3(1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f)};

    // On active le curseur et on le cache
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // On associe la fonction mouse_callback à l'évènement de déplacement de la souris
    glfwSetCursorPosCallback(window, mouse_callback);

    // Boucle de rendu
    while (!glfwWindowShouldClose(window))
    {
        // On calcule le temps écoulé depuis le dernier appel de la boucle de rendu
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // On traite un éventuel appui sur une touche (ici, ECHAP pour fermer la fenêtre)
        processInput(window);

        // On nettoie la couleur du buffer d'écran et on la remplit avec la couleur de fond
        glClearColor(CLEAR_COLOR.r, CLEAR_COLOR.g, CLEAR_COLOR.b, CLEAR_COLOR.a);
        // On nettoie le buffer de couleur et on le remplit avec la couleur précédemment définie
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // On utilise le shader program pour dessiner le triangle
        shaderProgram.use();

        // On associe nos textures aux unités de texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, wallTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, smileyTexture);

        // On calcule les matrices de transformation de la scène
        glm::mat4 view;
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection;
        // On prend en compte le FOV de la caméra pour la matrice de projection
        projection = glm::perspective(glm::radians(fov), WINDOW_WIDTH / WINDOW_HEIGHT, NEAR_CLIP_PLANE_DISTANCE, FAR_CLIP_PLANE_DISTANCE);

        // On récupère les identifiants des matrices dans le shader
        unsigned int viewID = glGetUniformLocation(shaderProgram.ID, "view");
        unsigned int projectionID = glGetUniformLocation(shaderProgram.ID, "projection");
        // On envoie les matrices au shader via leurs identifiants
        glUniformMatrix4fv(viewID, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionID, 1, GL_FALSE, glm::value_ptr(projection));

        // Pour chaque cube, on définit sa position, on lui applique une rotation et on le dessine
        for (unsigned int i = 0; i < 10; i++)
        {
            glm::mat4 model;
            model = glm::translate(model, cubePositions[i]);
            model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(1.0f, 0.3f, 0.5f));
            unsigned int modelID = glGetUniformLocation(shaderProgram.ID, "model");
            glUniformMatrix4fv(modelID, 1, GL_FALSE, glm::value_ptr(model));
            // On dessine les triangles définis par les indices
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // On échange les buffers de la fenêtre pour que ce qu'on vient de dessiner soit visible
        glfwSwapBuffers(window);
        // On regarde s'il y a des évènements (appui sur une touche, déplacement de la souris, etc.)
        glfwPollEvents();
    }

    // Quand la fenêtre est fermée, on libère les ressources
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    shaderProgram.deleteProgram();

    // On termine GLFW
    glfwTerminate();
    return 0;
}