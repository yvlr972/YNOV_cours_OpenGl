#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <memory>
#include <regex>
#include <algorithm>

#include "shader.hpp"
#include "constants.hpp"
#include "color.hpp"
#include "camera.hpp"
#include "gameObject.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Caméra
Camera camera(CAMERA_START_POSITION);

// Shader pour les objets, initialisé plus tard dans le main()
Shader objectShader;

// Temps pour une itération de la boucle de rendu
float deltaTime = 0.0f;
// Temps du dernier appel de la boucle de rendu
float lastFrame = 0.0f;

// Variables pour la gestion de la souris
bool firstMouse = true;
float lastX;
float lastY;

std::vector<std::unique_ptr<GameObject>> gameObjects;

// Tableau de positions des pointLight
std::vector<glm::vec3> pointLightPositions;

// Tableau des vertices pour LightCubes
std::vector<float> lightCubesVertices;

// Variables pour la gestion du clavier
bool graveAccentKeyPressed = false;

// Variables pour faire apparaître la souris
bool mouseHidden = true;

// Définition du motif regex pour les instructions de création d'un gameObject
std::regex gameObjectCreationPattern(R"(^(\S+)\s+(\S+)\s+(0|1)$)");

// Définition des motifs regex pour les instructions de transformation d'un gameObject
std::regex translatePattern(R"(t\s+([-\d\.]+)\s+([-\d\.]+)\s+([-\d\.]+))");
std::regex rotatePattern(R"(r\s+([-\d\.]+)\s+([-\d\.]+)\s+([-\d\.]+)\s+([-\d\.]+))");
std::regex scalePattern(R"(s\s+([-\d\.]+)\s+([-\d\.]+)\s+([-\d\.]+))");

// Fonction appelée lors du redimensionnement de la fenêtre
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void applyTransformations(const std::string &input)
{
    // Extraire le nom du GameObject au début de l'entrée
    size_t firstSpace = input.find(' ');
    if (firstSpace == std::string::npos)
    {
        std::cout << "Format d'entree invalide." << std::endl;
        return;
    }
    std::string gameObjectName = input.substr(0, firstSpace);
    std::string transformations = input.substr(firstSpace + 1);

    // Trouver le GameObject par son nom
    auto it = std::find_if(gameObjects.begin(), gameObjects.end(), [&gameObjectName](const std::unique_ptr<GameObject> &obj)
                           { return obj->getName() == gameObjectName; });

    if (it == gameObjects.end())
    {
        std::cout << "GameObject '" << gameObjectName << "' non trouve." << std::endl;
        return;
    }
    auto &gameObject = *it; // Référence au GameObject trouvé

    bool transformationApplied = false; // Pour vérifier si une transformation a été appliquée

    // Fonction pour appliquer les transformations
    auto apply = [&](const std::regex &pattern, const std::string &transformationInput, auto transformFunc)
    {
        std::smatch matches;
        std::string::const_iterator searchStart = transformationInput.cbegin();
        while (std::regex_search(searchStart, transformationInput.cend(), matches, pattern))
        {
            transformationApplied = true;
            transformFunc(matches);
            searchStart = matches.suffix().first; // Continue à chercher après la dernière correspondance
        }
    };

    // Application des translations
    apply(translatePattern, transformations, [&](const std::smatch &matches)
          {
        float x = std::stof(matches[1].str()), y = std::stof(matches[2].str()), z = std::stof(matches[3].str());
        gameObject->modelMatrixTranslate(glm::vec3(x, y, z));
        std::cout << "Translation appliquee: x=" << x << ", y=" << y << ", z=" << z << std::endl; });

    // Application des rotations
    apply(rotatePattern, transformations, [&](const std::smatch &matches)
          {
        float angle = std::stof(matches[1].str()), x = std::stof(matches[2].str()), y = std::stof(matches[3].str()), z = std::stof(matches[4].str());
        gameObject->modelMatrixRotate(angle, glm::vec3(x, y, z));
        std::cout << "Rotation appliquee: angle=" << angle << ", x=" << x << ", y=" << y << ", z=" << z << std::endl; });

    // Application des mises à l'échelle
    apply(scalePattern, transformations, [&](const std::smatch &matches)
          {
        float x = std::stof(matches[1].str()), y = std::stof(matches[2].str()), z = std::stof(matches[3].str());
        gameObject->modelMatrixScale(glm::vec3(x, y, z));
        std::cout << "Mise a l'echelle appliquee: x=" << x << ", y=" << y << ", z=" << z << std::endl; });

    if (!transformationApplied)
    {
        std::cout << "Aucune transformation appliquee." << std::endl;
    }
}

