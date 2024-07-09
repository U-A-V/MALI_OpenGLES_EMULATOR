#include <windows.h>
#include <iostream>
#include <time.h>
#include <Structures.h>
#include <ShaderMngr.h>

#define DEG_TO_RAD 0.017453f


LRESULT CALLBACK wdnproc(HWND, UINT, WPARAM, LPARAM);
void RenderTriangle(GLuint shaderProgram) noexcept {
    // Define vertices of a colored triangle
    GLfloat vertices[] = {
        0.0f,  0.5f,  0.0f,  // Vertex 0 (x, y, z)
       -0.5f, -0.5f,  0.0f,  // Vertex 1 (x, y, z)
        0.5f, -0.5f,  0.0f   // Vertex 2 (x, y, z)
    };

    // Define colors for each vertex
    GLfloat colors[] = {
        1.0f, 0.0f, 0.0f,  // Red
        0.0f, 1.0f, 0.0f,  // Green
        0.0f, 0.0f, 1.0f   // Blue
    };

    // Create and bind vertex and color buffers
    GLuint vertexBuffer, colorBuffer;
    GLCall(glGenBuffers(1, &vertexBuffer));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer));
    GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

    GLCall(glGenBuffers(1, &colorBuffer));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, colorBuffer));
    GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW));


    // Specify attribute locations and enable vertex arrays
    GLint posAttrib = GLCall(glGetAttribLocation(shaderProgram, "vPosition"));
    GLCall(glEnableVertexAttribArray(posAttrib));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer));
    GLCall(glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0));

    GLint colorAttrib = GLCall(glGetAttribLocation(shaderProgram, "vColor"));
    GLCall(glEnableVertexAttribArray(colorAttrib));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, colorBuffer));
    GLCall(glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0));

    // Draw the triangle
    GLCall(glClear(GL_COLOR_BUFFER_BIT));
    GLCall(glDrawArrays(GL_TRIANGLES, 0, 3));

    // Cleanup
    GLCall(glDisableVertexAttribArray(posAttrib));
    GLCall(glDisableVertexAttribArray(colorAttrib));
    GLCall(glDeleteBuffers(1, &vertexBuffer));
    GLCall(glDeleteBuffers(1, &colorBuffer));
}

HWND CreateWnd(const char *title) {
    HINSTANCE hInst = GetModuleHandle(NULL);
    // Register the window class.
    std::cout << hInst << std::endl;
    WNDCLASSA wc = { 0 };

    wc.lpfnWndProc = wdnproc;
    wc.hInstance = hInst;
    wc.lpszClassName = title;
    wc.style = CS_OWNDC;

    if (!RegisterClassA(&wc))
    {
        std::cout << "Class creation failed " << GetLastError() << std::endl;
    }

    // Create the window.

    HWND hwnd = CreateWindowA(
        // Optional window styles.
        title,        // Window class
        title, // Window text
       // WS_OVERLAPPEDWINDOW,        // Window style
        WS_POPUP | WS_CAPTION | WS_VISIBLE | WS_SYSMENU,
        // Size and position
        10, 10, 600, 600,

        NULL,  // Parent window
        NULL,  // Menu
        hInst, // Instance handle
        NULL   // Additional application data
    );
    if (hwnd == NULL)
    {
        std::cout << "Window creation failed " << GetLastError() << std::endl;
        return 0;
    }
    std::cout << "Window created" << std::endl;

    return hwnd;

}

esContext CreateESContext(HWND hwnd) {
    EGLDisplay display = eglGetDisplay(GetDC(hwnd));
    eglInitialize(display, NULL, NULL);

    // Create EGL window surface
    EGLConfig config;
    EGLSurface surface;
    EGLContext context;

    // Configure EGL attributes as needed
    const EGLint configAttributes[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        // Add more attributes as needed
        EGL_NONE
    };

    EGLint numConfig;
    eglChooseConfig(display, configAttributes, &config, 1, &numConfig);
    EGLint contextAttributes[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
    surface = eglCreateWindowSurface(display, config, (EGLNativeWindowType)hwnd, NULL);
    context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttributes);
    return { display, surface, context };

}

bool eglMakeCurrCtx(esContext& egl) {
    if (eglMakeCurrent(egl.display, egl.surface, egl.surface, egl.context) != EGL_TRUE) {
        EGLint error = eglGetError();
        // Handle the error appropriately
        // Common errors include EGL_BAD_DISPLAY, EGL_BAD_SURFACE, EGL_BAD_CONTEXT
        switch (error) {
        case EGL_BAD_DISPLAY:
            std::cout << "EGL_BAD_DISPLAY: " << error << std::endl;
            // Invalid display
            break;
        case EGL_BAD_SURFACE:
            // Invalid surface
            std::cout << "EGL_BAD_SURFACE: " << error << std::endl;
            break;
        case EGL_BAD_CONTEXT:
            // Invalid context
            std::cout << "EGL_BAD_CONTEXT: " << error << std::endl;
            break;
        case EGL_SUCCESS:
            // Invalid context
            std::cout << "EGL_SUCCESS: " << error << std::endl;
            break;
        case EGL_BAD_NATIVE_WINDOW:
            std::cout << "EGL_BAD_NATIVE_WINDOW: " << error << std::endl;
            break;
        default:
            // Handle any other errors
            std::cout << "don't know why: " << error << std::endl;
            break;

        };
        return false;
    }
    return true;
}

GLuint shaderProgram;

DWORD WINAPI SpawnDarwThreadFunction(LPVOID param) {
    esContext egl = *(esContext*)param;
    eglMakeCurrCtx(egl);
    // Load and compile shaders
    const char* vertexShaderSource =
        "attribute vec4 vPosition;\n"
        "attribute vec4 vColor;\n"
        "varying vec4 color;\n"
        "void main() {\n"
        "  gl_Position = vPosition;\n"
        "  color = vColor;\n"
        "}\n";

    const char* fragmentShaderSource =
        "precision mediump float;\n"
        "varying vec4 color;\n"
        "void main() {\n"
        "  gl_FragColor = color;\n"
        "}\n";

    GLuint shaderProgram = CreateShader(vertexShaderSource, fragmentShaderSource);
    while (true) {
        RenderTriangle(shaderProgram);
        eglSwapBuffers(egl.display, egl.surface);
    }
    GLCall(glDeleteProgram(shaderProgram));
    return 0;
}

void SpawnDarwThread(esContext* egl, void* funcptr) {
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)funcptr, &*egl, 0, NULL);
    return;
}

int main() {
    esContext egl1;

    HWND hwnd = CreateWnd("W");
    ShowWindow(hwnd, TRUE);
    egl1 = CreateESContext(hwnd);
    void *SpawnFunction = SpawnDarwThreadFunction;
    SpawnDarwThread(&egl1, SpawnFunction);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    // Clean up EGL
    return 0;
}

LRESULT CALLBACK wdnproc(HWND h, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
    case WM_CREATE:
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_CHAR:
        break;
    default:
        return DefWindowProc(h, msg, wp, lp);
    }
}