#include "Camera.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

Camera::Camera(glm::vec3 const &pos, glm::vec3 const &rotation) : position(pos), rotation(rotation), fov(45) {}

Camera::~Camera() {}

void Camera::update()
{
    front = glm::normalize(glm::vec3(
        cos(glm::radians(rotation.x)) * cos(glm::radians(rotation.y)),
        sin(glm::radians(rotation.y)),
        sin(glm::radians(rotation.x)) * cos(glm::radians(rotation.y))
    ));
    right = glm::normalize(glm::cross(front, glm::vec3(0, 1, 0)));
    up    = glm::normalize(glm::cross(right, front));
}

glm::mat4 Camera::getViewMatrix() const
{
    return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::getProjectionMatrix(int const windowWidth, int const windowHeight) const
{
    auto key = std::make_tuple(windowWidth, windowHeight, fov, near, far);
    if(m_ProjectionMatrixCache.find(key) != m_ProjectionMatrixCache.end()) return m_ProjectionMatrixCache[key];
    glm::mat4 matrix(glm::perspective(glm::radians(fov), (float) windowWidth / windowHeight, near, far));
    m_ProjectionMatrixCache[key] = matrix;
    return matrix;
}