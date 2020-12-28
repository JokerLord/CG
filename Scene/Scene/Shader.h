#pragma once
#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

class Shader
{
public:
    unsigned int prog;
    Shader(const char* vertPath, const char* fragPath)
    {
        std::string vtmp, ftmp;
        std::ifstream vFile, fFile;
        vFile.open(vertPath);
        fFile.open(fragPath);
        std::stringstream vStream, fStream;
        vStream << vFile.rdbuf();
        fStream << fFile.rdbuf();
        vFile.close();
        fFile.close();
        vtmp = vStream.str();
        ftmp = fStream.str();
        const char *vCode = vtmp.c_str();
        const char *fCode = ftmp.c_str();

        unsigned int vertShader, fragShader;
        vertShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertShader, 1, &vCode, NULL);
        glCompileShader(vertShader);

        fragShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragShader, 1, &fCode, NULL);
        glCompileShader(fragShader);

        prog = glCreateProgram();
        glAttachShader(prog, vertShader);
        glAttachShader(prog, fragShader);
        glLinkProgram(prog);
        glDeleteShader(vertShader);
        glDeleteShader(fragShader);
    }
    void use()
    {
        glUseProgram(prog);
    }
};

