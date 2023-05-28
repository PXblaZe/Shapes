
#define GLEW_STATIC

#include <math.h>
#include <chrono>
#include <unistd.h>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

bool isMultiple(GLdouble num, GLdouble divisor, GLdouble tolerance) {
    float remainder = std::fmod(num, divisor);
    return std::fabs(remainder) <= tolerance || std::fabs(remainder - divisor) <= tolerance;
}

void initClock() {
    for (GLdouble i = .0; i<2*M_PI; i+=M_PI/30) {
        GLboolean isHp = isMultiple(i, M_PI/6, 1e-6);
        GLdouble ci = isHp ? .02:.0025, bl = isHp ? .9:.95;
        glVertex2d(cos(i), sin(i));
        glVertex2d(bl*cos(i), bl*sin(i));
        glVertex2d(cos(i+ci), sin(i+ci));
        glVertex2d(cos(i+ci), sin(i+ci));
        glVertex2d(bl*cos(i), bl*sin(i));
        glVertex2d(bl*cos(i+ci), bl*sin(i+ci));
    }

    for(GLdouble i = .0; i<=2*M_PI; i+=1e-3) {
        glVertex2d(5e-2*cos(i), 5e-2*sin(i));
        glVertex2d(0, 0);
        glVertex2d(5e-2*cos(i + M_PI/30), 5e-2*sin(i + M_PI/30));
    }
}

void clock_needle(const char& mode, const GLdouble& l, const GLdouble& r, const GLdouble& angle, GLdouble head = 0.0) {
    GLdouble x = 0.0;

    if (mode == 's' || mode == 'S') x = 0.002;
    else if (mode == 'm' || mode == 'M') x = 2e-2;
    else if (mode == 'h' || mode == 'H') x = 2.6e-2;
    else return;

    GLdouble dl = sqrtf64(l*l + x*x), dr = sqrtf64(r*r + x*x);
    GLdouble pl = atanf64(x/l), pr = atanf64(x/r);
    GLdouble Points[4][2] = {
        {dl*cosf64(angle-pl), dl*sinf64(angle-pl)}, 
        {dl*cosf64(angle+pl), dl*sinf64(angle+pl)}, 
        {dr*cosf64(M_PI+angle-pr), dr*sinf64(M_PI+angle-pr)}, 
        {dr*cosf64(M_PI+angle+pr), dr*sinf64(M_PI+angle+pr)}
    };
    glVertex2d(Points[2][0], Points[2][1]), glVertex2d(Points[0][0], Points[0][1]), glVertex2d(Points[1][0], Points[1][1]);  
    glVertex2d(Points[2][0], Points[2][1]), glVertex2d(Points[3][0], Points[3][1]), glVertex2d(Points[0][0], Points[0][1]);

    if (r != 0) {
        glVertex2d(3*r*cos(angle+M_PI), 3*r*sin(angle+M_PI));
        glVertex2d(r*cos(angle+M_PI), r*sin(angle+M_PI));
        glVertex2d(3*r*cos(angle+M_PI + 50*x), 3*r*sin(angle+M_PI + 50*x));
        glVertex2d(3*r*cos(angle+M_PI + 50*x), 3*r*sin(angle+M_PI + 50*x));
        glVertex2d(r*cos(angle+M_PI), r*sin(angle+M_PI));
        glVertex2d(r*cos(angle+M_PI+ 50*x), r*sin(angle+M_PI + 50*x));

        glVertex2d(3*r*cos(angle+M_PI), 3*r*sin(angle+M_PI));
        glVertex2d(r*cos(angle+M_PI), r*sin(angle+M_PI));
        glVertex2d(3*r*cos(angle+M_PI - 50*x), 3*r*sin(angle+M_PI - 50*x));
        glVertex2d(3*r*cos(angle+M_PI - 50*x), 3*r*sin(angle+M_PI - 50*x));
        glVertex2d(r*cos(angle+M_PI), r*sin(angle+M_PI));
        glVertex2d(r*cos(angle+M_PI- 50*x), r*sin(angle+M_PI - 50*x));
    }

    if (head == 0.0) return;
    glVertex2d(Points[0][0], Points[0][1]);
    glVertex2d(Points[1][0], Points[1][1]);
    glVertex2d((l+head)*cos(angle), (l+head)*sin(angle));
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(800, 600, "Clock", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glewInit();

    // std::cout << glGetString(GL_VERSION) << std::endl;

    GLuint buffer[10];
    glGenBuffers(10, buffer);

    // for (GLuint b: buffer) std::cout << b << ' ';
    // std::endl;

    /* Loop until the user closes the window */

    time_t current_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm* ptm = std::localtime(&current_time);

    GLdouble s = 2*M_PI*ptm->tm_sec/60, m = 2*M_PI*(ptm->tm_min + ptm->tm_sec/60.0)/60.0, h = 2*M_PI*(ptm->tm_hour%12 + ptm->tm_min/60.0 + ptm->tm_sec/3600.0)/12.0;
    constexpr const GLdouble inc = M_PI/30, z = M_PI/2;

    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glBegin(GL_TRIANGLES);

        initClock();

        clock_needle('s', .9, .1, z-s);
        clock_needle('m', .8, 0,  z-m, .04);
        clock_needle('h', .65, 0, z-h, .05);
        s += inc;
        m += inc/60;
        h += inc/720;

        glEnd();   

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();

        usleep(1e6);
    }

    glfwTerminate();
    return 0;
}