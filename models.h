#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>

// GLM Headers
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// tinyloader
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

// textures
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#pragma once

class Model {
public:
    std::string path;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> material;
    std::string warning, error;
    tinyobj::attrib_t attributes;
    std::vector<GLfloat> fullVertexData;
    char const* tex_path;

    // More Stuff
    GLuint VAO, VBO;

    // Texture Stuff
    GLuint texture;
    unsigned char* tex_bytes;
    int img_width, img_height, color_channels;

    // Model Loading
    glm::mat4 transformation_matrix;

    // Constructor
    Model(std::string text, char const* link) {
        path = "3D/objects/" + text;
        tex_path = link;
        
        // set Texture
        setTexture();
    }

    // Model Functions
    bool success = tinyobj::LoadObj(&attributes,
        &shapes,
        &material,
        &warning,
        &error,
        path.c_str());

    bool loadMesh() {
        return tinyobj::LoadObj(&attributes,
            &shapes,
            &material,
            &warning,
            &error,
            path.c_str());
    }

    void fillVertexData() {
        for (int i = 0; i < shapes[0].mesh.indices.size(); i++) {

            tinyobj::index_t vData = shapes[0].mesh.indices[i];

            int vertexIndex = vData.vertex_index * 3;
            int uvIndex = vData.texcoord_index * 2;
            int normsIndex = vData.normal_index * 3;

            // X
            fullVertexData.push_back(
                attributes.vertices[vertexIndex]
            );

            // Y
            fullVertexData.push_back(
                attributes.vertices[vertexIndex + 1]
            );

            // Z
            fullVertexData.push_back(
                attributes.vertices[vertexIndex + 2]
            );

            // normals
            fullVertexData.push_back(
                attributes.normals[normsIndex]
            );

            fullVertexData.push_back(
                attributes.normals[normsIndex + 1]
            );

            fullVertexData.push_back(
                attributes.normals[normsIndex + 2]
            );

            // U
            fullVertexData.push_back(
                attributes.texcoords[uvIndex]
            );

            // V
            fullVertexData.push_back(
                attributes.texcoords[uvIndex + 1]
            );
        }
    }

    void apply() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER,
            sizeof(GL_FLOAT) * fullVertexData.size(),
            fullVertexData.data(),
            GL_STATIC_DRAW);

        glVertexAttribPointer(0,
            3,
            GL_FLOAT,
            GL_FALSE,
            8 * sizeof(GL_FLOAT),
            (void*)0);

        // norm ptr
        GLintptr normPtr = 3 * sizeof(float);
        glVertexAttribPointer(
            1,
            3,
            GL_FLOAT,
            GL_FALSE,
            8 * sizeof(GL_FLOAT),
            (void*)normPtr
        );

        // uv ptr
        GLintptr uvPtr = 6 * sizeof(float);
        glVertexAttribPointer(
            2,
            2,
            GL_FLOAT,
            GL_FALSE,
            8 * sizeof(GL_FLOAT),
            (void*)uvPtr
        );

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    // Texture Functions
    void setTexture() {
        stbi_set_flip_vertically_on_load(true);

        tex_bytes = stbi_load(tex_path,
            &img_width,
            &img_height,
            &color_channels, 0);

        glGenTextures(1, &texture);
        glActiveTexture(GL_TEXTURE);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexImage2D(GL_TEXTURE_2D,
            0,
            GL_RGBA,
            img_width,
            img_height,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            tex_bytes);

        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(tex_bytes);
    }

    std::vector<GLfloat> getVertexData() {
        return fullVertexData;
    }

    GLuint getVAO() {
        return VAO;
    }

    GLuint getVBO() {
        return VBO;
    }

    const GLuint* getAddVAO() {
        return &VAO;
    }

    const GLuint* getAddVBO() {
        return &VBO;
    }

    GLuint getTexture() {
        return texture;
    }

    void loadModel(float pos_x, float pos_y, float pos_z,
        float scale, float rot_x, float rot_y, float rot_z,
        float theta) {

        transformation_matrix = glm::mat4(1.0f);

        // translation
        transformation_matrix = glm::translate(transformation_matrix,
            glm::vec3(pos_x, pos_y, pos_z - 5.f));

        // scale (assume same values)
        transformation_matrix = glm::scale(transformation_matrix,
            glm::vec3(scale, scale, scale));

        // rotate
        transformation_matrix = glm::rotate(transformation_matrix,
            glm::radians(theta),
            glm::normalize(glm::vec3(rot_x, rot_y, rot_z)));
    }

    glm::mat4 getTransMatrix() {
        return transformation_matrix;
    }
};
