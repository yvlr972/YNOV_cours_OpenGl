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
#include "camera.hpp"

// Caméra
Camera camera(CAMERA_START_POSITION);

// Temps pour une itération de la boucle de rendu
float deltaTime = 0.0f;
// Temps du dernier appel de la boucle de rendu
float lastFrame = 0.0f;

// Variables pour la gestion de la souris
bool firstMouse = true;
float lastX = WINDOW_WIDTH / 2.0;
float lastY = WINDOW_HEIGHT / 2.0;

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
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.processKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.processKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.processKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.processKeyboard(RIGHT, deltaTime);
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

    camera.processMouseMovement(xoffset, yoffset);
}

// Fonction appelée lors du défilement de la molette de la souris
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.processMouseScroll(static_cast<float>(yoffset));
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

    Shader objectShader(OBJECT_VERTEX_SHADER_PATH, OBJECT_FRAGMENT_SHADER_PATH);
    Shader lightSourceShader(LIGHT_VERTEX_SHADER_PATH, LIGHT_FRAGMENT_SHADER_PATH);

    float vertices[] = {
        0.5f, -0.5f, -0.5f,
        0.5f, 0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        0.5f, 0.5f, -0.5f,
        -0.5f, 0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f, 0.5f,
        0.5f, -0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f,

        -0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,

        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,

        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, 0.5f,
        0.5f, -0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, 0.5f, -0.5f,
        0.5f, 0.5f, -0.5f,
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, -0.5f};

    // Création des VBO et VAO
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // On crée un VAO pour le cube qui va réfléchir la lumière
    unsigned int objectVAO;
    glGenVertexArrays(1, &objectVAO);
    glBindVertexArray(objectVAO);

    // Rappel : le VAO récupère la référence au VBO à cette étape
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // On crée un VAO pour le cube qui va émettre la lumière, on utilise le même VBO
    unsigned int lightSourceVAO;
    glGenVertexArrays(1, &lightSourceVAO);
    glBindVertexArray(lightSourceVAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // On active le test de profondeur
    glEnable(GL_DEPTH_TEST);

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

        // Rendu du cube qui va réfléchir la lumière

        // On utilise le shader program du cube qui va réfléchir la lumière
        objectShader.use();
        // On envoie les valeurs des couleurs de l'objet et de la lumière au shader via les uniform
        glUniform3f(glGetUniformLocation(objectShader.ID, "objectColor"), 1.0f, 0.5f, 0.31f);
        glUniform3f(glGetUniformLocation(objectShader.ID, "lightColor"), 1.0f, 1.0f, 1.0f);

        // Matrice de modèle du cube qui va réfléchir la lumière (position 0, 0, 0)
        glm::mat4 model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(objectShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

        // On calcule les matrices de transformation de la scène
        glm::mat4 view = camera.getViewMatrix();
        glUniformMatrix4fv(glGetUniformLocation(objectShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));

        // On prend en compte le FOV de la caméra pour la matrice de projection
        glm::mat4 projection = glm::perspective(glm::radians(camera.getZoom()), WINDOW_WIDTH / WINDOW_HEIGHT, NEAR_CLIP_PLANE_DISTANCE, FAR_CLIP_PLANE_DISTANCE);
        glUniformMatrix4fv(glGetUniformLocation(objectShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        // On dessine le cube qui va réfléchir la lumière en utilisant le VAO qui lui est associé
        glBindVertexArray(objectVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Rendu du cube source de lumière

        lightSourceShader.use();

        // Matrice de modèle du cube source de lumière (position 1.2f, 1.0f, 2.0f)
        model = glm::mat4(1.0f);
        model = glm::translate(model, LIGHT_SOURCE_POSITION);
        // On rend le cube plus petit
        model = glm::scale(model, glm::vec3(0.2f));
        glUniformMatrix4fv(glGetUniformLocation(lightSourceShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

        // On utilise les mêmes matrices de vue et de projection que pour le cube qui va réfléchir la lumière
        glUniformMatrix4fv(glGetUniformLocation(lightSourceShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(lightSourceShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        // On dessine le cube source de lumière en utilisant le VAO qui lui est associé
        glBindVertexArray(lightSourceVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // On échange les buffers de la fenêtre pour que ce qu'on vient de dessiner soit visible
        glfwSwapBuffers(window);
        // On regarde s'il y a des évènements (appui sur une touche, déplacement de la souris, etc.)
        glfwPollEvents();
    }

    // Quand la fenêtre est fermée, on libère les ressources
    glDeleteVertexArrays(1, &objectVAO);
    glDeleteVertexArrays(1, &lightSourceVAO);
    glDeleteBuffers(1, &VBO);
    objectShader.deleteProgram();
    lightSourceShader.deleteProgram();

    // On termine GLFW
    glfwTerminate();
    return 0;
}