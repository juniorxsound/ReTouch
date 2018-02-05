#include "DepthImage.h"


DepthImage::DepthImage(){

    //Initalize the OpenGL data structures
    initGL();

}

void DepthImage::draw(){
    //Set the current GL program prior to drawing
    vao.bind();
    program.bind();

    glDrawArrays(GL_TRIANGLES , 0, numVerts);

}

void DepthImage::setImageSize(float x, float y){
    vao.bind();
    program.bind();
    glUniform2f(program.uniform("tex_size"), x, y);
}

void DepthImage::bind(){
    vao.bind();
    program.bind();
}

 void DepthImage::initGL(){
     //Init the VertexArrayObject
 	vao.init();
 	vao.bind();

    //Init the vertecies VertexBufferObject
    vert_vbo.init();

    //Create a plane using two triangles
    verts = mat::Zero(3, 6);

    //Uniqe triangles
    verts.col(0) << -1.0, -1.0, 0.0;
    verts.col(1) << -1.0, 1.0, 0.0;
    verts.col(2) << 1.0, 1.0, 0.0;
    verts.col(3) << 1.0, 1.0, 0.0;
    verts.col(4) << -1.0, -1.0, 0.0;
    verts.col(5) << 1.0, -1.0, 0.0;

    numVerts = verts.cols();
    numFaces = numVerts / 3;

    vert_vbo.update(verts);

    uv_vbo.init();
    uvs = mat::Zero(2, 6);

    uvs.col(0) << 0, 1;
    uvs.col(1) << 0, 0;
    uvs.col(2) << 1, 0;
    uvs.col(3) << 1, 0;
    uvs.col(4) << 0, 1;
    uvs.col(5) << 1, 1;

    uv_vbo.update(uvs);

    //Load the GLSL files
	shader.load("../src/shaders/depth.vert",
		        "../src/shaders/depth.frag");

    //Setup the program and compile the shaders
	program.init(shader.vert, shader.frag, "color_out");
	program.bind();
	program.bindVertexAttribArray("position", vert_vbo);
    program.bindVertexAttribArray("uvs", uv_vbo);
 }
