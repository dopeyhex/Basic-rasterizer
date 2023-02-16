#define _USE_MATH_DEFINES

#include <algorithm>
#include <cmath>

#include "../headers/matrixops.h"
#include "../headers/renderer.h"

void rasterize(Matrix<3, 4, float> coordsScreen,
               ShaderBase& shader,
               std::vector<float>& zBuffer,
               std::vector<vector3Float>& colorBuffer,
               size_t width, size_t height, size_t samples,
               std::vector<std::vector<float> >& AA) {
    //bounding boxes
    vector2Int bbmin(int(width - 1), int(height - 1));
    vector2Int bbmax(0, 0);

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 3; j++) {
            bbmin[i] = std::max(0, std::min(bbmin[i], int(coordsScreen[j][i])));
            bbmax[i] = std::min(int(height - 1), std::max(bbmax[i], int(coordsScreen[j][i])));
        }
    }

    vector2Float A = project<2>(coordsScreen[0]),
                 B = project<2>(coordsScreen[1]),
                 C = project<2>(coordsScreen[2]);

#pragma omp parallel for
        for (int x = bbmin.x; x <= bbmax.x; x++) {
            for (int y = bbmin.y; y <= bbmax.y; y++) {
                vector3Float barMidScreen, color;
                bool isProcessed = false;

                for (int i = 0; i < samples; i++) {
                    vector3Float barScreen = Barycentric(A, B, C, vector2Float(x + AA[i][0], y + AA[i][1]));

                    float w = coordsScreen[0].w * barScreen.x + coordsScreen[1].w * barScreen.y + coordsScreen[2].w * barScreen.z;
                    float z = (coordsScreen[0].z * barScreen.x + coordsScreen[1].z * barScreen.y + coordsScreen[2].z * barScreen.z) * (1.0f / w);
                    size_t k = samples * (y * width + x) + i;

                    if (barScreen.x < 0 || barScreen.y < 0 || barScreen.z < 0 || z < zBuffer[k]) continue;

                    if (!isProcessed) {
                        barMidScreen = Barycentric(A, B, C, vector2Float(x + 0.5f, y + 0.5f));
                        bool discard = shader.isShaderPart(barMidScreen, color, samples);
                        if (!discard) break;
                        isProcessed = true;
                    }

                    colorBuffer[k] = color;
                    zBuffer[k] = z;
                }
            }
        }
}

void axisClip(const std::vector<Vertex>& source, std::vector<Vertex>& result, size_t axis) {
    for (size_t i = 0; i < source.size(); i++) {
        Vertex current = source[i],
               next = source[(i + 1) % source.size()];
        float clipCur = current.coordsClip[axis] / current.coordsClip.w,
              clipNext = next.coordsClip[axis] / next.coordsClip.w;

        if (clipCur <= 1.0f) {
            result.push_back(current);
        }

        if ((clipCur > 1.0f && clipNext < 1.0f) || (clipCur < 1.0f && clipNext > 1.0f)) {
            float t = (current.coordsClip[axis] - 1.0f / current.coordsClip.w) / 
                      (1.0f / next.coordsClip.w - 1.0f / current.coordsClip.w - next.coordsClip[axis] + current.coordsClip[axis]);
            float w = 1.0 / (1.0f / current.coordsClip.w + t * (1.0f / next.coordsClip.w - 1.0f / current.coordsClip.w));

            Vertex added;
            added.coordsClip = current.coordsClip + (next.coordsClip - current.coordsClip) * t;
            added.coordsClip[axis] = -added.coordsClip[axis];
            added.coordsWorld = (current.coordsWorld + (next.coordsWorld - current.coordsWorld) * t) * w;
            added.normal = (current.normal + (next.normal * current.normal) * t) * w;
            added.uv = (current.uv + (next.uv - current.uv) * t) * w;

            result.push_back(added);
        }
    }
}

//shaders

vector4Float Shader::vertexCoord(size_t vertex, vector4Float coordsWorld, vector2Float uv, vector3Float normal) {
    vector4Float coordsScreen = viewportProjected * coordsWorld;
    float w = coordsScreen.w;

    coordsScreen = coordsScreen / w;
    coordsScreen.z = coordsScreen.z / w;
    coordsScreen.w = 1.0f / w;

    vector4Float coordsLight = lightpathProjected * coordsWorld;
    coordsLight = coordsLight / coordsLight.w;

    worldPositionMatrix.setCol(vertex, project<3>(coordsWorld) / w);
    lightPositionMatrix.setCol(vertex, project<3>(coordsLight) / w);
    coordsScreenMatrix.setCol(vertex, coordsScreen);
    verticesUVMatrix.setCol(vertex, uv / w);
    verticesNormalMatrix.setCol(vertex, normal / w);

    return coordsScreen;
}

