#define _USE_MATH_DEFINES

#include <limits>
#include <vector>
#include <cfloat>

#include "../headers/matrixops.h"
#include "../headers/model.h"
#include "../headers/renderer.h"
#include "../headers/tgaimage.h"

//main parameters
size_t SCREEN_WIDTH = 1600;
size_t SCREEN_HEIGHT = 1600;

size_t SAMPLES = 4;
std::vector<std::vector<float> > MSAA = {
    {0.25f, 0.25f}, {0.25f, 0.75f},
    {0.75f, 0.25f}, {0.75f, 0.75f}
};

std::vector<std::vector<float> > SSAA = {
    {1.0f, 1.0f}
};

vector3Float lightPosition(1.0f, 1.0f, 1.0f);
ShadingParams params(vector3Float(0.3f, 0.3f, 0.3f), vector3Float(1.0f, 1.0f, 1.0f), vector3Float(0.5f, 0.5f, 0.5f));

vector3Float pov(1.0f, 1.0f, 3.0f);
vector3Float center(0.0f, 0.0f, 0.0f);
vector3Float up(0.0f, 1.0f, 0.0f);

//Paint frame
void Paint(TGAImage& frame, std::vector<float> zBuffer, std::vector<vector3Float> colorBuffer, size_t samples) {
#pragma omp parallel for
    for (size_t x = 0; x < frame.get_width(); x++) {
        for (size_t y = 0; y < frame.get_height(); y++) {
            vector3Float color(0.0f, 0.0f, 0.0f);
            for (size_t i = 0; i < samples; i++) {
                size_t coordinate = (y * frame.get_width() + x) * samples + i;
                if (zBuffer[coordinate] > -FLT_MAX) {
                    color = color + colorBuffer[coordinate];
                }
            }

            color = color / samples;
            frame.set(x, y, TGAColor(color.x, color.y, color.z, 255));
        }
    }
}

int main() {
    std::vector<float> zBuffer(SCREEN_WIDTH * SCREEN_HEIGHT * SAMPLES);
    std::vector<vector3Float> colorBuffer(SCREEN_WIDTH * SCREEN_HEIGHT * SAMPLES);
    std::vector<float> shadowBuffer(SCREEN_WIDTH * SCREEN_HEIGHT);
    std::vector<vector3Float> shadowColorBuffer(SCREEN_WIDTH * SCREEN_HEIGHT);

    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        for (int j = 0; j < SAMPLES; j++) {
            zBuffer[SAMPLES * i + j] = -FLT_MAX;
            colorBuffer[SAMPLES * i + j] = vector3Float(0.0f, 0.0f, 0.0f);
        }
        shadowBuffer[i] = -FLT_MAX;
        shadowColorBuffer[i] = vector3Float(0.0f, 0.0f, 0.0f);
    }

    size_t N = 2;
    std::vector<Model> models(N);
    models[0] = Model("../obj/teapot.obj");
    models[1] = Model("../obj/floor.obj");
    std::vector<Matrix4x4> modelPos(N);
    modelPos[0] = Matrix4x4::identity();
    modelPos[1] = Matrix4x4::identity();
	modelPos[1][1][3] = -0.3f;
    std::cout << "\n";

    Matrix4x4 view = View(lightPosition, center, up);
    TGAImage frame(SCREEN_WIDTH, SCREEN_HEIGHT, TGAImage::RGB);
    std::cout << "shadowing\n";
    Matrix4x4 lightVpPV = ShadowMapping(models, modelPos, N, shadowBuffer, shadowColorBuffer, frame, view, SSAA);

    std::cout << "shading\n";
    view = View(pov, center, up);
    PhongShading(models, modelPos, N, zBuffer, colorBuffer, shadowBuffer, lightVpPV, frame, SAMPLES, lightPosition, params, view, pov, MSAA);

    std::cout << "painting\n";
    Paint(frame, zBuffer, colorBuffer, SAMPLES);
    frame.write_tga_file("../output/out.tga");

    std::cout << "finished\n";

    return 0;
}
