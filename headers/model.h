#pragma once

#include <vector>
#include <string>

#include "vectorops.h"
#include "tgaimage.h"

class Model {
private:
    std::vector<vector3Float> vertices;
    std::vector<vector2Float> uvs;
    std::vector<vector3Float> normals;

    std::vector<int> polygonalVerts;
    std::vector<int> polygonalTextures;
    std::vector<int> polygonalNormals;

    TGAImage diffusemap_;
    TGAImage normalmap_;
    TGAImage specularmap_;
public:
    Model(const std::string file);
    Model() {}

    int vertCount() const;
    int faceCount() const;

    vector3Float vertex(const int i) const;
    vector3Float vertex(const int face, const int vert) const;

    vector3Float normal(const int face, const int vert) const;
    vector3Float normal(const vector2Float& uv) const;

    vector2Float uv(const int face, const int vert) const;
    TGAColor diffuse(const vector2Float& uv) const;
    double specular(const vector2Float& uv) const;

    void LoadTexture(const std::string file, const std::string suf, TGAImage& img);

    Model& operator=(const Model& other) {
        if (this != &other) {
            this->vertices = other.vertices;
            this->uvs = other.uvs;
            this->normals = other.normals;
            this->polygonalVerts = other.polygonalVerts;
            this->polygonalNormals = other.polygonalNormals;
            this->polygonalTextures = other.polygonalTextures;
            this->diffusemap_ = other.diffusemap_;
            this->normalmap_ = other.normalmap_;
            this->specularmap_ = other.specularmap_;
        }
            
        return *this;
    };
};
