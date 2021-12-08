
/* 
Author: Jackson Crandell
Class: ECE 4122
Last Date Modified: 12/07/21 
 
Description: Mandelbrot set with CUDA optimization.

Reference: http://selkie.macalester.edu/csinparallel/modules/CUDAArchitecture/build/html/1-Mandelbrot/Mandelbrot.html
*/

//nvcc mandelbrot_cuda.cu -lglut -lGL -lGLU -lGLEW -lgomp

#include <fstream>
#include <iostream>
#include <stdio.h>
#include <GL/glut.h>
#include <sstream>
#include <string>
#include <vector>
#include <complex>    
#include <iomanip>
#include <omp.h>

GLuint window;

// Must be square window
// Preferable a window that is a multiple of 32, otherwise it may not work
static int dim = 512;
GLuint width = dim, height = dim;
static int n = 512;
static int m = 512;
static int max_iter = 1000;
double frame_x = -0.5;
double frame_y = 0;
double scale = 3;

int lastx = 0;
int lasty = 0;

void display();
void reshape(int width, int height);
void draw_point(int i, int j, int k);
void special(int key, int x, int y);
void mouse(int button, int state, int x, int y);



/**
 * Cuda kernel function that calculates if a number is in the Mandelbrot set. 
 * This is defined by as any complex number, c, such that z = z^2 + c remains bounded.
 * For our purpose, we find if the magnitude of the number is < 2 then it is in the set. 
 * More iterations results in higher accuracy.
 *
 * @param n - holds number of points to calculate
 * @param h - holds heat calculates
 * @param g - temp array to hold heat calculations
 * 
 */
__device__ uint32_t mandelbrot_set(double cr, double ci, int max_iter) {
    double real = 0;
    double imag = 0;
    double realsqr = 0;
    double imagsqr = 0;

    uint32_t i;

    for (i = 0; i < max_iter; i++){
		imag = real * imag;
		imag += imag;
		imag += ci;
		real = realsqr - imagsqr + cr;
		realsqr = real * real;
		imagsqr = imag * imag;
		
		if (realsqr + imagsqr > 4.0) break;
    }
	
    return i;
}

/**
 * Cuda kernel function that turns each (x,y) coordinate into a complex number.
 * The it runs the mandel_double kernel.
 *
 * @param counts holds pixel values
 * @param xmin minimum x value of screen
 * @param ymin minimum y value of screen
 * @param step size to move left and right by. Needed when zooming in
 * @param max_iter number of iterations to run until. (Higher iterations lead to higher accuracy)
 * @param dim dimension of window
 * 
 */
__global__ void mandel_kernel(uint32_t *counts, double xmin, double ymin, double step, int max_iter, int dim) {
    int pix_per_thread = dim * dim / (gridDim.x * blockDim.x);
    int tId = blockDim.x * blockIdx.x + threadIdx.x;
    int offset = pix_per_thread * tId;
    for (int i = offset; i < offset + pix_per_thread; i++){
        int x = i % dim;
        int y = i / dim;
        double cr = xmin + x * step;
        double ci = ymin + y * step;
        counts[y * dim + x]  = mandelbrot_set(cr, ci, max_iter);
    }
    if (gridDim.x * blockDim.x * pix_per_thread < dim * dim && tId < (dim * dim) - (blockDim.x * gridDim.x)){
        int i = blockDim.x * gridDim.x * pix_per_thread + tId;
        int x = i % dim;
        int y = i / dim;
        double cr = xmin + x * step;
        double ci = ymin + y * step;
        counts[y * dim + x]  = mandelbrot_set(cr, ci, max_iter);
    }
    
}

/**
 * Cuda kernel function that turns each (x,y) coordinate into a complex number.
 * The it runs the mandel_double kernel.
 *
 * @param frame_x controls where to render fractal in x -changed via panning
 * @param frame_y controls where to render fractal in y-changed via panning
 * @param scale scaling factor of fractal 
 * @param dev_counts 
 * 
 */
void display_mandelbrot(double frame_x, double frame_y, double scale, uint32_t *dev_counts)
{
    double start = omp_get_wtime();
    dim3 numBlocks(dim,dim);
    double xmin = frame_x - (scale/2);
    double ymin = frame_y - (scale/2);
    double step = scale / dim;
    cudaError_t err = cudaSuccess;
    mandel_kernel<<<n, m>>>(dev_counts, xmin , ymin, step, max_iter, dim);
    err = cudaGetLastError();

    if (err != cudaSuccess)
    {
        fprintf(stderr, "Failed to launch kernel!\n");
        exit(EXIT_FAILURE);
    }
	int *data = (int *)malloc(dim * dim * sizeof(uint32_t));
    err = cudaMemcpy(data, dev_counts, dim * dim * sizeof(uint32_t), cudaMemcpyDeviceToHost);
    if (err != cudaSuccess)
    {
        fprintf(stderr, "Failed to copy dev_counts back\n");
        exit(EXIT_FAILURE);
    }
    double stop = omp_get_wtime();
    //printf("Blocks: %d\tThreads per Block: %d\tSize:%dx%d\tDepth: %d\tTime: %f\n",n, m, dim, dim, max_iter, stop - start);

    for (int i = 0; i < width; ++i)
    {
        for (int j = 0; j < height; ++j)
		{
			draw_point(j, i, data[i* dim + j]);
		} 
    }
}

