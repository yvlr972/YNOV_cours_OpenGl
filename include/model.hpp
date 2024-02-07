#ifndef MODEL_HPP
#define MODEL_HPP

#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>

#include "shader.hpp"
#include "mesh.hpp"


class Model
{
    public:
        Model(char *path)
        {
            loadModel(path);
        }
        void Draw(Shader &shader);
    private:
        // Données du modèle
        // 8
        vector<Texture> textures_loaded; // On stocke toutes les textures chargées pour vérifier par la suite si elles sont déjà chargées

        vector<Mesh> meshes;
        string directory;

        void loadModel(string path);
        void processNode(aiNode *node, const aiScene *scene);
        Mesh processMesh(aiMesh *mesh, const aiScene *scene);
        vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, 
                                             string typeName);
};

#endif