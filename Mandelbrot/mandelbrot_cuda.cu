//nvcc mandelbrot_cuda.cu -lglut -lGL -lGLU -lGLEW -lgomp && time ./a.out

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

static int dim = 512;
GLuint width = dim, height = dim;
static int n = 512;
static int m = 512;
static int max_iter = 10000;
double xcen = -0.5;
double ycen = 0;
double scale = 3;

void display();
void reshape(int width, int height);
void draw_point(int i, int j, int k);
void special(int key, int x, int y);
void mouse(int button, int state, int x, int y);
//void idle();

/* the mandelbrot set is defined as all complex numbers c such that the 
   equation z = z^2 + c remains bounded. In practice, we calculate max_iter
   iterations of this formula and if the magnitude of z is < 2 we assume it
   is in the set. The greater max_iters the more accurate our representation */
__device__ uint32_t mandel_double(double cr, double ci, int max_iter) {
    double zr = 0;
    double zi = 0;
    double zrsqr = 0;
    double zisqr = 0;

    uint32_t i;

    for (i = 0; i < max_iter; i++){
		zi = zr * zi;
		zi += zi;
		zi += ci;
		zr = zrsqr - zisqr + cr;
		zrsqr = zr * zr;
		zisqr = zi * zi;
		
    //the fewer iterations it takes to diverge, the farther from the set
		if (zrsqr + zisqr > 4.0) break;
    }
	
    return i;
}

/* turn each x y coordinate into a complex number and run the mandelbrot formula on it */
__global__ void mandel_kernel(uint32_t *counts, double xmin, double ymin, double step, int max_iter, int dim) {
    int pix_per_thread = dim * dim / (gridDim.x * blockDim.x);
    int tId = blockDim.x * blockIdx.x + threadIdx.x;
    int offset = pix_per_thread * tId;
    for (int i = offset; i < offset + pix_per_thread; i++){
        int x = i % dim;
        int y = i / dim;
        double cr = xmin + x * step;
        double ci = ymin + y * step;
        counts[y * dim + x]  = mandel_double(cr, ci, max_iter);
    }
    if (gridDim.x * blockDim.x * pix_per_thread < dim * dim && tId < (dim * dim) - (blockDim.x * gridDim.x)){
        int i = blockDim.x * gridDim.x * pix_per_thread + tId;
        int x = i % dim;
        int y = i / dim;
        double cr = xmin + x * step;
        double ci = ymin + y * step;
        counts[y * dim + x]  = mandel_double(cr, ci, max_iter);
    }
    
}

void display_double(double xcen, double ycen, double scale, uint32_t *dev_counts)
{
    double start = omp_get_wtime();
    dim3 numBlocks(dim,dim);
    double xmin = xcen - (scale/2);
    double ymin = ycen - (scale/2);
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
    cudaError_t err = cudaSuccess;

    uint32_t *dev_counts = NULL;
    size_t img_size = dim * dim * sizeof(uint32_t);
    err = cudaMalloc(&dev_counts, img_size);

    if (err != cudaSuccess)
    {
        fprintf(stderr,"Failed to allocate dev_counts\n");
        exit(EXIT_FAILURE);
    }
    display_double(xcen, ycen, scale, dev_counts);
	glutMainLoop();
    cudaFree(dev_counts);
	return 0;
}

void set_color(int color)
{
	if (color == max_iter)
	{
		glColor3f(0, 0, 0);
	}
	else
	{
        // TODO: Set different colors
        float freq = 6.3 / max_iter;
        glColor3f((sin(freq * color + 3)),sin(freq * color + 5),sin(freq * color + 1));
		//glColor3f((double)color / 100.0, (double)color / 100.0, (double)color / 100.0);
	}
}


void draw_point(int i, int j, int color) {
	glBegin(GL_POINTS);
	set_color(color);
	glVertex2i(i, height - j);
	glEnd();
}

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
   display();
}


void special(int key, int x, int y) {
	switch (key) {
		case GLUT_KEY_UP:
			ycen -= 20 * scale / dim;
			break;
		case GLUT_KEY_DOWN:
			ycen += 20 * scale / dim;
			break;
		case GLUT_KEY_RIGHT:
			xcen += 20 * scale / dim;
			break;
		case GLUT_KEY_LEFT:
			xcen -= 20 * scale / dim;
			break;
	}
}

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
    display_double(xcen, ycen, scale, dev_counts);

	glutSwapBuffers();
	glutPostRedisplay();
}

/* void idle()
{
    glutPostRedisplay();
} */

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
    display_double(xcen, ycen, scale, dev_counts);

}

