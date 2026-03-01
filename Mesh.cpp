#include "Mesh.h"
#include <glad/glad.h>

void Mesh::build(const std::vector<glm::vec3>& vertices,
                 const std::vector<glm::vec3>& normals)
{
    std::vector<float> data;

    for(size_t i=0;i<vertices.size();i++)
    {
        data.push_back(vertices[i].x);
        data.push_back(vertices[i].y);
        data.push_back(vertices[i].z);

        data.push_back(normals[i].x);
        data.push_back(normals[i].y);
        data.push_back(normals[i].z);
    }

    vertexCount = vertices.size();

    glGenVertexArrays(1,&VAO);
    glGenBuffers(1,&VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 data.size()*sizeof(float),
                 data.data(),
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,
                          sizeof(float)*6,(void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,
                          sizeof(float)*6,(void*)(sizeof(float)*3));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Mesh::draw() const
{
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES,0,vertexCount);
    glBindVertexArray(0);
}