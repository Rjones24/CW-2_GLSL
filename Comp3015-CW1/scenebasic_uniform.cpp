#include "scenebasic_uniform.h"
#include "helper/texture.h"

#include <iostream>
using std::cerr;
using std::endl;

#include <glm/gtc/matrix_transform.hpp>
using glm::vec3;
using glm::mat4;

#include <sstream>

//Sets up the camera positions as well as where the front and top of the camera are
glm::vec3 cameraPos = glm::vec3(0.0f, 4.0f, 10.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

bool firstMouse = true; // checks for the first time the mouse is processed in my the screen
double mouseX,  mouseY;
float lastX = 400, lastY = 300, yaw = -90.0f, pitch = 0.0f;

GLFWwindow* window1;

//this is where all of the ojects in the scene are initiated
SceneBasic_Uniform::SceneBasic_Uniform() : tPrev(0), plane(50.0f, 50.0f, 1, 1),
                                            sky(100.0f), drawBuf(1), drawBuf2(1), time(0), deltaT(0), nParticles(500),
                                            particleLifetime(1.0f), emitterPos(-0.08, 0.1, 0), emitterDir(0, 0.1, 0), lightPos(0.0f, 3.0f, 4.0f, 1.0f)
{
    statue = ObjMesh::load("../Comp3015-CW1/media/statue.obj", true);
    Chalic = ObjMesh::load("../Comp3015-CW1/media/Stone Chalic_OBJ.obj", false, true);
}

void SceneBasic_Uniform::initScene()
{
    compile();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    // sets window1 to the current window. This is so the camera will be able to move with keyboard input
    window1 = glfwGetCurrentContext();
    projection = mat4(1.0f);

    //extract the cube texture
    GLuint cubeTex = Texture::loadCubeMap("../Comp3015-CW1/media/texture/cube/skybox/skybox", ".jpg");

    //activate and bindtexture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTex);
    
    prog.use();
    //sets the position and intensitys used for the directional light(sunlight)
    prog.setUniform("Light.Position", view * glm::vec4(-4.0f, 0.0f, 0.0f, 0.0f));
    prog.setUniform("Light.L", vec3(0.7f, 0.7f, 0.2f));
    prog.setUniform("Light.La", vec3(0.1f));

    // sets all of the information for the spot lights. 
    prog.setUniform("Spot[0].intensity", vec3(0.9f, 0.9f, 0.9f));
    prog.setUniform("Spot[0].Exponent", 30.0f);
    prog.setUniform("Spot[0].Cutoff", 35.0f);

    prog.setUniform("Spot[1].intensity", vec3(0.9f, 0.9f, 0.9f));
    prog.setUniform("Spot[1].Exponent", 30.0f);
    prog.setUniform("Spot[1].Cutoff", 35.0f);

    prog.setUniform("Spot[2].intensity", vec3(0.9f, 0.9f, 0.9f));
    prog.setUniform("Spot[2].Exponent", 30.0f);
    prog.setUniform("Spot[3].Cutoff", 35.0f);

    prog.setUniform("Spot[3].intensity", vec3(0.9f, 0.9f, 0.9f));
    prog.setUniform("Spot[3].Exponent", 30.0f);
    prog.setUniform("Spot[3].Cutoff", 1.0f);

    norm.use();
    //sets the lights position and intenisity used for the pbr calulations
    norm.setUniform("Light[0].L", glm::vec3(50.0f));
    norm.setUniform("Light[0].Position", view * lightPos);
    norm.setUniform("Light[1].L", glm::vec3(0.9f));
    norm.setUniform("Light[1].Position", glm::vec4(0, 2.15f, -7.0f, 0));

    // loads in all of the textures that are used by the scene
    GLuint statue = Texture::loadTexture("../Comp3015-CW1/media/texture/Statue_diff.jpg");
    GLuint moss = Texture::loadTexture("../Comp3015-CW1/media/texture/moss.png");
    GLuint floor = Texture::loadTexture("../Comp3015-CW1/media/texture/Grass.jpg");
    GLuint water = Texture::loadTexture("../Comp3015-CW1/media/texture/bluewater.png");
    GLuint diffuse = Texture::loadTexture("../Comp3015-CW1/media/texture/Base_Color.png");
    GLuint normal = Texture::loadTexture("../Comp3015-CW1/media/texture/Normal.png");
    GLuint mossnoise = NoiseTex::generate2DTex(100.0f);

    //all of the textures are then bound
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, statue);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, moss);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, floor);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, water);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, diffuse);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, normal);
    glActiveTexture(GL_TEXTURE8);
    glBindTexture(GL_TEXTURE_2D, mossnoise);
    

    glActiveTexture(GL_TEXTURE6);
    Texture::loadTexture("../Comp3015-CW1/media/texture/fire.png");

    glActiveTexture(GL_TEXTURE7);
    ParticleUtils::createRandomTex1D(nParticles * 3);

    //sets up all of the buffers used by the partical sytem
    initBuffers();

    particle.use();
    // sets all of the uniforms used in the partical sytem for the left partical sytem
    particle.setUniform("RandomTex", 7);
    particle.setUniform("ParticleTex", 6);
    particle.setUniform("ParticleLifetime", particleLifetime);
    particle.setUniform("Accel", vec3(0.0f, 0.5f, 0.0f));
    particle.setUniform("ParticleSize", 1.0f);
    particle.setUniform("Emitter", vec3(-0.08, 0.1, 0));
    particle.setUniform("EmitterBasis", ParticleUtils::makeArbitraryBasis(emitterDir));

    particle2.use();
    // sets all of the uniforms used in the partical sytem for the right partical sytem
    particle2.setUniform("RandomTex", 7);
    particle2.setUniform("ParticleTex", 6);
    particle2.setUniform("ParticleLifetime", particleLifetime);
    particle2.setUniform("Accel", vec3(0.0f, 0.5f, 0.0f));
    particle2.setUniform("ParticleSize", 1.0f);
    particle2.setUniform("Emitter", vec3(-0.08, 0.1, 0));
    particle2.setUniform("EmitterBasis", ParticleUtils::makeArbitraryBasis(emitterDir));
}

