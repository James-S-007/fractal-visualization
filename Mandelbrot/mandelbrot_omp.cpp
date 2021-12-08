/* 
Author: Jack Crandell & James Springer
Class: ECE 4122
Last Date Modified: 12/07/21
 
Description: Mandelbrot implementation via OpenMP
*/

#include <GL/glew.h>

#include "mandelbrot_omp.h"

#define MAX_WINDOW_X 1920
#define MAX_WINDOW_Y 1080
#define MAX_ITERATIONS 500

namespace omp {

constexpr double eps = 2, umin = -2.2, umax = 0.7, vmin = -1.2, vmax = 1.2;
int colors[MAX_WINDOW_X*MAX_WINDOW_Y];

/**
 * Display function called in the main.cpp
 *
 * @param width of screen
 * @param height of screen
 * @param zoom scaling factor of fractal 
 * @param frame_x controls where to render fractal in x -changed via panning
 * @param frame_y controls where to render fractal in y-changed via panning
 * 
 */
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

	mandelbrotSet(width, height, zoom, frame_x, frame_y);
	glFlush();
}


/**
 * Display function called by display which calls getIterations to see if number
 * is within the mandelbrot set.
 *
 * @param width of screen
 * @param height of screen
 * @param zoom scaling factor of fractal 
 * @param frame_x controls where to render fractal in x -changed via panning
 * @param frame_y controls where to render fractal in y-changed via panning
 * 
 */
void mandelbrotSet(int width, int height, float zoom, float frame_x, float frame_y)
{
	int minDim = (width < height) ? width : height;  // prevents stretching
	#pragma omp parallel for
	for (int ix = 0; ix < width; ++ix)
		for (int iy = 0; iy < height; ++iy)
		{
			colors[ix * width + iy] = getIterations(ix, iy, minDim, minDim, zoom, frame_x, frame_y);
		}
	for (int i = 0; i < width; ++i)
    {
        for (int j = 0; j < height; ++j)
		{
			drawPoint(i, j, height, colors[i* width + j]);
		} 
    } 
}

/**
 * Finds if a number is in the Mandelbrot set. 
 * This is defined by as any complex number, c, such that z = z^2 + c remains bounded.
 * For our purpose, we find if the magnitude of the number is < 2 then it is in the set. 
 * More iterations results in higher accuracy. If it is in the set it will return the number of iterations.
 *
 * @param width of screen
 * @param height of screen
 * @param zoom scaling factor of fractal 
 * @param frame_x controls where to render fractal in x -changed via panning
 * @param frame_y controls where to render fractal in y-changed via panning
 * 
 */
int getIterations(int i, int j, int width, int height, float zoom, float frame_x, float frame_y) 
{
	float real = ((i / float(width) - 0.5f) * zoom + frame_x) * 5.0;
    float imag = ((j / float(height) - 0.5f) * zoom - frame_y) * 5.0;
 
    int iterations = 0;
    float const_real = real;
    float const_imag = imag;
 
    while (iterations < MAX_ITERATIONS)
	{
        float temp_real = real;
        real = (real * real - imag * imag) + const_real;
        imag = (2.0f * temp_real * imag) + const_imag;
         
        float mag_sq = real * real + imag * imag;
         
        if (mag_sq > 4.0)
		{
			return iterations;
		}

        ++iterations;
    }

    return iterations;
}

/**
 * Draw the point on the screen.
 * 
 */
void drawPoint(int i, int j, int height, int color)
{
	glBegin(GL_POINTS);
	setColor(color);
	glVertex2i(i, height - j);
	glEnd();
}

/**
 * Assigns a color based returned iteration.
 *
 * @param color represents the iteration returned.
 * 
 */
void setColor(int color)
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
