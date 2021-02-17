#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>

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
    void set_int(const std::string& name, int value) const
    {
        glUniform1i(glGetUniformLocation(prog, name.c_str()), value);
    }
    void set_float(const std::string& name, float value) const
    {
        glUniform1f(glGetUniformLocation(prog, name.c_str()), value);
    }
    void set_vec3(const std::string& name, const glm::vec3& value) const
    {
        glUniform3fv(glGetUniformLocation(prog, name.c_str()), 1, &value[0]);
    }
    void set_vec3(const std::string& name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(prog, name.c_str()), x, y, z);
    }
    void set_mat4(const std::string& name, const glm::mat4& mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(prog, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
};

