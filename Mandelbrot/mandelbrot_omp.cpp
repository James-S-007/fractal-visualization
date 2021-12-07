#include <GL/glew.h>

#include "mandelbrot_omp.h"

#define MAX_WINDOW_X 1920
#define MAX_WINDOW_Y 1080
#define MAX_ITERATIONS 500

namespace omp {

constexpr double eps = 2, umin = -2.2, umax = 0.7, vmin = -1.2, vmax = 1.2;
int colors[MAX_WINDOW_X][MAX_WINDOW_Y];


void display(int width, int height, float zoom, float frame_x, float frame_y)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, width, 0, height);
	glMatrixMode(GL_MODELVIEW);	
	glLoadIdentity();

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
	glPointSize(1.0);

	mandelbrot_set(width, height, zoom, frame_x, frame_y);
	glFlush();
}


void mandelbrot_set(int width, int height, float zoom, float frame_x, float frame_y)
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


int get_iterations(int i, int j, int width, int height, float zoom, float frame_x, float frame_y) 
{
	float real = ((i / float(width) - 0.5f) * zoom + frame_x) * 5.0;
    float imag = ((j / float(height) - 0.5f) * zoom - frame_y) * 5.0;
 
    int iterations = 0;
    float const_real = real;
    float const_imag = imag;
 
    while (iterations < MAX_ITERATIONS) {
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


void draw_point(int i, int j, int height, int color)
{
	glBegin(GL_POINTS);
	set_color(color);
	glVertex2i(i, height - j);
	glEnd();
}


void set_color(int color)
{
	if (color == MAX_ITERATIONS)
	{
		glColor3f(0, 0, 0);
	}
	else
	{
		glColor3f((double)color / MAX_ITERATIONS, (double)color / MAX_ITERATIONS, 0);
	}
}

}  // namespace omp
