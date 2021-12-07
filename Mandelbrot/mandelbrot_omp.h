#pragma once

//g++ -std=c++14 mandelbrot_omp.cpp -o3 -lglut -lGL -lGLU -lGLEW -fopenmp -o omp_mandelbrot

#include <fstream>
#include <iostream>
#include <stdio.h>
// #include <GL/glut.h>
#include <sstream>
#include <string>
#include <vector>
#include <complex>

namespace omp {


// GLuint window;
// GLuint width = 720, height = 720;
// GLuint position_x = 100, position_y = 100;



double get_u0(int i);
double get_v0(int j);
int find_color(int i, int j, int width, int height);
int find_color(double u0, double v0);
void draw_point(int i, int j, int k);
int get_iterations(int i, int j);

void set_color(int color);
void mandelbrot_set(int width, int height, float zoom, int frame_x, int frame_y);
int find_color(int i, int j);
void draw_point(int i, int j, int height, int color);
int get_iterations(int i, int j, int width, int height, float zoom, float frame_x, float frame_y);
int find_color(double u0, double v0);
double get_u0(int i, int width);
double get_v0(int j, int height);
void display(int width, int height, float zoom, int frame_x, int frame_y);
// void mouse(int button, int state, int x, int y);
// void reshape(int w, int h);

}  // namespace omp
