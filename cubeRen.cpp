#include <iostream>
#include <math.h>
#include <csignal>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <omp.h>



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


class Cuboid {

    float data[24] = {};
    float xlen, ylen, zlen;

public:

    const int indices[36] = {
        0, 1, 2, 2, 3, 0, 
        0, 6, 7, 7, 3, 0, 
        4, 5, 6, 6, 7, 4, 
        4, 5, 1, 1, 4, 2, 
        0, 6, 5, 5, 1, 0, 
        3, 2, 4, 4, 7, 3
    };

    const float& lengthX = xlen, &lengthY = ylen, &lengthZ = zlen;
    float colpz[3], colnz[3], colpx[3], colnx[3], colpy[3], colny[3];

    void get_positionVertices(float* data) const {
        std::copy(this->data, this->data+24, data);
    }

    void set_positionVertices(float sideX, float sideY, float sideZ) {
        xlen = sideX, ylen = sideY, zlen = sideZ;
        data[0] = xlen/2, data[1] = ylen/2, data[2] = zlen/2;
        data[3] = data[0], data[4] = data[1], data[5] = -data[2];
        data[6] = data[0], data[7] = -data[1], data[8] = -data[2];
        data[9] = data[0], data[10] = -data[1], data[11] = data[2];
        data[12] = -xlen/2, data[13] = -ylen/2, data[14] = -zlen/2;
        data[15] = data[12], data[16] = -data[13], data[17] = data[14];
        data[18] = data[12], data[19] = -data[13], data[20] = -data[14];
        data[21] = data[12], data[22] = data[13], data[23] = -data[14];
    }

    void rotate_normalx(const float angle) {
        float n0[3] = {1, 0, 0},
              n1[3] = {0, cos(angle), -sin(angle)},
              n2[3] = {0, sin(angle), cos(angle)};
        for (int i = 0; i < 8; i++) {
            float x = data[3*i], y = data[3*i+1], z = data[3*i+2];
            data[3*i] = n0[0]*x+n0[1]*y+n0[2]*z, data[3*i+1] = n1[0]*x+n1[1]*y+n1[2]*z, data[3*i+2] = n2[0]*x+n2[1]*y+n2[2]*z;
        }
    }

    void rotate_normaly(const float angle) {
        float n0[3] = {cos(angle), 0, sin(angle)},
              n1[3] = {0, 1, 0},
              n2[3] = {-sin(angle), 0, cos(angle)};
        for (int i = 0; i < 8; i++) {
            float x = data[3*i], y = data[3*i+1], z = data[3*i+2];
            data[3*i] = n0[0]*x+n0[1]*y+n0[2]*z, data[3*i+1] = n1[0]*x+n1[1]*y+n1[2]*z, data[3*i+2] = n2[0]*x+n2[1]*y+n2[2]*z;
        }
    }

    void rotate_normalz(const float angle) {
        float n0[3] = {cos(angle), -sin(angle), 0},
              n1[3] = {sin(angle), cos(angle), 0},
              n2[3] = {0, 0, 1};
        for (int i = 0; i < 8; i++) {
            float x = data[3*i], y = data[3*i+1], z = data[3*i+2];
            data[3*i] = n0[0]*x+n0[1]*y+n0[2]*z, data[3*i+1] = n1[0]*x+n1[1]*y+n1[2]*z, data[3*i+2] = n2[0]*x+n2[1]*y+n2[2]*z;
        }
    }


    void get_colorVertices(float* data) const {
        std::copy(colpx, colpx+3, data);
        std::copy(colpz, colpz+3, data+3);
        std::copy(colnx, colnx+3, data+6);
        std::copy(colnz, colnz+3, data+9);
        std::copy(colpy, colpy+3, data+12);
        std::copy(colny, colny+3, data+15);
    }

    void reset_positionVertices() {
        this->set_positionVertices(lengthX, lengthY, lengthZ);
    }

};

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

struct MouseInputs {
    bool isCursorEnteredWindow;
    float xpos, ypos;
    int button, action;
} mi;


int fbwidth, fbheight;


void cpcall(GLFWwindow* win, double x, double y) {
    mi.xpos = x, mi.ypos = y;
    // std::cout << "Cursor pos: {" << x << ", " << y << "}\n";
}

void cecall(GLFWwindow* win, int ent) {
    mi.isCursorEnteredWindow = ent ? true: false;
}

void mbcall(GLFWwindow* win, int button, int action, int mods) {
    mi.button = button, mi.action = action;
}

void fbscall(GLFWwindow* win, int width, int height) {
    fbwidth = width, fbheight = height;
}

void wscall(GLFWwindow* win, int w, int h) {
    glViewport(0, 0, fbwidth, fbheight);
}


