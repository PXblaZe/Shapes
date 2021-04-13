#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>

#define clrscr() printf("\e[H\e[J")
#define pi acos(-1.0)

typedef struct define{
    int len, p1[2], p2[2], p3[2], p4[2];
}Cube;

void display(Cube cube, int latency) {
    clrscr();
    int _, __, lp;
    lp = (cube.p1[0]>=cube.p2[0]&&cube.p1[0]>cube.p3[0]&&cube.p1[0]>cube.p4[0])?cube.p1[0]:
        ((cube.p2[0]>cube.p3[0]&&cube.p2[0]>cube.p4[0])?cube.p2[0]:((cube.p3[0]>=cube.p4[0])?cube.p3[0]:cube.p4[0]));
    for (_=1; _<cube.p1[1]+cube.len; _++) {
        if (_ < cube.p4[1]);
        else {
            double m;
            int y1, x2, hw;
            if (_<=cube.p1[1]) {
                if (_==cube.p4[1]) {
                    if (cube.p3[0]<cube.p4[0]) {
                        for (__ = 1; __<cube.p4[0]; __++) {
                            if (__<cube.p3[0]) printf(" ");
                            else printf("*");
                        }
                    } else {
                        for (__ = 1; __<cube.p3[0]; __++) {
                            if (__<cube.p4[0]) printf(" ");
                            else printf(".");
                        }
                    }
                } else if (_==cube.p1[1]) {
                    if (cube.p1[0]<cube.p2[0]) {
                        for (__ = 1; __<cube.p2[0]; __++) {
                            if (__<cube.p1[0]) printf(" ");
                            else printf("*");
                        }
                    } else {
                        for (__ = 1; __<cube.p1[0]; __++) {
                            if (__<cube.p2[0]) printf(" ");
                            else printf(".");
                        }
                    }
                } else {
                    if (cube.p4[0]!=cube.p1[0]) {
                        m = (double)(cube.p4[1]-cube.p1[1])/(cube.p4[0]-cube.p1[0]);
                        hw = abs((cube.p3[0]-cube.p2[0])/2);
                        for (__=1; __<=lp; __++) {
                            y1 = round((m*__)-(m*cube.p1[0])+cube.p1[1]);
                            x2 = round((_-cube.p3[1])/m + cube.p3[0]);
                            if (y1 == (round((m*(__-1))-(m*cube.p1[0])+cube.p1[1]))) y1 = -1;
                            if ((__==cube.p4[0]&&cube.p4[0]>cube.p3[0])||(__==cube.p3[0]&&cube.p4[0]>cube.p3[0])
                                ||(_==y1&&__<=(hw+cube.p1[0]))||(__==x2&&__<=(hw+cube.p2[0]))) printf("*");
                            else if ((__==cube.p4[0]&&cube.p4[0]<=cube.p3[0])||(__==cube.p3[0]&&cube.p4[0]<=cube.p3[0])
                                    ||(_==y1&&__>(hw+cube.p1[0]))||(__==x2&&__>(hw+cube.p2[0]))) printf(".");
                            else printf(" ");
                        }
                    } else {
                        hw = (cube.p1[1]-cube.p4[1])/2;
                        if (cube.p4[0]<cube.p3[0]) {
                            for (__=1; __<=lp; __++) {
                                if ((_<=hw+cube.p4[1])&&(__==cube.p3[0]||__==cube.p4[0])) printf(".");
                                else if ((_>hw+cube.p4[1])&&(__==cube.p3[0]||__==cube.p4[0])) printf("*");
                                else printf(" ");
                            }
                        } else {
                            for (__=1; __<=lp; __++) {
                                if ((__==cube.p3[0]||__==cube.p4[0])) printf("*");
                                else printf(" ");
                            }
                        }
                    }
                    
                }
            } else if (_<cube.p4[1]+cube.len-1) {
                for (__=1; __<=lp;__++) {
                    if ((__==cube.p1[0]&&cube.p1[0]<=cube.p2[0])||(__==cube.p3[0]&&cube.p3[0]<=cube.p4[0])
                        ||(__==cube.p2[0]&&cube.p1[0]<=cube.p2[0])||(__==cube.p4[0]&&cube.p3[0]<=cube.p4[0]))
                        printf("*");
                    else if ((__==cube.p1[0]&&cube.p1[0]>=cube.p2[0])||(__==cube.p3[0]&&cube.p3[0]>=cube.p4[0])
                        ||(__==cube.p2[0]&&cube.p1[0]>=cube.p2[0])||(__==cube.p4[0]&&cube.p3[0]>=cube.p4[0]))
                        printf(".");
                    else printf(" ");
                }
            } else {
                if (_==cube.p4[1]+cube.len-1) {
                    if (cube.p3[0]<cube.p4[0]) {
                        for (__ = 1; __<cube.p4[0]; __++) {
                            if (__<cube.p3[0]) printf(" ");
                            else printf("*");
                        }
                    } else {
                        for (__ = 1; __<cube.p3[0]; __++) {
                            if (__<cube.p4[0]) printf(" ");
                            else printf(".");
                        }
                    }
                } else if (_==cube.p1[1]+cube.len-1) {
                    if (cube.p1[0]<cube.p2[0]) {
                        for (__ = 1; __<cube.p2[0]; __++) {
                            if (__<cube.p1[0]) printf(" ");
                            else printf("*");
                        }
                    } else {
                        for (__ = 1; __<cube.p1[0]; __++) {
                            if (__<cube.p2[0]) printf(" ");
                            else printf(".");
                        }
                    }
                } else {
                    if (cube.p4[0]!=cube.p1[0]) {
                        m = (double)(cube.p4[1]-cube.p1[1])/(cube.p4[0]-cube.p1[0]);
                        hw = abs((cube.p3[0]-cube.p2[0])/2);
                        for (__=1; __<=lp; __++) {
                            y1 = round((m*__)-(m*cube.p1[0])+cube.p1[1]+cube.len-1);
                            x2 = round((_-cube.p3[1]-cube.len+1)/m + cube.p3[0]);
                            if (y1 == (round((m*(__-1))-(m*cube.p1[0])+cube.p1[1]+cube.len-1))) y1 = -1;
                            if ((__==cube.p1[0]&&cube.p2[0]>cube.p1[0])||(__==cube.p2[0]&&cube.p2[0]>cube.p1[0])
                                ||(_==y1&&__<=(hw+cube.p1[0]))||(__==x2&&__<=(hw+cube.p2[0]))) printf("*");
                            else if ((__==cube.p1[0]&&cube.p2[0]<=cube.p1[0])||(__==cube.p2[0]&&cube.p2[0]<=cube.p1[0])
                                    ||(_==y1&&__>(hw+cube.p1[0]))||(__==x2&&__>(hw+cube.p2[0]))) printf(".");
                            else printf(" ");
                        }
                    } else {
                        hw = (cube.p1[1]-cube.p4[1])/2;
                        if (cube.p4[0]<cube.p3[0]) {
                            for (__=1; __<=lp; __++) {
                                if ((_<=hw+cube.p4[1]+cube.len-1)&&(__==cube.p3[0]||__==cube.p4[0])) printf(".");
                                else if ((_>hw+cube.p4[1]+cube.len-1)&&(__==cube.p3[0]||__==cube.p4[0])) printf("*");
                                else printf(" ");
                            }
                        } else {
                            for (__=1; __<=lp; __++) {
                                if ((__==cube.p3[0]||__==cube.p4[0])) printf("*");
                                else printf(" ");
                            }
                        }
                    }
                }
            }
        } printf("\n");
    } usleep(latency*1000);
}

