#pragma once
#include "Camera.hpp"
#include "GLFW/glfw3.h"

class ControllableCamera : public Camera {
private:
    bool firstCursorMove = true;
    bool roatationCallback;
    double prevx;
    double prevy;
public:
    float sensitivity = 1.0f;
    float cameraSpeed = 1.0f;
    bool mouseLocked = false;
    GLFWwindow *window;

    ControllableCamera(glm::vec3 const &position, glm::vec3 const &rotation, GLFWwindow *window);
    virtual ~ControllableCamera();

    virtual void update(double deltatime);
    virtual void processPosition(double deltatime);
    virtual void processRotation(double xpos, double ypos);
    virtual void processScroll(int xoffset);
};