int main(int argc, char const *argv[]) {

    GLFWwindow *win;

    if (!glfwInit()) return -1;

    // glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    win  = glfwCreateWindow(800, 600, "Cube", nullptr, nullptr);

    if (win == nullptr) {
        glfwTerminate();
        return -1;
    }

    // glfwSetWindowSize(win, 1920, 1080);
    // glfwSetWindowPos(win, 0, 0);

    // glfwSetWindowSizeLimits(win, 1920, 1080, 1920, 1080);
    
    glfwMakeContextCurrent(win);

    glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetInputMode(win, GLFW_STICKY_MOUSE_BUTTONS, 1);
    
    glfwSetCursorPosCallback(win, cpcall);
    glfwSetCursorEnterCallback(win, cecall);
    glfwSetMouseButtonCallback(win, mbcall);
    glfwSetFramebufferSizeCallback(win, fbscall);
    glfwSetWindowSizeCallback(win, wscall);

    if (glewInit() != GLEW_OK) {
        glfwDestroyWindow(win);
        glfwTerminate();
        return -1;
    }
    

    Cuboid c;
    c.set_positionVertices(1, 1, 1);
    c.colnx[0] = 1, c.colnx[1] = 0, c.colnx[2] = 0;
    c.colpx[0] = 0.39, c.colpx[1] = 0, c.colpx[2] = 0;
    c.colny[0] = 0, c.colny[1] = 1, c.colny[2] = 0;
    c.colpy[0] = 0, c.colpy[1] = 0.39, c.colpy[2] = 0;
    c.colnz[0] = 0, c.colnz[1] = 0, c.colnz[2] = 1;
    c.colpz[0] = 0, c.colpz[1] = 0, c.colpz[2] = 0.39;
    // c.originX = -.2, c.originY = .2;

    float posv[24], colv[6*3];
    c.get_positionVertices(posv);
    c.get_colorVertices(colv);

    for (int i = 0; i < 8; i++) {
        std::cout << i  << " -> " << posv[3*i] << ", " << posv[3*i+1] << ", " << posv[3*i+2] << std::endl;
    }

    unsigned int vao, vbo, ibo;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(posv), posv, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*4, nullptr);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(c.indices), c.indices, GL_STATIC_DRAW);

    ShaderCode sc("shaders/cubeshader.glsl", "//Type->");

    unsigned int shader = createShader(sc.parse("fragmentShader"), sc.parse("vertexShader"));
    glUseProgram(shader);

    int planeColLoc[6];
    for (int i=6; i--;) {
        planeColLoc[i] = glGetUniformLocation(shader, std::string("pc.side["+std::to_string(i)+"]").c_str());
        GLcall(glUniform3f(planeColLoc[i], colv[3*i], colv[3*i+1], colv[3*i+2]));
    }

    for (int i=0; i<6; i++) {
        float f[3];
        glGetUniformfv(shader, planeColLoc[i], f);
        std::cout << i << " -> " << f[0] << ", " << f[1] << ", " << f[2] << std::endl;
    }
    std::cout << std::endl;
    float dp[24];

    // float angy = M_PI/2e3, angx = M_PI/2e3, angz = M_PI/6e3;
    float sx, sy;
    bool iss = true;


    while (!glfwWindowShouldClose(win)) {
        #pragma omp parallel
        {
        glClear(GL_COLOR_BUFFER_BIT);
        // glGetBufferSubData(GL_ARRAY_BUFFER, 0, 24*4, dp);
        // for (int i = 0; i < 8; i++) {
        //     std::cout << i  << " -> " << dp[3*i] << ", " << dp[3*i+1] << ", " << dp[3*i+2] << std::endl;
        // }

        // Draw Call
        
        GLcall(glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr));

        
        #pragma omp critical
        {
        if (mi.button == GLFW_MOUSE_BUTTON_LEFT && mi.action == GLFW_PRESS) {
            if (iss) sx = mi.xpos, sy = mi.ypos, iss = false;
            c.reset_positionVertices();
            c.rotate_normaly(-2*M_PI*(mi.xpos-sx)/800.0);
            c.rotate_normalx(-2*M_PI*(mi.ypos-sy)/600.0);
            // c.rotate_normalz(-2*M_PI*(sqrt(mi.xpos*mi.xpos + mi.ypos*mi.ypos)/sqrt(640000+360000)));
            std::cout << "Cursor pos: {" << mi.xpos << ", " << mi.ypos << "}, InSideWindow = " << (mi.isCursorEnteredWindow?"true":"false") << ", O: {" << sx << ", " << sy << "}" << std::endl;
        }
        else if (mi.button == GLFW_MOUSE_BUTTON_LEFT && mi.action == GLFW_RELEASE) iss = true;
        c.get_positionVertices(posv);
        }

        glBufferSubData(GL_ARRAY_BUFFER, 0, 24*4, posv);

        }
        glfwSwapBuffers(win);
        glfwPollEvents();
        
    }
    glfwTerminate();
    return 0;
}