void SceneBasic_Uniform::initBuffers() {
    //generates buffer object names with size 2
    glGenBuffers(2, posBuf);
    glGenBuffers(2, velBuf);
    glGenBuffers(2, age);

    int size = nParticles * 3 * sizeof(GLfloat);
    //bindes all of the array infomation  with the buffer object names
    glBindBuffer(GL_ARRAY_BUFFER, posBuf[0]);
    glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, posBuf[1]);
    glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);

    glBindBuffer(GL_ARRAY_BUFFER, velBuf[0]);
    glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, velBuf[1]);
    glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);

    glBindBuffer(GL_ARRAY_BUFFER, age[0]);
    glBufferData(GL_ARRAY_BUFFER, nParticles * sizeof(float), 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, age[1]);
    glBufferData(GL_ARRAY_BUFFER, nParticles * sizeof(float), 0, GL_DYNAMIC_COPY);

    std::vector<GLfloat> tempData(nParticles);
    //creates the rate to spawn the Particles
    float rate = particleLifetime / nParticles;
    for (int i = 0; i < nParticles; i++)
    {
        tempData[i] = rate * (i - nParticles);

    }

    glBindBuffer(GL_ARRAY_BUFFER, age[0]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, nParticles * sizeof(float), tempData.data());

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenVertexArrays(2, particleArray);

    // Set up particle array 0
    glBindVertexArray(particleArray[0]);
    glBindBuffer(GL_ARRAY_BUFFER, posBuf[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, velBuf[0]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, age[0]);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    // Set up particle array 1
    glBindVertexArray(particleArray[1]);
    glBindBuffer(GL_ARRAY_BUFFER, posBuf[1]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, velBuf[1]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, age[1]);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    // Setup the feedback objects
    glGenTransformFeedbacks(2, feedback);

    // Transform feedback 0
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[0]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, posBuf[0]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, velBuf[0]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, age[0]);

    // Transform feedback 1
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[1]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, posBuf[1]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, velBuf[1]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, age[1]);

    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
}