// Fonction pour charger les gameObject à partir d'un fichier .txt
void loadGameObjects(const char* filePath)
{
    std::ifstream fichier(filePath);

    if (fichier)
    {
        std::string ligne;
        while (std::getline(fichier, ligne))
        {
            std::istringstream iss(ligne);
            std::smatch matches;
            if (std::regex_search(ligne, matches, gameObjectCreationPattern))
            {
                std::string gameObjectName = matches[1];
                std::string objectPath = matches[2];
                bool flipTextureVertically = matches[3] == "1";

                gameObjects.push_back(std::make_unique<GameObject>(gameObjectName, objectPath, flipTextureVertically, objectShader, gameObjects));
            }
            else
            {
                std::cout << "Format gameObject invalide." << std::endl;
            }
        }
        fichier.close();
    }
    else {
        std::cerr << "Impossible d'ouvrir le fichier.\n";
    }
}

// Fonction pour sauvegarder les gameObject dans un fichier .txt
void saveGameObject(std::string gameObjectName, std::string gameObjectPath, bool flipTextureVertically, const char* filePath)
{
    std::ofstream fichier(filePath, std::ios::app); // Ouvrir en mode append

    if (fichier)
    {
        fichier << gameObjectName << " "
                << gameObjectPath << " "
                << flipTextureVertically << "\n";
        fichier.close();
    }
    else
    {
        std::cerr << "Impossible d'ouvrir le fichier.\n";
    }
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
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
        camera.processKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
        camera.processKeyboard(DOWN, deltaTime);


    // Montrer/Masquer curseur souris
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        if(mouseHidden)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            mouseHidden = false;
        }
        else
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            mouseHidden = true;
        }
    }

    // Augmenter/Diminuer intensité des cubes de lumière
    // if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    // if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)


    // Menu "²"
    if (glfwGetKey(window, GLFW_KEY_GRAVE_ACCENT) == GLFW_PRESS && !graveAccentKeyPressed)
    {
        graveAccentKeyPressed = true;
        // Menu principal
        std::cout << "Menu principal :"
                  << std::endl
                  << "Entrez 1 pour creer un nouveau GameObject."
                  << std::endl
                  << "Entrez 2 pour appliquer une transformation a un GameObject existant."
                  << std::endl;

        std::string choice;
        std::getline(std::cin, choice);

        if (choice == "1" || choice == "2")
        {
            // On convertit le choix en entier
            int choiceInt = std::stoi(choice);

            // Création d'un GameObject
            if (choiceInt == 1)
            {
                std::cout << "Pour creer un nouveau GameObject :"
                          << std::endl
                          << "nomDuGameObject path/vers/mon/modele.obj 1 pour inverser verticalement les texture ou 0 pour ne pas les inverser :"
                          << std::endl;

                std::string userInput;
                std::getline(std::cin, userInput);
                std::smatch matches;
                if (std::regex_search(userInput, matches, gameObjectCreationPattern))
                {
                    std::string gameObjectName = matches[1];
                    std::string objectPath = matches[2];
                    bool flipTextureVertically = matches[3] == "1";

                    std::cout << "GameObject '" << gameObjectName << "' cree.\n"
                              << "Path: " << objectPath << "\n"
                              << "Inverser verticalement les textures: " << std::boolalpha << flipTextureVertically << std::endl;

                    gameObjects.push_back(std::make_unique<GameObject>(gameObjectName, objectPath, flipTextureVertically, objectShader, gameObjects));

                    // On sauvegarde le gameObject dans le fichier GameObjectList.txt
                    saveGameObject(gameObjectName, objectPath, flipTextureVertically, GAMEOBJECT_LIST_PATH);
                }
                else
                {
                    std::cout << "Format d'entree invalide." << std::endl;
                }
            }
            // Modification de GameObject
            else if (choiceInt == 2)
            {
                std::cout << "Pour modifier un gameObject :"
                          << std::endl
                          << "nomDuGameObject t valeurX valeurY valeurZ r valeurAngle valeurAxeX valeurAxeY valeurAxeZ s valeurX valeurY valeurZ :"
                          << std::endl;
                string userInput;
                std::getline(std::cin, userInput);
                applyTransformations(userInput);
            }
        }
        else
        {
            std::cout << "Entree invalide." << std::endl;
        }
    }
    else if (glfwGetKey(window, GLFW_KEY_GRAVE_ACCENT) == GLFW_RELEASE)
    {
        graveAccentKeyPressed = false;
    }
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

