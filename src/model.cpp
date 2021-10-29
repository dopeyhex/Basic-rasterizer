#include <iostream>
#include <fstream>
#include <sstream>

#include "../headers/model.h"

int Model::vertCount() const {
    return vertices.size();
}

int Model::faceCount() const {
    return polygonalVerts.size() / 3;
}

vector3Float Model::vertex(const int i) const {
    return vertices[i];
}

vector3Float Model::vertex(const int face, const int vert) const {
    return vertices[polygonalVerts[face * 3 + vert]];
}

vector3Float Model::normal(const int face, const int vert) const {
    return normals[polygonalNormals[face * 3 + vert]];
}

vector3Float Model::normal(const vector2Float& f) const {
    TGAColor c = normalmap_.get(f[0] * normalmap_.get_width(), f[1] * normalmap_.get_height());
    vector3Float tmp;
    for (int i = 0; i < 3; i++) {
        tmp[2 - i] = c[i] / 255.0f * 2.0f - 1.0f;
    }
    return tmp;
}

TGAColor Model::diffuse(const vector2Float& f) const {
    return diffusemap_.get(f[0] * diffusemap_.get_width(), f[1] * diffusemap_.get_height());
}

vector2Float Model::uv(const int face, const int vert) const {
    return uvs[polygonalTextures[face * 3 + vert]];
}

double Model::specular(const vector2Float& f) const {
    return specularmap_.get(f[0] * specularmap_.get_width(), f[1] * specularmap_.get_height())[0] / 1.0f;
}

void Model::LoadTexture(std::string file, const std::string suf, TGAImage& img) {
    size_t dot = file.find_last_of(".");
    if (dot == std::string::npos)
        return;
    std::string texture = file.substr(0, dot) + suf;

    if (img.read_tga_file(texture.c_str())) {
        std::cout << texture << " loaded succesfully\n";
    } else {
        std::cout << texture << " was not found\n";
    }

    img.flip_vertically();
}

Model::Model(const std::string file) : vertices(),
                                       uvs(),
                                       normals(),
                                       polygonalVerts(),
                                       polygonalTextures(),
                                       polygonalNormals(),
                                       diffusemap_(),
                                       normalmap_(),
                                       specularmap_() {
    std::ifstream input;
    input.open(file, std::ifstream::in);
    if (input.fail()) {
        return;
    }
    std::string line;

    while (!input.eof()) {
        std::getline(input, line);
        std::istringstream stream(line.c_str());
        char c;

        if (line.compare(0, 2, "v ") == 0) {
            //geometric vertices
            stream >> c;
            vector3Float v;
            for (int i = 0; i < 3; i++)
                stream >> v[i];
            vertices.push_back(v);
        } else if (line.compare(0, 3, "vt ") == 0) {
            //texture coordinates
            stream >> c >> c;
            vector2Float uv;
            for (int i = 0; i < 2; i++)
                stream >> uv[i];
            uvs.push_back(uv);
        } else if (line.compare(0, 3, "vn ") == 0) {
            //vertex normals
            stream >> c >> c;
            vector3Float n;
            for (int i = 0; i < 3; i++)
                stream >> n[i];
            normals.push_back(n.normalize());
        } else if (line.compare(0, 2, "f ") == 0) {
            //polygonal faces
            stream >> c;
            int v, t, n;
            int counter = 0;
            while (stream >> v >> c >> t >> c >> n) {
                v--;
                t--;
                n--;

                polygonalVerts.push_back(v);
                polygonalTextures.push_back(t);
                polygonalNormals.push_back(n);
                counter++;
            }

            if (counter != 3) {
                std::cerr << "Triangulation error!\n";
                input.close();
                return;
            }
        }
    }

    input.close();
    std::cout << "Model loaded!\n";
    LoadTexture(file, "_diffuse.tga", diffusemap_);
    LoadTexture(file, "_tangent.tga", normalmap_);
    LoadTexture(file, "_specular.tga", specularmap_);
}