void SceneBasic_Uniform::compile()
{
	try {
        // creates and links the basic shader used for the scene
		prog.compileShader("shader/basic_uniform.vert");
		prog.compileShader("shader/basic_uniform.frag");
		prog.link();

        // creates and links the shaders used for the skybox
        skyboxShader.compileShader("shader/Skybox.vert");
        skyboxShader.compileShader("shader/Skybox.frag");
        skyboxShader.link();

        // creates and links the shaders used for the pbr
        norm.compileShader("shader/Normal_Map.vert");
        norm.compileShader("shader/Normal_Map.frag");
        norm.link();

        // creates and links the shaders used for the partical sytems
        particle.compileShader("shader/particle.vert");
        particle.compileShader("shader/particle.frag");

        GLuint progHandle = particle.getHandle();
        const char* outputNames[] = { "Position", "Velocity", "Age" };
        glTransformFeedbackVaryings(progHandle, 3, outputNames, GL_SEPARATE_ATTRIBS);

        particle.link();

        particle2.compileShader("shader/Particle - Copy.vert");
        particle2.compileShader("shader/Particle - Copy.frag");

        GLuint progHandle2 = particle2.getHandle();
        const char* outputNames2[] = { "Position", "Velocity", "Age" };
        glTransformFeedbackVaryings(progHandle2, 3, outputNames2, GL_SEPARATE_ATTRIBS);

        particle2.link();
        prog.use();

	} catch (GLSLProgramException &e) {
		cerr << e.what() << endl;
		exit(EXIT_FAILURE);
	}
}

void SceneBasic_Uniform::update( float t )
{
    view = lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

    //process the keyboard input
    processInputs(window1);
    //disables the CURSOR_ from being visible on the screen
    glfwSetInputMode(window1, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // calculates the time since the last frame
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    deltaT = (t - time);
    time = t;
    float lightAngle = 0.0f;
    lightRotationSpeed = 1.5f;
    lightAngle = glm::mod(lightAngle + deltaT * lightRotationSpeed, glm::two_pi<float>());
    lightPos.x = glm::cos(lightAngle) * 7.0f;
    lightPos.y = 3.0f;
    lightPos.z = glm::sin(lightAngle) * 7.0f;
}

void  SceneBasic_Uniform::fire() {
    particle.use();
    particle.setUniform("EmitterBasis", ParticleUtils::makeArbitraryBasis(vec3(0, 0.1, 0)));
    particle.setUniform("smoke", 0);
    particle.setUniform("Accel", vec3(0.0f, 0.5f, 0.0f));

    particle2.use();
    particle2.setUniform("EmitterBasis", ParticleUtils::makeArbitraryBasis(vec3(0, 0.1, 0)));
    particle2.setUniform("smoke", 0);
    particle2.setUniform("Accel", vec3(0.0f, 0.5f, 0.0f));
    glActiveTexture(GL_TEXTURE6);
    Texture::loadTexture("../Comp3015-CW1/media/texture/fire.png");
}

void  SceneBasic_Uniform::smoke() {
    particle.use();
    particle.setUniform("EmitterBasis", ParticleUtils::makeArbitraryBasis(vec3(1, 0.1, 0)));
    particle.setUniform("Accel", vec3(0.0f, 1.0f, 0.0f));
    particle.setUniform("smoke", 1);

    particle2.use();
    particle2.setUniform("EmitterBasis", ParticleUtils::makeArbitraryBasis(vec3(1, 0.1, 0)));
    particle2.setUniform("Accel", vec3(0.0f, 1.0f, 0.0f));
    particle2.setUniform("smoke", 1);
    glActiveTexture(GL_TEXTURE6);
    Texture::loadTexture("../Comp3015-CW1/media/texture/smoke.png");
}

// takes the keyboard input and converts it to moving the camera in a set direction depending on the key pressed
void  SceneBasic_Uniform::processInputs(GLFWwindow* window)
{
    float cameraSpeed = 2.5f * deltaTime;     
    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) {
        prog.use();
        prog.setUniform("Light.Position", view * glm::vec4(0.0f, -5.0f, -5.0f, 0.0f));
        fire();
    }
    else if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        prog.use();
        prog.setUniform("Light.Position", view * glm::vec4(-40.0f, 0.0f, -5.0f, 0.0f));
        fire();
    }
    else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        prog.use();
        prog.setUniform("Light.Position", view * glm::vec4(-30.0f, 10.0f, -5.0f, 0.0f));
        smoke();
    }
    else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        prog.use();
        prog.setUniform("Light.Position", view * glm::vec4(-20.0f, 20.0f, -5.0f, 0.0f));
        smoke();
    }
    else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
        prog.use();
        prog.setUniform("Light.Position", view * glm::vec4(-10.0f, 30.0f, -5.0f, 0.0f));
        smoke();
    }
    else if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) {
        prog.use();
        prog.setUniform("Light.Position", view * glm::vec4(0.0f, 50.0f, -5.0f, 0.0f));
        smoke();
    }
    else if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) {
        prog.use();
        prog.setUniform("Light.Position", view * glm::vec4(10.0f, 30.0f, -5.0f, 0.0f));
        smoke();
    }
    else if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS) {
        prog.use();
        prog.setUniform("Light.Position", view * glm::vec4(20.0f, 20.0f, -5.0f, 0.0f));
        fire();
    }
    else if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS) {
        prog.use();
        prog.setUniform("Light.Position", view * glm::vec4(30.0f, 10.0f, -5.0f, 0.0f));
        fire();
    }
    else if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS) {
        prog.use();
        prog.setUniform("Light.Position", view * glm::vec4(40.0f, 0.0f, -5.0f, 0.0f));

        fire();
    }
}

