#ifndef _SHADER_LOADER_H_
#define _SHADER_LOADER_H_

#include <iostream>
#include <fstream>
#include <string>

class Shader {
public:
    
    //Raw strings for the shaders
	std::string vert;
	std::string frag;

    //Constructor
	Shader();

	//Main method to load GLSL files
	void load(const char *vertex_path,
	          const char *frag_path);
private:

    //Chuncks for util stuff
    std::string utils;

    //Method to read the GLSL src files
	std::string readFile(const char* filePath);
};

#endif
