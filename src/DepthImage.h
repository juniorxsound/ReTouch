#ifndef _DEPTH_PLAYER_H_
#define _DEPTH_PLAYER_H_

#include <nanogui/nanogui.h>

//Eigen Linear Algebra library
#include <Eigen/Core>

//Helper file to keep this clean!
#include "Helpers.h"
#include "Shader.h"

//std
#include <stdio.h>
#include <string.h>
#include <vector>
#include <fstream>

//Eigen Abstraction
typedef Eigen::Matrix4f mat4;
typedef Eigen::Matrix3f mat3;
typedef Eigen::Vector4f vec4;
typedef Eigen::Vector3f vec3;
typedef Eigen::MatrixXf mat;

class DepthImage{
    public:

    mat verts;
    mat indices;
    mat normals;
    mat uvs;

    Program program;
    VertexArrayObject vao;
    Shader shader;

    //VertexBufferObjects
    VertexBufferObject vert_vbo;
    VertexBufferObject uv_vbo;


    /*
     * Constructor
     */
    DepthImage();

    void draw();

    void setImageSize(float x, float y);

    void bind();

    private:

    int numVerts;
    int numFaces;

    void initGL();
};

#endif