//whoa, thx internet, very cool!
float fastPow(float a, float b) {
    union {
        float d;
        int x[2];
    } u = {a};
    u.x[1] = (int)(b * (u.x[1] - 1064866805) + 1064866805);
    u.x[0] = 0;
    return u.d;
}

bool Shader::isShaderPart(vector3Float barScreen, vector3Float& color, int samples) {
    float w = (coordsScreenMatrix * barScreen).w;
    if (fabs(w) < 1e-5) {
        return false;
    } else {
        w = 1.0f / w;
        vector2Float uv = verticesUVMatrix * barScreen * w;

        Matrix3x3 coords;
        coords.setCol(0, tangent);
        coords.setCol(1, biTangent);
        coords.setCol(2, verticesNormalMatrix * barScreen * w);

        vector3Float ambient = shadeParams.ambient * (modelShading.diffuse(uv).rgb());
        vector3Float normal = (coords * modelShading.normal(uv)).normalize();

        vector3Float lightDir = lightPosition.normalize();
        vector3Float povDir = (camPosition - worldPositionMatrix * barScreen * w).normalize();
        vector3Float midDir = (lightDir + povDir) / 2.0f;

        vector3Float diffuse = shadeParams.diffuse * ((modelShading.diffuse(uv).rgb()) * std::max(0.0f, dot(normal, lightDir)));
        vector3Float specular = shadeParams.specular * ((modelShading.specular(uv)) * fastPow(std::max(0.0f, dot(normal, midDir)), 20.0f));

        float darkness = 0.0f;
        vector3Float lightPos = lightPositionMatrix * barScreen * w;

        int sampleCounter = 0;
        for (int dx = -samples; dx < samples; dx++) {
            int xShadow = lightPos.x + dx;
            if (xShadow < 0 || xShadow >= shadowBufferWidth) continue;

            for (int dy = -samples; dy < samples; dy++) {
                int yShadow = lightPos.y + dy;
                if (yShadow < 0 || yShadow >= shadowBufferHeight) continue;

                sampleCounter++;
                if (lightPos.z + 0.005f < shadowBuffer[yShadow * shadowBufferWidth + xShadow]) {
                    darkness += 1.0f;
                }
            }
        }

        darkness /= sampleCounter;
        for (int i = 0; i < 3; i++) {
            color[i] = std::min(ambient[i] + (diffuse[i] + specular[i]) * (1.0f - darkness), 255.0f);
        }

        return true;
    }
}

vector4Float DepthShader::vertexCoord(size_t vertex, vector4Float coordsWorld, vector2Float uv, vector3Float normal) {
    vector4Float coordsVertex = projectedView * coordsWorld;
    coordsVertex = coordsVertex / coordsVertex.w;
    coordsScreen.setCol(vertex, coordsVertex);
    return coordsVertex;
}

bool DepthShader::isShaderPart(vector3Float barScreen, vector3Float& color, int samples) {
    vector4Float fragPos = coordsScreen * barScreen;
    color = vector3Float(255.0f, 255.0f, 255.0f) * expf((fragPos[2] - 1.0f) * 4.0f);
    return true;
}

