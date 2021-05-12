#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"

#include <glad/glad.h>
#include "helper/glslprogram.h"
#include "helper/plane.h"
#include "helper/objmesh.h"
#include "helper/skybox.h"
#include "helper/noisetex.h"
#include "helper/texture.h"

#include "helper/cube.h"

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include "helper/particleutils.h"

using namespace glm;

class SceneBasic_Uniform : public Scene
{
private:

    GLSLProgram prog, skyProg;
    GLSLProgram norm;
    GLSLProgram skyboxShader;
    GLSLProgram particle;
    GLSLProgram particle2;

    SkyBox sky;

    Plane plane; //plane surface

    std::unique_ptr<ObjMesh> statue;
    std::unique_ptr<ObjMesh> Chalic;

    vec3 emitterPos, emitterDir;

    GLuint posBuf[2], velBuf[2], age[2];

    GLuint particleArray[2], particleArray2[2];
    GLuint feedback[2], initVel;
    GLuint drawBuf;

    float tPrev, lightangle, lightRotationSpeed;
    vec4 lightPos;
    int nParticles;

    float angle, time, particleLifetime, deltaT;

    void initBuffers();
    void fire();
    void smoke();

    void DrawPartical(GLSLProgram&);
    void DrawCalice();
    void DrawStatue();

    void setMatrices(GLSLProgram &);

    void processInputs(GLFWwindow *window);

    void compile();

public:
    SceneBasic_Uniform();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
};

#endif // SCENEBASIC_UNIFORM_H
