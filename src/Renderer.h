#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <nanogui/nanogui.h>

//GLFW
#include <GLFW/glfw3.h>

#ifndef __APPLE__
#  define GLEW_STATIC
#  include <GL/glew.h>
#endif

//STD
#include <iostream>
#include <string>
#include <vector>

using namespace nanogui;

////////////////////////////////
///////////Renderer/////////////
/////////////////////////c///////
class Renderer {
public:

	//GLFW window
	GLFWwindow* window;

	int width, height;

	Renderer();

    void begin();

    void end();

	void dispose();

	bool nextFrame();

	double currentTime();

private:

	void setupWindow();
};

#endif
