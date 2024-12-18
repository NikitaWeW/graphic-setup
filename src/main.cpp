/*
i use this
cmake -S . -B build -DCMAKE_BUILD_TYPE=DEBUG -DCMAKE_CXX_FLAGS='-fdiagnostics-color=always -Wall'
*/

#include "glad/gl.h"
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"
#include "logger.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "Application.hpp"
#include "opengl/Renderer.hpp"
#include "utils/ControllableCamera.hpp"

#include <chrono>
#include <memory>
#include <thread>
#include <stdexcept>

#ifdef NDEBUG
extern const bool debug = false;
#else
extern const bool debug = true;
#endif
#define SHOW_LOGS         true // for readability
#define LOAD_NOW          true
#define FLIP_TEXTURES     true
#define FLIP_WINING_ORDER true

void renderthing(Application &app, Renderer &renderer, Camera &camera);
void imguistuff(Application &app, ControllableCamera &cam, PointLight &light, SpotLight &flashlight);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

int main(int argc, char **argv)
{
    bool fastLoad = false;
    for(int i = 0; i < argc; ++i) if(strcmp(argv[i], "--fast") == 0) fastLoad = true;
    printf("loading...\n"); // TODO: cool progress bar
    Application app;
    GLFWwindow *window = app.window;
    Model lightCube("res/models/cube.obj");
    ControllableCamera camera(window, {0, 0, 5}, {-90, 0, 0});
    PointLight light;
    SpotLight flashlight;
    VertexBufferLayout layout;
    Renderer renderer;

//  =========================================== 

    renderer.getLights().push_back(&flashlight);
    renderer.getLights().push_back(&light);

    app.shaders = {
        {"shaders/lighting.glsl", SHOW_LOGS},
        {"shaders/basic.glsl",    SHOW_LOGS},
        {"shaders/test.glsl",     SHOW_LOGS}
    }; // on shader reload contents will be recompiled, if fails failed shader will be restored. shows in shader list.

    flashlight.position  = camera.position;
    flashlight.direction = camera.getFront();

    light.position= glm::vec3{2, 1, 3};

    app.plainColorShader = Shader{"shaders/plain_color.glsl", SHOW_LOGS};
    app.quad = Model{"res/models/quad.obj"};
    app.cube = Model{"res/models/cube.obj"};
// TODO: why tf is cube black?

//   ==================================================================

    app.loadModel  ("res/models/cube.obj",                          {  FLIP_TEXTURES,  FLIP_WINING_ORDER });
    if(!fastLoad) app.loadModel  ("res/models/backpack/backpack.obj",             { !FLIP_TEXTURES, !FLIP_WINING_ORDER });
    if(!fastLoad) app.loadModel  ("res/models/rock/namaqualand_cliff_02_4k.gltf", {  FLIP_TEXTURES, !FLIP_WINING_ORDER });
    app.loadModel  ("res/models/lemon/lemon_4k.gltf",               {  FLIP_TEXTURES, !FLIP_WINING_ORDER });
    app.loadModel  ("res/models/apple/food_apple_01_4k.gltf",       {  FLIP_TEXTURES, !FLIP_WINING_ORDER });
    app.loadTexture("res/textures/tile.png",                        {  FLIP_TEXTURES });
    app.loadTexture("res/textures/white.png",                       {  FLIP_TEXTURES });
    if(!fastLoad) app.loadTexture("res/textures/oak.jpg",                         {  FLIP_TEXTURES });
    app.loadTexture("res/textures/concrete.jpg",                    {  FLIP_TEXTURES });
    if(!fastLoad) app.loadTexture("res/textures/brick_wall.jpg",                  {  FLIP_TEXTURES });

    glEnable(GL_STENCIL_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glfwSwapInterval(1);
    glfwSetInputMode(window, GLFW_CURSOR, camera.mouseLocked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    glfwSetWindowUserPointer(window, &camera);
    glfwSetKeyCallback(window, key_callback);
    glfwSetScrollCallback(window, scroll_callback);

    LOG_INFO("loaded!");

    std::thread flash = std::thread{[&app](){
        app.clearColor = glm::vec4{0.5, 0.5, 0.5, 1};
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        app.clearColor = glm::vec4{0, 0, 0, 1};
    }};

    while (!glfwWindowShouldClose(window))
    {
        auto start = std::chrono::high_resolution_clock::now();
        camera.update(app.deltatime);
        flashlight.position  = camera.position;
        flashlight.direction = camera.getFront();
        glfwGetWindowSize(window, &camera.windowWidthPx, &camera.windowHeightPx);
        
        renderthing(app, renderer, camera);

        lightCube.resetMatrix();
        lightCube.translate(light.position);
        lightCube.scale(glm::vec3{0.03125});

        if(light.enabled) {
            app.plainColorShader.bind();
            glUniform3fv(app.plainColorShader.getUniform("u_color"), 1, &light.color.x);
            renderer.draw(lightCube, app.plainColorShader, camera);
        }

        imguistuff(app, camera, light, flashlight);

        glfwSwapBuffers(window);
        glfwPollEvents();
        ++app.frameCounter;
        app.deltatime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count() * 1.0E-6;
    }
    flash.join();
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    ControllableCamera &camera = *static_cast<ControllableCamera *>(glfwGetWindowUserPointer(window));
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        camera.mouseLocked = !camera.mouseLocked;
        if (camera.mouseLocked)
        {
            camera.firstCursorMove = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
    if(key == GLFW_KEY_LEFT_CONTROL && action == GLFW_RELEASE) {
        // evaluate fov
        if (camera.fov < 1.0f)
            camera.fov = 1.0f;
        if (camera.fov > 45.0f)
            camera.fov = 45.0f;
    } else {
        ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
    }
}
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    ControllableCamera *cam = static_cast<ControllableCamera *>(glfwGetWindowUserPointer(window));
    if(cam->mouseLocked) {
        cam->fov -= (float)yoffset * 4.5f;
        if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
            if (cam->fov < 0.01f)
                cam->fov = 0.01f;
            if (cam->fov > 60.0f)
                cam->fov = 60.0f;
        } else {
            if (cam->fov < 1.0f)
                cam->fov = 1.0f;
            if (cam->fov > 45.0f)
                cam->fov = 45.0f;
        }
    } else {
        ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
    }
}