Cube cube(int length, short view, double x_i, double z_i) {
    Cube cube;
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    cube.len = length;
    double Z[2] = {-z_i, x_i};
    int plen, xyCentre[2] = {w.ws_col/2, w.ws_row/2};
    plen = (int)(round(sqrt(2)*cube.len*cos((x_i>=0&&z_i>=0)?atan(z_i/x_i):(z_i<=0&&x_i>0)?(2*pi+atan(z_i/x_i)):(pi+atan(z_i/x_i))-(pi/4))));
    if (plen%2) cube.p1[0] = xyCentre[0]-(plen/2);
    else cube.p1[0] = xyCentre[0]-(plen/2)+1;
    cube.p1[1] = xyCentre[1]-cube.len/2+view/2;
    cube.p2[0] = (int)round(cube.p1[0]+cube.len*Z[1]);
    cube.p2[1] = cube.p1[1];
    cube.p3[0] = (int)round(cube.p2[0]+cube.len*z_i);
    cube.p3[1] = cube.p1[1]-view;
    cube.p4[0] = (int)round(cube.p3[0]-(cube.len*Z[1]));
    cube.p4[1] = cube.p3[1];
    return cube; 
}

/*
void main() {
    // For rotating cube 2 times around Y-axis.
    Cube c;
    for (double i = 0; i<=10*pi; i+=(pi/64)) {
        c = cube(30, 5, cos(i), sin(i));
        display(c, 30);
    }
}
*/