/**
 * Assigns a color based returned iteration.
 *
 * @param color represents the iteration returned.
 * 
 */
void set_color(int color)
{
    // Set color to black if number is in max iter
	if (color == max_iter)
	{
		glColor3f(0, 0, 0);
	}
	else
	{
        float freq = 6.3 / max_iter;
        glColor3f((sin(freq * color + 5)),sin(freq * color + 3),sin(freq * color + 1));
	}
}

/**
 * Draw the point on the screen.
 * 
 */
void draw_point(int i, int j, int color) {
	glBegin(GL_POINTS);
	set_color(color);
	glVertex2i(i, height - j);
	glEnd();
}

/**
 * Handles mouse input. Use the scroll wheel to zoom in and out
 * of the scene
 * 
 * @param button holds the different mouse inputs (i.e. scroll wheel in our case)
 * @param x holds mouse x location when key is pushed
 * @param y holds mouse y location when key is pushed
 * 
 */
void mouse(int button, int state, int x, int y) {
	// Wheel reports as button 3(scroll up) and button 4(scroll down)
   if ((button == 3) || (button == 4)) // It's a wheel event
   {
       // Each wheel event reports like a button click, GLUT_DOWN then GLUT_UP
       if (state == GLUT_UP) return; // Disregard redundant GLUT_UP events
	   if (button == 3)
	   {
			scale *= 0.80;
	   }
	   else 
	   {
			scale *= 1.25;
	   }
   }

    // Panning
    lastx = x;
    lasty = y;
    display();
}

/**
 * Handles keyboard input. Use the arrow keys to control the camera to 
 * simulate the tetrahedron moving.
 * 
 * @param key holds value of key pushed
 * @param x holds mouse x location when key is pushed
 * @param y holds mouse y location when key is pushed
 * 
 */
void special(int key, int x, int y) {
	switch (key) {
		case GLUT_KEY_UP:
			frame_y -= 20 * scale / dim;
			break;
		case GLUT_KEY_DOWN:
			frame_y += 20 * scale / dim;
			break;
		case GLUT_KEY_RIGHT:
			frame_x += 20 * scale / dim;
			break;
		case GLUT_KEY_LEFT:
			frame_x -= 20 * scale / dim;
			break;
	}
}

/**
 * Displays Mandelbrot set.
 */
void display()
{
	glViewport(0, 0, width, height);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, width, 0, height);
	glMatrixMode(GL_MODELVIEW);	
	glLoadIdentity();

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glPointSize(1.0);

    cudaError_t err = cudaSuccess;

    uint32_t *dev_counts = NULL;
    size_t img_size = dim * dim * sizeof(uint32_t);
    err = cudaMalloc(&dev_counts, img_size);

    if (err != cudaSuccess)
    {
        fprintf(stderr,"Failed to allocate dev_counts\n");
        exit(EXIT_FAILURE);
    }
    display_mandelbrot(frame_x, frame_y, scale, dev_counts);

	glutSwapBuffers();
	glutPostRedisplay();
}

void reshape(int w, int h)
{
	width = w; height = h;
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, width, 0, height);
	glMatrixMode(GL_MODELVIEW);	
	glLoadIdentity();

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glPointSize(1.0);

    cudaError_t err = cudaSuccess;

    uint32_t *dev_counts = NULL;
    size_t img_size = dim * dim * sizeof(uint32_t);
    err = cudaMalloc(&dev_counts, img_size);

    if (err != cudaSuccess)
    {
        fprintf(stderr,"Failed to allocate dev_counts\n");
        exit(EXIT_FAILURE);
    }
    display_mandelbrot(frame_x, frame_y, scale, dev_counts);

}

/**
 * Handles mouse input to allow for panning.
 * 
 * @param key holds value of key pushed
 * @param x holds mouse x location when key is pushed
 * @param y holds mouse y location when key is pushed
 * 
 */
void mouseMovement(int x, int y)
{
    int diffx = x - lastx; 
    int diffy = y - lasty; 
    lastx = x;
    lasty = y;
    diffx = (diffx > 1.0f) ? 1.0f : diffx;
    diffx = (diffx < -1.0f) ? -1.0f : diffx;
    diffy = (diffy > 1.0f) ? 1.0f : diffy;
    diffy = (diffy < -1.0f) ? -1.0f : diffy;
	frame_y -= diffy * 10 * scale / dim;
	frame_x -= diffx * 10 * scale / dim;
}



int main(int argc, char** argv)
{
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(100, 100);
	glutInit(&argc, argv);

	window = glutCreateWindow("OpenGL Mandelbrot Fractal Set");
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
    glutSpecialFunc(special);
    glutMouseFunc(mouse);
    glutMotionFunc(mouseMovement);
    cudaError_t err = cudaSuccess;

    uint32_t *dev_counts = NULL;
    size_t img_size = dim * dim * sizeof(uint32_t);
    err = cudaMalloc(&dev_counts, img_size);

    if (err != cudaSuccess)
    {
        fprintf(stderr,"Failed to allocate dev_counts\n");
        exit(EXIT_FAILURE);
    }
    display_mandelbrot(frame_x, frame_y, scale, dev_counts);
	glutMainLoop();
    cudaFree(dev_counts);
	return 0;
}
