//App components
#include "DepthImage.h"
#include "Renderer.h"

//Include NanoGUI for simple GUI
#include <nanogui/nanogui.h>

//Include Eigen for linear algebra
#include <Eigen/Core>

//Eigen Abstraction - less is more (;
typedef Eigen::Matrix4f mat4;
typedef Eigen::Matrix3f mat3;
typedef Eigen::Vector4f vec4;
typedef Eigen::Vector3f vec3;
typedef Eigen::MatrixXf mat;

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

//STD
#include <iostream>
#include <string>
#include <vector>

//Depth of Field states
bool toggleColorDepth = false; // Debug: show depth-map
bool isDepthOfField = false; // Is DOF on or not
float dofIntensity = 1.0; //Multiply the per pixel blend of the DOF
bool depthOrder = 0; // Is the depth forward or backwards

//Lighting states
bool isColorChanging = false;
float foregroundIntensity = 0.0;
float backgroundIntensity = 0.0;

//gl Texture class based on stb image loader
static int NUM_TEXTURES = -1;
struct Texture{
    //General props
    int width, height, bits;

    //Container for raw image data
    unsigned char* image;

    int texIdentifier = 0;

    //GL Texture
    GLuint texture;

    Texture(char const *path,
            Program program,
            std::string uniform){


        NUM_TEXTURES++;
        texIdentifier = NUM_TEXTURES;

        loadFromFile(path, program, uniform);

    }

    void loadFromFile(char const *path,
                      Program program,
                      std::string uniform){

        glGenTextures(1, &texture);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        image = stbi_load(path, &width, &height, &bits, 0);

        //Log the texture sizes
        std::cout << "Image width: " << width << " and height: " << height << " bits: " << bits << std::endl;

        //Generate a gl tex
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                      GL_UNSIGNED_BYTE, image);
        stbi_image_free(image);

        //Make sure to bind for re-loading
        program.bind();
        glUniform1i(program.uniform(uniform), 0);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } const

    void bind(Program program){
        //Bind texture
        glActiveTexture(GL_TEXTURE0 + texIdentifier);
        glBindTexture(GL_TEXTURE_2D, texture);
    }
};

//Create a renderer - holds the GLFW window in renderer.window
Renderer renderer;

//Create a depth image
DepthImage example;

// example.bind();
Texture top_bottom("../resources/top_bottom.png",
                  example.program,
                  "topBottomMap");

//GUI instance
using namespace nanogui;
Screen *screen = nullptr;

