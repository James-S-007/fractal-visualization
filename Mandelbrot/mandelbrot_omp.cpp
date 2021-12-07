//g++ -std=c++14 mandelbrot_omp.cpp -o3 -lglut -lGL -lGLU -lGLEW -fopenmp -o omp_mandelbrot

#include <fstream>
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <string>
#include <vector>
#include <complex>

#include <GL/glew.h>

#include "mandelbrot_omp.h"

#define MAX_WINDOW_X 1920
#define MAX_WINDOW_Y 1080

namespace omp {

constexpr double eps = 2, umin = -2.2, umax = 0.7, vmin = -1.2, vmax = 1.2;
int m = 500;

int colors[MAX_WINDOW_X][MAX_WINDOW_Y];


void set_color(int color)
{
	if (color == m)
	{
		glColor3f(0, 0, 0);
	}
	else
	{
		glColor3f((double)color / m, (double)color / m, 0);
	}
}

void mandelbrot_set(int width, int height, float zoom, int frame_x, int frame_y)
{
	#pragma omp parallel for
	for (int ix = 0; ix < width; ++ix)
		for (int iy = 0; iy < height; ++iy)
		{
			colors[ix][iy] = get_iterations(ix, iy, width, height, zoom, frame_x, frame_y);
		}
	for (int i = 0; i < width; ++i)
    {
        for (int j = 0; j < height; ++j)
		{
			draw_point(i, j, height, colors[i][j]);
		} 
    } 
}


int find_color(int i, int j, int width, int height) {
	double u0 = get_u0(i, width);
	double v0 = get_v0(j, height);

	int k = find_color(u0, v0);
	return k;
}

void draw_point(int i, int j, int height, int color) {
	glBegin(GL_POINTS);
	set_color(color);
	glVertex2i(i, height - j);
	glEnd();
}

int get_iterations(int i, int j, int width, int height, float zoom, float frame_x, float frame_y) 
{
	float real = ((i / float(width) - 0.5f) * zoom + frame_x) * 5.0;
    float imag = ((j / float(height) - 0.5f) * zoom + frame_y) * 5.0;
 
    int iterations = 0;
    float const_real = real;
    float const_imag = imag;
 
    while (iterations < m) {
        float temp_real = real;
        real = (real * real - imag * imag) + const_real;
        imag = (2.0f * temp_real * imag) + const_imag;
         
        float mag_sq = real * real + imag * imag;
         
        if (mag_sq > 4.0)
        break;
 
        ++iterations;
    }

    return iterations;
}


int find_color(double u0, double v0) {
	int color = -1;
	std::complex<double> zn = std::complex<double>(u0, v0);
	double real = std::abs(zn);
	std::complex<double> c = std::complex<double>(u0, v0);

	while (real < eps && color < m)
	{
		color++;
		zn = zn * zn + c;
		real = std::abs(zn);
	}

	return color;
}

double get_u0(int i, int width) {
	return umin + (i*((umax - umin)) / width);
}

double get_v0(int j, int height) {
	return vmin + (j*((vmax - vmin)) / height);
}

void display(int width, int height, float zoom, int frame_x, int frame_y)
{
	// glViewport(0, 0, width, height);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// gluOrtho2D(0- zoom, width +zoom, 0-zoom, height+zoom);
	glMatrixMode(GL_MODELVIEW);	
	glLoadIdentity();

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glPointSize(1.0);

	mandelbrot_set(width, height, zoom, frame_x, frame_y);
	glFlush();
	// glutPostRedisplay();
}

// void mouse(int button, int state, int x, int y) {
// 	// Wheel reports as button 3(scroll up) and button 4(scroll down)
//    if ((button == 3) || (button == 4)) // It's a wheel event
//    {
//        // Each wheel event reports like a button click, GLUT_DOWN then GLUT_UP
//        if (state == GLUT_UP) return; // Disregard redundant GLUT_UP events
// 	   if (button == 3)
// 	   {
// 			zoom -= 0.005;
// 	   }
// 	   else 
// 	   {
// 			zoom += 0.005;
// 	   }
//    }
//    display();
// }

// void reshape(int w, int h)
// {
// 	width = w; height = h;
// 	glViewport(0, 0, width, height);
	
// 	glMatrixMode(GL_PROJECTION);
// 	glLoadIdentity();
// 	gluOrtho2D(0- zoom, width +zoom, 0-zoom, height+zoom);
// 	glMatrixMode(GL_MODELVIEW);	
// 	glLoadIdentity();

// 	glClearColor(0.0, 0.0, 0.0, 1.0);
// 	glClear(GL_COLOR_BUFFER_BIT);
// 	glPointSize(1.0);

// 	mandelbrot_set();
// }

}  // namespace omp