// Fonction pour charger les positions de pointLight à partir d'un fichier .txt
void loadPointLightsPositions(std::vector<glm::vec3>& vecPositions, const char* filePath)
{
    std::ifstream fichier(filePath);

    if (fichier) {
        std::string ligne;
        while (std::getline(fichier, ligne)) {
            std::istringstream iss(ligne);
            float x, y, z;
            if (iss >> x >> y >> z) {
                vecPositions.push_back(glm::vec3(x, y, z));
            }
        }
        fichier.close();
    } else {
        std::cerr << "Impossible d'ouvrir le fichier.\n";
    }
}

// Fonction pour charger les vertices de lightCube à partir d'un fichier .txt
void loadLightCubesVertices(std::vector<float>& vecVertices, const char* filePath)
{
    std::ifstream fichier(filePath);

    if (fichier) {
        std::string ligne;
        while (std::getline(fichier, ligne)) {
            std::istringstream iss(ligne);
            float vertex;
            while (iss >> vertex) {
                vecVertices.push_back(vertex);
            }
        }
        fichier.close();
    } else {
        std::cerr << "Impossible d'ouvrir le fichier.\n";
    }
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

    objectShader = Shader(OBJECT_VERTEX_SHADER_PATH, OBJECT_FRAGMENT_SHADER_PATH);
    Shader lightSourceShader(LIGHT_VERTEX_SHADER_PATH, LIGHT_FRAGMENT_SHADER_PATH);

    // Charge les positions des point lights à partir du fhichier CubeVertices.txt
    loadLightCubesVertices(lightCubesVertices, CUBE_VERTICES_PATH);

    // On active le test de profondeur
    glEnable(GL_DEPTH_TEST);

    // On active le curseur et on le cache
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // On associe la fonction mouse_callback à l'évènement de déplacement de la souris
    glfwSetCursorPosCallback(window, mouse_callback);
    // On associe la fonction scroll_callback à l'évènement de roulement de la mollette
    glfwSetScrollCallback(window, scroll_callback);

    // Création des VBO et VAO
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, lightCubesVertices.size() * sizeof(float), &lightCubesVertices[0], GL_STATIC_DRAW);

    // On crée un VAO pour le cube qui va émettre la lumière, on utilise le même VBO
    unsigned int lightSourceVAO;
    glGenVertexArrays(1, &lightSourceVAO);
    glBindVertexArray(lightSourceVAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // Charge les positions des point lights à partir du fhichier PointLightsPositions.txt
    loadPointLightsPositions(pointLightPositions, POINT_LIGHT_PATH);

    // Charge les gameObjects à partir du fichier GameObjectList.txt
    loadGameObjects(GAMEOBJECT_LIST_PATH);

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

        // Rendu de l'objet qui va réfléchir la lumière

        // On utilise le shader program de l'objet qui va réfléchir la lumière
        objectShader.use();
        // On envoie les valeurs des couleurs de l'objet et de la lumière au shader via les uniform
        glUniform3f(glGetUniformLocation(objectShader.ID, "material.ambient"), 0.1f, 0.1f, 0.1f);
        glUniform1f(glGetUniformLocation(objectShader.ID, "material.shininess"), 32.0f);

        // Uniforms de la lumière directionnelle
        glUniform3f(glGetUniformLocation(objectShader.ID, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
        glUniform3f(glGetUniformLocation(objectShader.ID, "dirLight.ambient"), 0.05f, 0.05f, 0.05f);
        glUniform3f(glGetUniformLocation(objectShader.ID, "dirLight.diffuse"), 0.4f, 0.4f, 0.4f);
        glUniform3f(glGetUniformLocation(objectShader.ID, "dirLight.specular"), 0.5f, 0.5f, 0.5f);

        // Point light 0
        glUniform3f(glGetUniformLocation(objectShader.ID, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
        glUniform3f(glGetUniformLocation(objectShader.ID, "pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);
        glUniform3f(glGetUniformLocation(objectShader.ID, "pointLights[0].diffuse"), 0.8f, 0.8f, 0.8f);
        glUniform3f(glGetUniformLocation(objectShader.ID, "pointLights[0].specular"), 1.0f, 1.0f, 1.0f);
        glUniform1f(glGetUniformLocation(objectShader.ID, "pointLights[0].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(objectShader.ID, "pointLights[0].linear"), 0.09f);
        glUniform1f(glGetUniformLocation(objectShader.ID, "pointLights[0].quadratic"), 0.032f);

        // Point light 1
        glUniform3f(glGetUniformLocation(objectShader.ID, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
        glUniform3f(glGetUniformLocation(objectShader.ID, "pointLights[1].ambient"), 0.05f, 0.05f, 0.05f);
        glUniform3f(glGetUniformLocation(objectShader.ID, "pointLights[1].diffuse"), 0.8f, 0.8f, 0.8f);
        glUniform3f(glGetUniformLocation(objectShader.ID, "pointLights[1].specular"), 1.0f, 1.0f, 1.0f);
        glUniform1f(glGetUniformLocation(objectShader.ID, "pointLights[1].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(objectShader.ID, "pointLights[1].linear"), 0.09f);
        glUniform1f(glGetUniformLocation(objectShader.ID, "pointLights[1].quadratic"), 0.032f);

        // Point light 2
        glUniform3f(glGetUniformLocation(objectShader.ID, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
        glUniform3f(glGetUniformLocation(objectShader.ID, "pointLights[2].ambient"), 0.05f, 0.05f, 0.05f);
        glUniform3f(glGetUniformLocation(objectShader.ID, "pointLights[2].diffuse"), 0.8f, 0.8f, 0.8f);
        glUniform3f(glGetUniformLocation(objectShader.ID, "pointLights[2].specular"), 1.0f, 1.0f, 1.0f);
        glUniform1f(glGetUniformLocation(objectShader.ID, "pointLights[2].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(objectShader.ID, "pointLights[2].linear"), 0.09f);
        glUniform1f(glGetUniformLocation(objectShader.ID, "pointLights[2].quadratic"), 0.032f);

        // Point light 3
        glUniform3f(glGetUniformLocation(objectShader.ID, "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
        glUniform3f(glGetUniformLocation(objectShader.ID, "pointLights[3].ambient"), 0.05f, 0.05f, 0.05f);
        glUniform3f(glGetUniformLocation(objectShader.ID, "pointLights[3].diffuse"), 0.8f, 0.8f, 0.8f);
        glUniform3f(glGetUniformLocation(objectShader.ID, "pointLights[3].specular"), 1.0f, 1.0f, 1.0f);
        glUniform1f(glGetUniformLocation(objectShader.ID, "pointLights[3].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(objectShader.ID, "pointLights[3].linear"), 0.09f);
        glUniform1f(glGetUniformLocation(objectShader.ID, "pointLights[3].quadratic"), 0.032f);

        ///* TODO */
        /// Point lights
        /// A voir comment faire fonctionner le code dans cette boucle
        /// les gameObjects créés apparissent noirs
        /*for (unsigned int i = 1; i < pointLightPositions.size(); i++)
        {
            glUniform3f(glGetUniformLocation(objectShader.ID, "pointLights[i].position"), pointLightPositions[i].x, pointLightPositions[i].y, pointLightPositions[i].z);
            glUniform3f(glGetUniformLocation(objectShader.ID, "pointLights[i].ambient"), 0.05f, 0.05f, 0.05f);
            glUniform3f(glGetUniformLocation(objectShader.ID, "pointLights[i].diffuse"), 0.8f, 0.8f, 0.8f);
            glUniform3f(glGetUniformLocation(objectShader.ID, "pointLights[i].specular"), 1.0f, 1.0f, 1.0f);
            glUniform1f(glGetUniformLocation(objectShader.ID, "pointLights[i].constant"), 1.0f);
            glUniform1f(glGetUniformLocation(objectShader.ID, "pointLights[i].linear"), 0.09f);
            glUniform1f(glGetUniformLocation(objectShader.ID, "pointLights[i].quadratic"), 0.032f);
        }*/

        // SpotLight
        glUniform3f(glGetUniformLocation(objectShader.ID, "spotLight.position"), camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);
        glUniform3f(glGetUniformLocation(objectShader.ID, "spotLight.direction"), camera.getFront().x, camera.getFront().y, camera.getFront().z);
        glUniform3f(glGetUniformLocation(objectShader.ID, "spotLight.ambient"), 0.0f, 0.0f, 0.0f);
        glUniform3f(glGetUniformLocation(objectShader.ID, "spotLight.diffuse"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(objectShader.ID, "spotLight.specular"), 1.0f, 1.0f, 1.0f);
        glUniform1f(glGetUniformLocation(objectShader.ID, "spotLight.constant"), 1.0f);
        glUniform1f(glGetUniformLocation(objectShader.ID, "spotLight.linear"), 0.09f);
        glUniform1f(glGetUniformLocation(objectShader.ID, "spotLight.quadratic"), 0.032f);
        glUniform1f(glGetUniformLocation(objectShader.ID, "spotLight.cosCutOff"), glm::cos(glm::radians(12.5f)));
        glUniform1f(glGetUniformLocation(objectShader.ID, "spotLight.cosOuterCutOff"), glm::cos(glm::radians(15.0f)));

        glUniform3f(glGetUniformLocation(objectShader.ID, "viewPos"), camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);

        // On calcule les matrices de transformation de la scène
        glm::mat4 view = camera.getViewMatrix();
        glUniformMatrix4fv(glGetUniformLocation(objectShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));

        // On prend en compte le FOV de la caméra pour la matrice de projection
        glm::mat4 projection = glm::perspective(glm::radians(camera.getZoom()), WINDOW_WIDTH / WINDOW_HEIGHT, NEAR_CLIP_PLANE_DISTANCE, FAR_CLIP_PLANE_DISTANCE);
        glUniformMatrix4fv(glGetUniformLocation(objectShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        for (auto &gameObject : gameObjects)
        {
            gameObject->Draw();
        }

        // Rendu des cubes source de lumière

        lightSourceShader.use();
        // On utilise les mêmes matrices de vue et de projection que pour le cube qui va réfléchir la lumière
        glUniformMatrix4fv(glGetUniformLocation(lightSourceShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(lightSourceShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        // Matrice de modèle des sources de lumière
        glm::mat4 model = glm::mat4(1.0f);

        // On dessine les cubes source de lumière en utilisant le VAO qui leur est associé
        glBindVertexArray(lightSourceVAO);
        for (unsigned int i = 0; i < pointLightPositions.size(); i++)
        {
            glUniform3f(glGetUniformLocation(lightSourceShader.ID, "lightColor"), 0.0f, 0.0f, 1.0f);
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            // model = glm::scale(model, glm::vec3(0.2f));
            glUniformMatrix4fv(glGetUniformLocation(lightSourceShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // On échange les buffers de la fenêtre pour que ce qu'on vient de dessiner soit visible
        glfwSwapBuffers(window);
        // On regarde s'il y a des évènements (appui sur une touche, déplacement de la souris, etc.)
        glfwPollEvents();
    }

    // Quand la fenêtre est fermée, on libère les ressources
    glDeleteVertexArrays(1, &lightSourceVAO);
    glDeleteBuffers(1, &VBO);
    gameObjects.clear();
    objectShader.deleteProgram();
    lightSourceShader.deleteProgram();

    // On termine GLFW
    glfwTerminate();
    return 0;
}