void SceneBasic_Uniform::render()
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //tells the scene to use the skybox shader and then how to load in the skybox
    skyboxShader.use();
    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, -1.5f, 0.0f));
    mat4 mv = (view )* model;
    skyboxShader.setUniform("ModelViewMatrix", mv );
    skyboxShader.setUniform("MVP", projection * mv);
    sky.render();

    //tells the scene to use the basic shader
    prog.use();
    prog.setUniform("MaterialColor", glm::vec4(0.055f, 0.255f, 0.055f, 1.0f));
    prog.setUniform("Spot[0].Position", view * vec4(-5.0f, 1.0f, 0.0f, 1.0f));
    prog.setUniform("Spot[0].Direction", vec3(5.0f,2.0f,0.0f));

    prog.setUniform("Spot[1].Position", view * vec4(-5.0f, 1.0f, 0.0f, 1.0f));
    prog.setUniform("Spot[1].Direction", vec3(5.0f, 2.0f, -1.0f));

    prog.setUniform("Spot[2].Position", view * vec4(5.0f, 1.0f, 0.0f, 1.0f));
    prog.setUniform("Spot[2].Direction", vec3(-5.0f, 2.0f, 0.0f));

    prog.setUniform("Spot[3].Position", view * vec4(5.0f, 1.0f, 0.0f, 1.0f));
    prog.setUniform("Spot[3].Direction", vec3(-5.0f, 2.0f, -1.0f));

    //sets the material infomation for the model to be loaded
    prog.setUniform("Material.Kd", 0.4f, 0.4f, 0.4f);
    prog.setUniform("Material.Ks", 0.1f, 0.1f, 0.1f);
    prog.setUniform("Material.Ka", 0.1f, 0.1f, 0.1f);
    prog.setUniform("Material.Shininess", 1.0f);
    prog.setUniform("Material.pick", 0.0f);

    //sets the model scale, rotation and transform in the scene
    model = mat4(1.0f);
    model = glm::rotate(model, glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, vec3(0.1f, 0.1f, 0.1f));
    model = glm::translate(model, vec3(0.0f, 60.0f, 40.0f));
    setMatrices(prog);

    statue->render();

    model = glm::translate(model, vec3(30.0f, 0.0f, 0.0f));
    setMatrices(prog);
    statue->render();
  
    model = glm::rotate(model, glm::radians(-180.0f), vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, vec3(0.0f, 120.0f, 0.0f));
    setMatrices(prog);
    statue->render();

    model = glm::translate(model, vec3(30.0f, 0.0f, 0.0f));
    setMatrices(prog);
    statue->render();

    //sets the material infomation for the model to be loded
    prog.setUniform("Material.Kd", 0.1f, 0.1f, 0.1f);
    prog.setUniform("Material.Ks", 0.9f, 0.9f, 0.9f);
    prog.setUniform("Material.Ka", 0.1f, 0.1f, 0.1f);
    prog.setUniform("Material.Shininess", 10.0f);
    prog.setUniform("Material.pick", 1.0f);

    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, 0.5f, 0.0f));

    setMatrices(prog);

    //tells the scene what to render
    plane.render();

    norm.use();

    model = mat4(1.0f);

    norm.setUniform("Material.Rough", 0.8f);
    norm.setUniform("Material.Metal", 0);

    model = glm::translate(model, vec3(5.0f, 2.0f, -10.0f));
    model = glm::scale(model, vec3(10.0f, 10.0f, 10.0f));

    setMatrices(norm);

    Chalic->render();

    model = mat4(1.0f);
    model = glm::translate(model, vec3(-5.0f, 2.0f, -10.0f));
    model = glm::scale(model, vec3(10.0f, 10.0f, 10.0f));

    setMatrices(norm);

    Chalic->render();

    particle.use();
    particle.setUniform("Time", time);
    particle.setUniform("DeltaTime", deltaT);

    particle.setUniform("Pass", 1);

    glEnable(GL_RASTERIZER_DISCARD);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[drawBuf]);
    glBeginTransformFeedback(GL_POINTS);

    glBindVertexArray(particleArray[1 - drawBuf]);
    glVertexAttribDivisor(0, 0);
    glVertexAttribDivisor(1, 0);
    glVertexAttribDivisor(2, 0);
    glDrawArrays(GL_POINTS, 0, nParticles);
    glBindVertexArray(0);

    glEndTransformFeedback();
    glDisable(GL_RASTERIZER_DISCARD);

 
    particle.setUniform("Pass", 2);
    setMatrices(particle);

    glDepthMask(GL_FALSE);

    glBindVertexArray(particleArray[drawBuf]);
    glVertexAttribDivisor(0, 1);
    glVertexAttribDivisor(1, 1);
    glVertexAttribDivisor(2, 1);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, nParticles);

    glBindVertexArray(0);
    glDepthMask(GL_TRUE);

    drawBuf = 1 - drawBuf;

    particle2.use();
    particle2.setUniform("Time", time);
    particle2.setUniform("DeltaTime", deltaT);

    particle2.setUniform("Pass", 1);

    glEnable(GL_RASTERIZER_DISCARD);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[drawBuf]);
    glBeginTransformFeedback(GL_POINTS);

    glBindVertexArray(particleArray[1 - drawBuf]);
    glVertexAttribDivisor(0, 0);
    glVertexAttribDivisor(1, 0);
    glVertexAttribDivisor(2, 0);
    glDrawArrays(GL_POINTS, 0, nParticles);
    glBindVertexArray(0);

    glEndTransformFeedback();
    glDisable(GL_RASTERIZER_DISCARD);

    particle2.setUniform("Pass", 2);
    setMatrices(particle2);

    glDepthMask(GL_FALSE);

    glBindVertexArray(particleArray[drawBuf]);
    glVertexAttribDivisor(0, 1);
    glVertexAttribDivisor(1, 1);
    glVertexAttribDivisor(2, 1);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, nParticles);

    glBindVertexArray(0);
    glDepthMask(GL_TRUE);


    drawBuf = 1 - drawBuf;
}

void SceneBasic_Uniform::resize(int w, int h)
{
    //sets the viewport sizes and the projection matrix
    glViewport(0, 0, w, h);
    width = w;
    height = h;
    projection = glm::perspective(glm::radians(60.0f), (float)w / h, 0.3f, 100.0f);
}

void  SceneBasic_Uniform::setMatrices(GLSLProgram& p) {

    //passess all of the matrix to the shader
    mat4 mv = view * model;
    p.setUniform("ModelViewMatrix", mv);
    p.setUniform("NormalMatrix", mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    p.setUniform("Proj", projection);
    p.setUniform("MVP", projection * mv);
}