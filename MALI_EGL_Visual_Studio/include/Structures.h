#pragma once

#ifndef __STRUCTURES_H__
#define __STRUCTURES_H__
#include <stdint.h>
#include <EGL/egl.h>

struct esContext {
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
};

#endif