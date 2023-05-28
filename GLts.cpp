#include <math.h>
#include <csignal>
#include <iostream>
#include <random>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "parseShader/glscode.hpp"

#define GL_DEBUG

#define normrand() float(rand())/RAND_MAX

#define glAssert(x) if(!(x)) std::raise(SIGINT);
#ifdef GL_DEBUG 
    #define GLcall(x) clearGLERlogs(); \
    x; \
    glAssert(disGLERlog(#x, __FILE__, __LINE__)) 
#else 
    #define GLcall(...) __VA_ARGS__
#endif

inline void clearGLERlogs() {
    while (glGetError());
}

inline bool disGLERlog(const char* funname, const char* filename, size_t line) {
    while (GLenum err = glGetError()) {
        std::cout << "[OpenGL Error: " << line << "] (0x" << std::hex << err << ") " << funname << ' ' << filename << std::endl; 
        return false;
    }
    return true;
}


GLuint compileShader(GLenum type, const GLchar* GLScode) {
    GLuint shid = glCreateShader(type);
    glShaderSource(shid, 1, &GLScode, nullptr);
    glCompileShader(shid);

    GLint res;
    glGetShaderiv(shid, GL_COMPILE_STATUS, &res);
    if (res == GL_FALSE) {
        GLint mlen;
        glGetShaderiv(shid, GL_INFO_LOG_LENGTH, &mlen);
        char* msg = (char*)malloc(mlen);
        glGetShaderInfoLog(shid, mlen, &mlen, msg);

        std::cout << "Failed to compile " << (type==GL_VERTEX_SHADER ? "vertex ": "fragment ")
                  << "shader!\n" << msg << std::endl;
        return 0;
    }

    return shid;
}

GLuint createShader(const GLchar* fragmentShader, const GLchar* vertexShader) {
    GLuint programid = glCreateProgram();

    // std::cout << vertexShader << '\n';
    // std::cout << fragmentShader << '\n';

    GLuint vs = compileShader(GL_VERTEX_SHADER, vertexShader);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(programid, vs);
    glAttachShader(programid, fs);
    glLinkProgram(programid);
    glValidateProgram(programid);

    glDeleteShader(vs);
    glDeleteShader(fs);
    
    return programid;
}


bool isMultiple(GLdouble num, GLdouble divisor, GLdouble tolerance) {
    float remainder = std::fmod(num, divisor);
    return std::fabs(remainder) <= tolerance || std::fabs(remainder - divisor) <= tolerance;
}

inline const GLfloat* randfv(const size_t& nver, const size_t& npos, const GLfloat* color = nullptr) {
    GLfloat* pVers =  new GLfloat[nver*(npos+3)];
    for (size_t i=0; i<nver*(npos+3); i++) {
        size_t li = i%(npos+3);
        pVers[i] = (li<npos ? pow<short>(-1, rand()): 1)*(li<npos ? normrand(): (color==nullptr?normrand():color[li-npos]));
    }
    return pVers;
}

int main() {
    
    GLFWwindow* win;

    if (!glfwInit()) return -1;
    
    win = glfwCreateWindow(800, 600, "OpenGL Testing", nullptr, nullptr);

    if (win == nullptr) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(win);
    // glfwSwapInterval(1);

    glewInit();

    GLfloat pc[] = {
       -.5, -.5, normrand(), normrand(), normrand(),
        .5, -.5, normrand(), normrand(), normrand(),
        .5,  .5, normrand(), normrand(), normrand(),
       -.5,  .5, normrand(), normrand(), normrand()
    };
    // GLfloat pc[] = {
    //    -.5, -.5, .5, .5, .5,
    //     .5, -.5, .5, .5, .5,
    //     .5,  .5, .5, .5, .5
    // //    -.5,  .5, .5, .5, .5
    // };

    GLint ord[] = {0, 1, 2, 2, 0, 3};

    GLfloat color[] = {.5, .3, .7};

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pc), pc, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), 0);
    glEnableVertexAttribArray(0);
    
    GLcall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (const void*)(2*sizeof(GLfloat))));
    GLcall(glEnableVertexAttribArray(1));

    GLuint bufferord;
    glGenBuffers(1, &bufferord);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferord);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6*sizeof(GLint), ord, GL_STATIC_DRAW);

    ShaderCode glscode("tsshader.glsl");

    GLuint shader = createShader(glscode.parse("fragment"), glscode.parse("vertex"));
    glUseProgram(shader);

    // GLint cloc = glGetUniformLocation(shader, "attc");

    // std::cout << glscode.fragment()
    // glBindVertexArray(0);

    // GLboolean rf = GL_TRUE;
 
    GLfloat d[sizeof(pc)/sizeof(GLfloat)];
    // glGetBufferSubData(GL_ARRAY_BUFFER, 0, 15*4, d);
    // for (int i = 0; i < 20; i++) {
    //     if (!(i%5)) std::cout << std::endl;
    //     std::cout << d[i] << ", ";
    // }
    // std::cout << std::endl;

    while (!glfwWindowShouldClose(win)) {

        glClear(GL_COLOR_BUFFER_BIT);

        glGetBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(d), d);
        for (int i = 0; i < sizeof(d)/sizeof(GLfloat); i++) {
            if (!(i%5)) std::cout << std::endl;
            std::cout << d[i] << ", ";
        }
        std::cout << std::endl;


        // glDrawArrays(GL_TRIANGLES, 0, 3);
        // glUniform4f(cloc, color[0], color[1], color[2], color[3]);
        GLcall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

        glfwSwapBuffers(win);
        glfwPollEvents();
        // usleep(1e6);
        
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(pc), randfv(4, 2, color));
    }
    glDeleteProgram(shader);
    glfwTerminate();
    return 0;
}