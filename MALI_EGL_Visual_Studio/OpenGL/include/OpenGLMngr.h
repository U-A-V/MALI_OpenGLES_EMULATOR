#pragma once
#ifndef __OPENGL_MNGR_H__
#define __OPENGL_MNGR_H__
#include <iostream>
#include <GLES2/gl2.h>
#define ASSERT(x) if(!(x)) __debugbreak();
#define GLCall(x) x;\
				ASSERT(CheckGLError(#x, __FILE__, __LINE__))

bool CheckGLError(const char* func, const char* file, int line) {
	bool res = true;
	while (GLenum const err = glGetError()) {
		switch (err) {
		case GL_INVALID_ENUM:
			std::cout << "[OpenGL ERRROR] GL_INVALID_ENUM : " << func << " | " << file << ": " << line << std::endl;
			res = false;
			break;
		case GL_INVALID_VALUE:
			std::cout << "[OpenGL ERRROR] GL_INVALID_VALUE : " << func << " | " << file << ": " << line << std::endl;
			res = false;
			break;
		case GL_INVALID_OPERATION:
			std::cout << "[OpenGL ERRROR] GL_INVALID_OPERATION : " << func << " | " << file << ": " << line << std::endl;
			res = false;
			break;
		case GL_OUT_OF_MEMORY:
			std::cout << "[OpenGL ERRROR] GL_OUT_OF_MEMORY : " << func << " | " << file << ": " << line << std::endl;
			res = false;
			break;
		default:
			std::cout << "[OpenGL ERRROR] " << err << ": " << func << " | " << file << ": " << line << std::endl;
			res = false;
			break;
		}
	}
		return res;
}
#endif