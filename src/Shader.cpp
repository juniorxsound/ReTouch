#include "Shader.h"

Shader::Shader() {}

//Main method to load GLSL files
void Shader::load(const char *vertex_path,
          const char *frag_path) {

    //Read the current GLSL shaders and append utils to them
    vert =  readFile(vertex_path);
    frag =  readFile(frag_path);
}

//Method to read the GLSL src files
std::string Shader::readFile(const char* filePath) {
    //Temp contents of the file
    std::string src;

    //Open the stream
    std::ifstream fileStream(filePath, std::ios::in);

    if (!fileStream.is_open()) {
        std::cerr << "File " << filePath << " doesn't exist" << std::endl;
        return "";
    }

    //Read line by line
    std::string line = "";
    while (!fileStream.eof()) {
        std::getline(fileStream, line);
        src.append(line + "\n");
    }

    fileStream.close();
    return src;
}
