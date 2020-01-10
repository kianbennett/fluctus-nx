#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <switch.h>

#include <EGL/egl.h>    // EGL library
#include <EGL/eglext.h> // EGL extensions
#include <glad/glad.h>  // glad library (OpenGL loader)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "level.h"

bool initEgl(NWindow* win);
void deinitEgl();
void setMesaConfig();
void createShader();
GLuint createAndCompileShader(GLenum type, const char* source);

EGLDisplay eglDisplay;
EGLContext eglContext;
EGLSurface eglSurface;

GLuint shaderProgram;

Level level;

u64 lastFrame = 0;
bool isPaused = false;

const char* const vertexShaderSource = R"text(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    uniform mat4 projection, model;
    void main()
    {
        gl_Position = projection * model * vec4(aPos, 1.0);
    }
)text";

const char* const fragmentShaderSource = R"text(
    #version 330 core
    uniform vec4 color;
    out vec4 fragColor;
    void main()
    {
        fragColor = vec4(color);
    }
)text";

int main(int argc, char* argv[]) {
    // Set mesa configuration (useful for debugging)
    setMesaConfig();

    // Initialise nxlink connection (for reading console)
    socketInitializeDefault();
    nxlinkStdio();

    // Intialise romfs for file handling
    // Result rc = romfsInit();
    // if (R_FAILED(rc)) printf("romfsInit: %08X\n", rc);

    printf("Starting app...\n");

    // Initialize EGL on the default window
    if (!initEgl(nwindowGetDefault()))
        return EXIT_FAILURE;

    // Load OpenGL routines using glad
    gladLoadGL();
    // Load shader from source and set projection matrix
    createShader();

    level.init();

    while (appletMainLoop()) {
        // Get delta time
        u64 t = 0;
        appletGetProgramTotalActiveTime(&t);
        float deltaTime = (t - lastFrame) * 0.000000001f; // t is in nanoseconds, convert to seconds
        if(isPaused) deltaTime = 0;
        lastFrame = t;

        // Get and process input
        hidScanInput();
        u32 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
        if (kDown & KEY_PLUS) break; // Exit app
        if (kDown & KEY_MINUS) isPaused = !isPaused;
        if (kDown & KEY_A || kDown & KEY_B || kDown & KEY_X || kDown & KEY_Y) level.player->flip();

        // Clear OpenGL colour buffer
        glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        level.update(deltaTime);
        level.render(shaderProgram);

        eglSwapBuffers(eglDisplay, eglSurface);
    }
    
    glDeleteProgram(shaderProgram);
    deinitEgl();
    socketExit();
    level.cleanup();
    return EXIT_SUCCESS;
}

bool initEgl(NWindow* win) {
    // Connect to the EGL default display
    eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (!eglDisplay) {
        printf("Could not connect to display! error: %d\n", eglGetError());
        return false;
    }

    // Initialize the EGL display connection
    eglInitialize(eglDisplay, nullptr, nullptr);

    // Select OpenGL (Core) as the desired graphics API
    if (eglBindAPI(EGL_OPENGL_API) == EGL_FALSE) {
        printf("Could not set API! error: %d\n", eglGetError());
        eglTerminate(eglDisplay);
        eglDisplay = nullptr;
    }

    // Get an appropriate EGL framebuffer configuration
    EGLConfig config;
    EGLint numConfigs;
    static const EGLint framebufferAttributeList[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
        EGL_RED_SIZE,     8,
        EGL_GREEN_SIZE,   8,
        EGL_BLUE_SIZE,    8,
        EGL_ALPHA_SIZE,   8,
        EGL_DEPTH_SIZE,   24,
        EGL_STENCIL_SIZE, 8,
        EGL_NONE
    };
    eglChooseConfig(eglDisplay, framebufferAttributeList, &config, 1, &numConfigs);
    if (numConfigs == 0) {
        printf("No config found! error: %d\n", eglGetError());
        eglTerminate(eglDisplay);
        eglDisplay = nullptr;
    }

    // Create an EGL window surface
    eglSurface = eglCreateWindowSurface(eglDisplay, config, win, nullptr);
    if (!eglSurface) {
        printf("Surface creation failed! error: %d\n", eglGetError());
        eglTerminate(eglDisplay);
        eglDisplay = nullptr;
    }

    // Create an EGL rendering context
    static const EGLint contextAttributeList[] = {
        EGL_CONTEXT_OPENGL_PROFILE_MASK_KHR, EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT_KHR,
        EGL_CONTEXT_MAJOR_VERSION_KHR, 4,
        EGL_CONTEXT_MINOR_VERSION_KHR, 3,
        EGL_NONE
    };
    eglContext = eglCreateContext(eglDisplay, config, EGL_NO_CONTEXT, contextAttributeList);
    if (!eglContext) {
        printf("Context creation failed! error: %d\n", eglGetError());
        eglDestroySurface(eglDisplay, eglSurface);
        eglSurface = nullptr;
    }

    // Connect the context to the surface
    eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);
    return true;
}

void deinitEgl() {
    if (eglDisplay) {
        eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (eglContext) {
            eglDestroyContext(eglDisplay, eglContext);
            eglContext = nullptr;
        }
        if (eglSurface) {
            eglDestroySurface(eglDisplay, eglSurface);
            eglSurface = nullptr;
        }
        eglTerminate(eglDisplay);
        eglDisplay = nullptr;
    }
}

void setMesaConfig() {
    // Uncomment below to disable error checking and save CPU time (useful for production):
    //setenv("MESA_NO_ERROR", "1", 1);

    // Uncomment below to enable Mesa logging:
    //setenv("EGL_LOG_LEVEL", "debug", 1);
    //setenv("MESA_VERBOSE", "all", 1);
    //setenv("NOUVEAU_MESA_DEBUG", "1", 1);

    // Uncomment below to enable shader debugging in Nouveau:
    //setenv("NV50_PROG_OPTIMIZE", "0", 1);
    //setenv("NV50_PROG_DEBUG", "1", 1);
    //setenv("NV50_PROG_CHIPSET", "0x120", 1);
}

GLuint createAndCompileShader(GLenum type, const char* source) {
    GLint success;
    GLchar msg[512];

    GLuint handle = glCreateShader(type);
    if (!handle) {
        printf("%u: cannot create shader\n", type);
        return 0;
    }
    glShaderSource(handle, 1, &source, nullptr);
    glCompileShader(handle);
    glGetShaderiv(handle, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(handle, sizeof(msg), nullptr, msg);
        printf("%u: %s\n", type, msg);
        glDeleteShader(handle);
        return 0;
    }

    return handle;
}

void createShader() {
    GLint vsh = createAndCompileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLint fsh = createAndCompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vsh);
    glAttachShader(shaderProgram, fsh);
    glLinkProgram(shaderProgram);

    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char buf[512];
        glGetProgramInfoLog(shaderProgram, sizeof(buf), nullptr, buf);
        printf("Link error: %s\n", buf);
    }
    glDeleteShader(vsh);
    glDeleteShader(fsh);

    // Create projection matrix and apply to shader
    glm::mat4 projection = glm::ortho(0.0f, 1280.0f, 720.0f, 0.0f, -1.0f, 1.0f);
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
}