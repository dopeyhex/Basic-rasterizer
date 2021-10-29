#pragma once

#include <vector>

#include "vectorops.h"
#include "matrixops.h"
#include "tgaimage.h"
#include "model.h"

struct ShaderBase {
	virtual vector4Float vertexCoord(size_t vertex, vector4Float coordsWorld, vector2Float uv, vector3Float normal) = 0;
	virtual bool isShaderPart(vector3Float bar, vector3Float &color, int samples) = 0;
};

struct ShadingParams {
	vector3Float ambient, diffuse, specular;
	ShadingParams(vector3Float ambient_ = vector3Float(),
                  vector3Float diffuse_ = vector3Float(),
                  vector3Float specular_ = vector3Float()) {
		ambient = ambient_;
		diffuse = diffuse_;
		specular = specular_;
	}
};

struct Shader : ShaderBase {
    Model modelShading;
    Matrix4x4 viewportProjected, lightpathProjected;
    vector3Float camPosition, lightPosition, tangent, biTangent;
    ShadingParams shadeParams;
    std::vector<float> shadowBuffer = {0.0f};
    size_t shadowBufferWidth = 0, shadowBufferHeight = 0;

    Matrix<4, 3, float> coordsScreenMatrix;
    Matrix<2, 3, float> verticesUVMatrix;
    Matrix3x3 verticesNormalMatrix;
    Matrix3x3 lightPositionMatrix;
    Matrix3x3 worldPositionMatrix;

    Shader() {}

    vector4Float vertexCoord(size_t vertex, vector4Float coordsWorld, vector2Float uv, vector3Float normal);
    bool isShaderPart(vector3Float barScreen, vector3Float& color, int samples);
};

struct DepthShader : ShaderBase {
    Matrix4x4 projectedView;
    Matrix<4, 3, float> coordsScreen;

    DepthShader() {}

    vector4Float vertexCoord(size_t vertex, vector4Float coordsWorld, vector2Float uv, vector3Float normal);
    bool isShaderPart(vector3Float bar, vector3Float& color, int samples);
};

void PhongShading(std::vector<Model>& models,
                  std::vector<Matrix4x4>& modelTransform,
                  size_t N,
                  std::vector<float>&zBuffer,
                  std::vector<vector3Float>& colorBuffer,
                  std::vector<float>& shadowBuffer,
                  Matrix4x4 lightVpPV, TGAImage& frame, size_t samples,
                  vector3Float lightPos, ShadingParams lightColor,
                  Matrix4x4 view, vector3Float pov,
                  std::vector<std::vector<float> >& MSAA);

Matrix4x4 ShadowMapping(std::vector<Model>& models,
                        std::vector<Matrix4x4>& modelTransform,
                        size_t N,
                        std::vector<float>& zBuffer,
                        std::vector<vector3Float>& colorBuffer,
                        TGAImage& frame, Matrix4x4 view,
                        std::vector<std::vector<float> >& SSAA);

struct Vertex {
	vector4Float coordsWorld, coordsClip;
    vector3Float normal;
	vector2Float uv;

	Vertex(vector4Float world = vector4Float(),
           vector4Float clip = vector4Float(),
           vector2Float uv = vector2Float(),
           vector3Float normal = vector3Float()) : coordsWorld(world),
                                                   coordsClip(clip),
                                                   normal(normal),
                                                   uv(uv) {}
};

void rasterize(Matrix<3, 4, float> coordsScreen,
               ShaderBase& shader,
               std::vector<float>& zBuffer,
               std::vector<vector3Float>& colorBuffer,
               size_t width, size_t height, size_t samples,
               std::vector<std::vector<float> >& v);
void axisClip(const std::vector<Vertex> &source, std::vector<Vertex> &result, size_t axis);
