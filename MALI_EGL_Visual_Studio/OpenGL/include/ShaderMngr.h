#pragma once
#ifndef __SHADER_MNGR_H__
#define __SHADER_MNGR_H__
#include <OpenGLMngr.h>
GLuint CreateShader(const char* vertSrc, const char* fragSrc) {
    GLuint const vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLCall(glShaderSource(vertexShader, 1, &vertSrc, NULL));
    GLCall(glCompileShader(vertexShader));

    GLuint const fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    GLCall(glShaderSource(fragmentShader, 1, &fragSrc, NULL));
    GLCall(glCompileShader(fragmentShader));

    // Link shaders into a program
    GLuint const shaderProgram = glCreateProgram();
    GLCall(glAttachShader(shaderProgram, vertexShader));
    GLCall(glAttachShader(shaderProgram, fragmentShader));
    GLCall(glLinkProgram(shaderProgram));
    GLCall(glUseProgram(shaderProgram));

    GLCall(glDeleteShader(vertexShader));
    GLCall(glDeleteShader(fragmentShader));
    return shaderProgram;
}
#endif