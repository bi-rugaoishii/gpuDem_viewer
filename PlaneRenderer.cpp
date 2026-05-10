#include "PlaneRenderer.h"
#include <glm/gtc/matrix_transform.hpp>

/* == constructor ==*/
PlaneRenderer::PlaneRenderer()
{
    position = glm::vec3(0.0f);

    normal = glm::vec3(0.0f, 1.0f, 0.0f);

    size = 10.0f;
    thickness = 0.005f;

    visible = false;
}

void PlaneRenderer::render(Shader& shader)
{
    if (!visible)
    {
        return;
    }

    glm::vec3 up(0.0f, 1.0f, 0.0f);

    glm::vec3 n = glm::normalize(this->normal);

    float dotValue = glm::dot(up, n);

    dotValue = glm::clamp(dotValue, -1.0f, 1.0f);

    float angle = acos(dotValue);

    glm::vec3 axis = glm::cross(up, n);

    glm::mat4 model(1.0f);

    model = glm::translate(model, this->position);

    if (glm::length(axis) > 0.0001f)
    {
        model *= glm::rotate(glm::mat4(1.0f), angle, glm::normalize(axis));
    }

    model = glm::scale(model, glm::vec3(this->size,this->thickness,this->size));

    shader.setMat4("model", model);
    shader.setVec3("objectColor", glm::vec3(0.0f,0.7f,0.7f)); //cyan

    cubeMesh.draw();
}
