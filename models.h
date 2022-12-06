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

    // Texture Stuff
    GLuint texture;
    unsigned char* tex_bytes;
    int img_width, img_height, color_channels;

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

    std::vector<GLfloat> getVertexData() {
        return fullVertexData;
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

    GLuint getTexture() {
        return texture;
    }
};
