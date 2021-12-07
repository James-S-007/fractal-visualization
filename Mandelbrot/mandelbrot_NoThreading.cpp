//g++ -std=c++14 mandelbrot_omp.cpp -o3 -lglut -lGL -lGLU -lGLEW -o mandelbrot

#include <fstream>
#include <iostream>
#include <stdio.h>
#include <GL/glut.h>
#include <sstream>
#include <string>
#include <vector>
#include <complex>       

constexpr double eps = 2, umin = -2.2, umax = 0.7, vmin = -1.2, vmax = 1.2;
int m = 500;

GLuint window;
GLuint width = 720, height = 720;
GLuint position_x = 100, position_y = 100;

int colors[720][720];

//glm::vec3 clear_color = glm::vec3(1.0, 1.0, 1.0);

void flush_display();
void reshape(int width, int height);
void handle_key_press(unsigned char theKey, int mouseX, int mouseY);
void mandelbrot_set();

double get_u0(int i);
double get_v0(int j);
int find_color(int i, int j);
int find_color(double u0, double v0);
void draw_point(int i, int j, int k);

int main(int argc, char** argv)
{
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(position_x, position_y);
	glutInit(&argc, argv);

	window = glutCreateWindow("OpenGL Mandelbrot Fractal Set");
	glutReshapeFunc(reshape);
	glutDisplayFunc(flush_display);
	glutKeyboardFunc(handle_key_press);
	
	mandelbrot_set();

	glutMainLoop();
	return 0;
}

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

void mandelbrot_set()
{
	#pragma omp parallel for
	for (int ix = 0; ix < width; ++ix)
		for (int iy = 0; iy < height; ++iy)
		{
			colors[ix][iy] = find_color(ix, iy);
		} 
	for (int i = 0; i < width; ++i)
		for (int j = 0; j < height; ++j)
		{
			draw_point(i, j, colors[i][j]);
		}  

}


int find_color(int i, int j) {
	double u0 = get_u0(i);
	double v0 = get_v0(j);

	int k = find_color(u0, v0);
	return k;
}

void draw_point(int i, int j, int color) {
	glBegin(GL_POINTS);
	set_color(color);
	glVertex2i(i, height - j);
	glEnd();
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

double get_u0(int i) {
	return (umax - umin) / width * i + umin;
}

double get_v0(int j) {
	return (vmax - vmin) / height * j + vmin;
}

void flush_display()
{
	glFlush();
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

	mandelbrot_set();
}

void handle_key_press(unsigned char theKey, int mouseX, int mouseY)
{
	switch (theKey)
	{
	case 'w': // increase m
		m++;
		break;
	case 's': // decrease m
		m--;
		break;
	}
}
