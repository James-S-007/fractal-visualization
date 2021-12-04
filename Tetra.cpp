#include <GL/glut.h>
#include <cstdlib>
#include <ctime>
#include <stdio.h>

// menu item
#define MENU_SMOOTH 1
#define MENU_FLAT 0

// Function prototypes
void generateColors();
void drawPyramid(GLfloat *a, GLfloat *b, GLfloat *c, GLfloat *d);
void dividePyramid(GLfloat *a, GLfloat *b, GLfloat *c, GLfloat *d, int iteraciones);
void keyboard(unsigned char key, int x, int y);
void special(int key, int x, int y);
void mouse(int button, int state, int x, int y);
void display();
void doZoom();
void init();

/* GLfloat pyramid[4][3] = {{-1.0, -1.0, -1.0},
						 { 1.0, -1.0, -1.0},
						 { 0.0, -1.0,  0.732},
						 { 0.0,  1.0, -0.134}}; */

/* GLfloat pyramid[4][3] = {{-0.5, -0.5, 0.5},
						 { 0.5, -0.5, 0.5},
						 { 0.0, 0.5,  0.5},
						 { 0.0,  0.1, -0.5}}; */

GLfloat pyramid[4][3] =   {
                            {-0.65,-0.5, 0.5},
                            { 0.65,-0.5, 0.5},
                            { 0  , 0.6, 0.5},
                            { 0  ,-0.05,-0.5},
                        };

double colors[1000] = {0};

double anglex = 0;
double angley = 0;
int iterations = 5;
double zoom = 0;
int shading = GL_SMOOTH;

void generateColors() {
	for (int i = 0; i < 1000; i++) {
		colors[i] = rand() / (double) RAND_MAX;
	}
}

double random_(bool reset = false) {
	static int curr = 0;
	if (reset) { 
		curr = 0;
		return 0.0;
	} else {
		if (curr >= 1000) curr = 0;
		return colors[curr++];
	}
}

void drawPyramid(GLfloat *a, GLfloat *b, GLfloat *c, GLfloat *d) {
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);	
	glShadeModel(shading);
	
	// bottom
	glBegin(GL_TRIANGLES);
		glColor3f(random_(), random_(), random_());
		glVertex3fv(a);
		glColor3f(random_(), random_(), random_());
		glVertex3fv(b);
		glColor3f(random_(), random_(), random_());
		glVertex3fv(c);
	glEnd();

	glBegin(GL_TRIANGLES);
		glColor3f(random_(), random_(), random_());
		glVertex3fv(a);
		glColor3f(random_(), random_(), random_());
		glVertex3fv(b);
		glColor3f(random_(), random_(), random_());
		glVertex3fv(d);
	glEnd();

	glBegin(GL_TRIANGLES);
		glColor3f(random_(), random_(), random_());
		glVertex3fv(a);
		glColor3f(random_(), random_(), random_());
		glVertex3fv(c);
		glColor3f(random_(), random_(), random_());
		glVertex3fv(d);
	glEnd();

	glBegin(GL_TRIANGLES);
		glColor3f(random_(), random_(), random_());
		glVertex3fv(b);
		glColor3f(random_(), random_(), random_());
		glVertex3fv(c);
		glColor3f(random_(), random_(), random_());
		glVertex3fv(d);
	glEnd();
}

void dividePyramid(GLfloat *a, GLfloat *b, GLfloat *c, GLfloat *d, int iteraciones) {
     GLfloat v[6][3];
     int j;
     if (iteraciones > 0) {
        for (j = 0; j < 3; j++) {
            v[0][j] = (a[j] + b[j]) / 2;
		}
        for (j = 0; j < 3; j++) {
			v[1][j] = (a[j] + c[j]) / 2;
		}
        for (j = 0; j < 3; j++) {
            v[2][j] = (b[j] + c[j]) / 2;
		}
		for (j = 0; j < 3; j++) {
            v[3][j] = (a[j] + d[j]) / 2;
		}
		for (j = 0; j < 3; j++) {
            v[4][j] = (b[j] + d[j]) / 2;
		}
		for (j = 0; j < 3; j++) {
            v[5][j] = (c[j] + d[j]) / 2;
		}

        dividePyramid( a  , v[0], v[1], v[3], iteraciones-1);
        dividePyramid(v[0],  b  , v[2], v[4], iteraciones-1);
        dividePyramid(v[1], v[2],  c  , v[5], iteraciones-1);
		dividePyramid(v[3], v[4], v[5],  d  , iteraciones-1);
		// commenting this will create a Sierpinski Triangle
        //divideTriangle(v[0], v[1], v[2], iteraciones-1);

     } else {
        drawPyramid(a, b, c, d);
     }
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
		case '+':
			if (iterations < 10) iterations += 1;
			display();
			break;
		case '-':
			if (iterations > 0) iterations -= 1;
			display();
			break;
		case 'q':
			exit(0);
			break;
	}
}

void special(int key, int x, int y) {
	switch (key) {
		case GLUT_KEY_UP:
			anglex += 1;
			break;
		case GLUT_KEY_DOWN:
			anglex -= 1;
			break;
		case GLUT_KEY_RIGHT:
			angley += 1;
			break;
		case GLUT_KEY_LEFT:
			angley -= 1;
			break;
	}
}

void mouse(int button, int state, int x, int y) {
	// Wheel reports as button 3(scroll up) and button 4(scroll down)
   if ((button == 3) || (button == 4)) // It's a wheel event
   {
       // Each wheel event reports like a button click, GLUT_DOWN then GLUT_UP
       if (state == GLUT_UP) return; // Disregard redundant GLUT_UP events
	   if (button == 3)
	   {
			if (zoom - 0.1 > -1) zoom -= 0.05;
			doZoom();
	   }
	   else 
	   {
			zoom += 0.05;
			doZoom();
	   }
   }
}

void doZoom() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0 - zoom, 1.0 + zoom, -1.0 - zoom, 1.0 + zoom, -20.0, 20.0);
	glMatrixMode(GL_MODELVIEW);
	display();
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	glRotatef(angley, 0, 1, 0);
	glRotatef(anglex, 1, 0.0, 0.0 );

	random_(true);
	dividePyramid(pyramid[0], pyramid[1], pyramid[2], pyramid[3], iterations);

	glPopMatrix();
    glutSwapBuffers();

	glutPostRedisplay();
}

void init() {
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glColor3f(0.0, 0.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-2.0, 2.0, -2.0, 2.0, -20.0, 20.0);
	glEnable(GL_DEPTH_TEST);
}


int main(int argc, char *argv[]) {
	srand(time(NULL));

	generateColors();
    glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Sierpinski Triangle");
	glutPositionWindow(100, 100);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutMouseFunc(mouse);
	glutAddMenuEntry("Smooth shading", MENU_SMOOTH);
	glutAddMenuEntry("Flat shading", MENU_FLAT);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	init();
	glutDisplayFunc(display);
	glutMainLoop();
}