int main(int /* argc */, char ** /* argv */) {

    glUniform2f(example.program.uniform("ScreenSize"), renderer.width, renderer.height);
    glUniform2f(example.program.uniform("texSize"), top_bottom.width, top_bottom.height);

    // Create a nanogui screen and pass the glfw pointer to initialize
    screen = new Screen();
    screen->initialize(renderer.window, true);
    FormHelper *controls = new FormHelper(screen);
    FormHelper *lighting = new FormHelper(screen);
    FormHelper *examples = new FormHelper(screen);
    ref<Window> controlsWin = controls->addWindow(Eigen::Vector2i(10, 10), "Depth of Field");
    ref<Window> lightingWin = lighting->addWindow(Eigen::Vector2i(10, 170), "Lighting");
    ref<Window> examplesWin = examples->addWindow(Eigen::Vector2i(10, 355), "Example Scenes");

    /********************
    * Lighting
    *********************/
    lighting->addVariable("Enable Color Retouching", isColorChanging);
    Color fcolor;
    lighting->addVariable("Background Color", fcolor)
       ->setFinalCallback([](const Color &c) {
             std::cout << "ColorPicker Final Callback: ["
                       << c.r() << ", "
                       << c.g() << ", "
                       << c.b() << ", "
                       << c.w() << "]" << std::endl;
            example.bind();
            glUniform3f(example.program.uniform("frontColor"), c.r(), c.g(), c.b());
    });
    lighting->addVariable("Background Intensity", foregroundIntensity)->setSpinnable(true);

    Color bcolor;
    lighting->addVariable("Foreground Color", bcolor)
       ->setFinalCallback([](const Color &c) {
             std::cout << "ColorPicker Final Callback: ["
                       << c.r() << ", "
                       << c.g() << ", "
                       << c.b() << ", "
                       << c.w() << "]" << std::endl;
            example.bind();
            glUniform3f(example.program.uniform("backColor"), c.r(), c.g(), c.b());
    });
    lighting->addVariable("Foreground Intensity", backgroundIntensity)->setSpinnable(true);

    /********************
    * Depth of Field
    *********************/
    controls->addVariable("Show Depth-map", toggleColorDepth);
    controls->addVariable("Enable Depth of Field", isDepthOfField);
    controls->addVariable("DOF Intensity", dofIntensity)->setSpinnable(true);
    controls->addVariable("DOF Background/Foreground", depthOrder);

    examples->addButton("Living Room", []() {
        std::cout << "Living Room Scene Selected" << std::endl;
        top_bottom.loadFromFile("../resources/top_bottom.png",
                                example.program,
                                "topBottomMap");

    });

    examples->addButton("Pulp Fiction", []() {
        std::cout << "Pulp Fiction Scene Selected" << std::endl;
        top_bottom.loadFromFile("../resources/pulp2.png",
                                example.program,
                                "topBottomMap");
    });

    examples->addButton("The Shining", []() {
        std::cout << "The Shining Scene Selected" << std::endl;
        top_bottom.loadFromFile("../resources/shining2.png",
                                example.program,
                                "topBottomMap");

    });

    screen->setVisible(true);
    screen->performLayout();
    // nanoguiWindow->center();

    //Setup GUI hooks
    glfwSetCursorPosCallback(renderer.window,
            [](GLFWwindow *, double x, double y) {
            screen->cursorPosCallbackEvent(x, y);
        }
    );

    glfwSetMouseButtonCallback(renderer.window,
        [](GLFWwindow *, int button, int action, int modifiers) {
            screen->mouseButtonCallbackEvent(button, action, modifiers);
        }
    );

    glfwSetKeyCallback(renderer.window,
        [](GLFWwindow *, int key, int scancode, int action, int mods) {
            screen->keyCallbackEvent(key, scancode, action, mods);
        }
    );

    glfwSetCharCallback(renderer.window,
        [](GLFWwindow *, unsigned int codepoint) {
            screen->charCallbackEvent(codepoint);
        }
    );

    glfwSetDropCallback(renderer.window,
        [](GLFWwindow *, int count, const char **filenames) {
            screen->dropCallbackEvent(count, filenames);
        }
    );

    glfwSetScrollCallback(renderer.window,
        [](GLFWwindow *, double x, double y) {
            screen->scrollCallbackEvent(x, y);
       }
    );

    glfwSetFramebufferSizeCallback(renderer.window,
        [](GLFWwindow *, int width, int height) {
            screen->resizeCallbackEvent(width, height);
        }
    );

    //Render loop
    while(renderer.nextFrame()){
        //Clears the buffers
        renderer.begin();

        //Bind the texture
        top_bottom.bind(example.program);

        //Draw GL elements
        example.draw();

        //Uniform update (mostly state related)
        glUniform1i(example.program.uniform("colorDepthToogle"), (int)toggleColorDepth);
        glUniform1i(example.program.uniform("isDOF"), (int)isDepthOfField);
        glUniform1f(example.program.uniform("iDOF"), dofIntensity);
        glUniform1i(example.program.uniform("depthOrder"), (int)depthOrder);
        glUniform1i(example.program.uniform("isColorRetouching"), (int)isColorChanging);
        glUniform1f(example.program.uniform("backgroundIntensity"), backgroundIntensity);
        glUniform1f(example.program.uniform("foregroundIntensity"), foregroundIntensity);
        // glUniform1f(example.program.uniform("time"), renderer.currentTime());

        //Update the gui
        screen->drawContents();
        screen->drawWidgets();

        //Swaps the front and back buffers
        renderer.end();
    }

    //Dispose the GLFW context nicely
    renderer.dispose();

    return 0;
}