void PhongShading(std::vector<Model>& models,
                  std::vector<Matrix4x4>& modelTransform,
                  size_t N,
                  std::vector<float>& zBuffer,
                  std::vector<vector3Float>& colorBuffer,
                  std::vector<float>& shadowBuffer,
                  Matrix4x4 lightVpPV, TGAImage& frame, size_t samples,
                  vector3Float lightPos, ShadingParams lightColor,
                  Matrix4x4 view, vector3Float pov,
                  std::vector<std::vector<float> >& MSAA) {
    Matrix4x4 projection = Projection(M_PI / 4.0f, 1.0f, -0.01f, -10.0f);
    Matrix4x4 viewport = Viewport(frame.get_width(), frame.get_height());
    Matrix4x4 projectedView = projection * view;
    Shader PhongShader;

    for (size_t m = 0; m < N; m++) {
        PhongShader.modelShading = models[m];
        PhongShader.viewportProjected = viewport * projection * view;
        PhongShader.lightpathProjected = lightVpPV;
        PhongShader.camPosition = pov;
        PhongShader.lightPosition = lightPos;
        PhongShader.shadeParams = lightColor;
        PhongShader.shadowBuffer = shadowBuffer;
        PhongShader.shadowBufferWidth = frame.get_width();
        PhongShader.shadowBufferHeight = frame.get_height();

        for (int i = 0; i < models[m].faceCount(); i++) {
            //backface culling
            vector3Float normal = cross(models[m].vertex(i, 1) - models[m].vertex(i, 0), 
                                        models[m].vertex(i, 2) - models[m].vertex(i, 0)).normalize();
            normal = project<3>((view * modelTransform[m]).invertT() * vector4Float(normal, 0.0f));
            if (normal.z <= 0.0f) continue;

            //z-clipping
            Matrix4x4 modelInvT = modelTransform[m].invertT();
            std::vector<Vertex> vertices, clippedVertices;
            for (int j = 0; j < 3; j++) {
                vector4Float coordsWorld = modelTransform[m] * complement<4>(models[m].vertex(i, j));
                vector4Float clipCoord = projectedView * coordsWorld;
                vector2Float uv = models[m].uv(i, j);
                vector3Float normal = project<3>(modelInvT * vector4Float(models[m].normal(i, j), 0.0f));
                Vertex vertex(coordsWorld, clipCoord, uv, normal);
                vertices.push_back(vertex);
            }

            //homogeneus clip space
            std::vector<Vertex> intermediate;
            axisClip(vertices, clippedVertices, 2);

            if (clippedVertices.size() < 3) continue; //processing triangle polygones

            //Darboux basis
            Matrix<2, 2, float> invI;
            Matrix<2, 3, float> J, tangents;

            invI[0] = models[m].uv(i, 1) - models[m].uv(i, 0);
            invI[1] = models[m].uv(i, 2) - models[m].uv(i, 0);

            J[0] = project<3>(modelTransform[m] * vector4Float(models[m].vertex(i, 1) - models[m].vertex(i, 0), 0.0f));
            J[1] = project<3>(modelTransform[m] * vector4Float(models[m].vertex(i, 2) - models[m].vertex(i, 0), 0.0f));

            tangents = invI.invert() * J;
            PhongShader.tangent = tangents[0].normalize();
            PhongShader.biTangent = tangents[1].normalize();

            Matrix<3, 4, float> coordsScreen;
            for (size_t j = 1; j < clippedVertices.size() - 1; j++) {
                coordsScreen[0] = PhongShader.vertexCoord(0, clippedVertices[0].coordsWorld, clippedVertices[0].uv, clippedVertices[0].normal);
                coordsScreen[1] = PhongShader.vertexCoord(1, clippedVertices[j].coordsWorld, clippedVertices[j].uv, clippedVertices[j].normal);
                coordsScreen[2] = PhongShader.vertexCoord(2, clippedVertices[j + 1].coordsWorld, clippedVertices[j + 1].uv, clippedVertices[j + 1].normal);

                rasterize(coordsScreen, PhongShader, zBuffer, colorBuffer, frame.get_width(), frame.get_height(), samples, MSAA);
            }
        }
    }
}

Matrix4x4 ShadowMapping(std::vector<Model>& models,
                        std::vector<Matrix4x4>& modelTransform,
                        size_t N,
                        std::vector<float>& zBuffer,
                        std::vector<vector3Float>& colorBuffer,
                        TGAImage& frame, Matrix4x4 view,
                        std::vector<std::vector<float> >& SSAA) {
    Matrix4x4 ortho = Orthographic(-2.0f, 2.0f, -2.0f, 2.0f, -0.01f, -10.0f);
    Matrix4x4 viewport = Viewport(frame.get_width(), frame.get_height());
    DepthShader depthShader;

    for (size_t m = 0; m < N; m++) {
        depthShader.projectedView = viewport * ortho * view;
        //depth map
        Matrix4x4 modelIT = modelTransform[m].invertT();
        for (int i = 0; i < models[m].faceCount(); i++) {
            Matrix<3, 4, float> coordsScreen;
            for (int j = 0; j < 3; j++) {
                coordsScreen[j] = depthShader.vertexCoord(j, 
                                                          modelTransform[m] * complement<4>(models[m].vertex(i, j)),
                                                          models[m].uv(i, j),
                                                          project<3>(modelIT * vector4Float(models[m].normal(i, j), 0.0f)));
            }

            rasterize(coordsScreen, depthShader, zBuffer, colorBuffer, frame.get_width(), frame.get_height(), 1, SSAA);
        }
    }

    return depthShader.projectedView;
}
