
/* 
Author: Jackson Crandell
Class: ECE 4122
Last Date Modified: 12/07/21 
 
Description: Renders Sierpinski Tetrahedron in OpenGL
*/

#include <GL/glut.h>
#include <cstdlib>
#include <ctime>
#include <stdio.h>

// Funcation prototypes
void display();
void zoomIn();

GLfloat Tetra[4][3] =   {
                            {-0.65,-0.5, 0.5},
                            { 0.65,-0.5, 0.5},
                            { 0  , 0.6, 0.5},
                            { 0  ,-0.05,-0.5},
                        };

double anglex = 0;
double angley = 0;
int iterations = 5;
double zoom = 0;
int shading = GL_SMOOTH;

/**
 * Creates a tetrahedron by drawing four triangular faces.
 * 
 * 
 * @param *a sets vertex
 * @param *b sets vertex
 * @param *c sets vertex
 * @param *d sets vertex
 * 
 */
void drawTetra(GLfloat *a, GLfloat *b, GLfloat *c, GLfloat *d) {
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);	
	glShadeModel(shading);
	
	glBegin(GL_TRIANGLES);
		glColor3f(1,0,0);
		glVertex3fv(a);
		glColor3f(0,1,0);
		glVertex3fv(b);
		glColor3f(0,0,1);
		glVertex3fv(c);
	glEnd();

	glBegin(GL_TRIANGLES);
		glColor3f(0,0,1);
		glVertex3fv(a);
		glColor3f(0,1,0);
		glVertex3fv(b);
		glColor3f(1,0,0);
		glVertex3fv(d);
	glEnd();

	glBegin(GL_TRIANGLES);
		glColor3f(1,0,0);
		glVertex3fv(a);
		glColor3f(0,0,1);
		glVertex3fv(c);
		glColor3f(0,1,0);
		glVertex3fv(d);
	glEnd();

	glBegin(GL_TRIANGLES);
		glColor3f(0,1,0);
		glVertex3fv(b);
		glColor3f(1,0,0);
		glVertex3fv(c);
		glColor3f(0,0,1);
		glVertex3fv(d);
	glEnd();
}

/**
 * Divides triangle lengths to find midpoint to draw next triangle.
 * 
 * @param V1 sets (x,y) of vertex
 * @param V2 sets (x,y) of vertex
 * @param V3 sets (x,y) of vertex
 * @param V4 sets (x,y) of vertex
 * 
 */
void divideTetra(GLfloat V1[],GLfloat V2[],GLfloat V3[],GLfloat V4[],int iterations)
{
    GLfloat V12[3],V23[3],V31[3],V14[3],V24[3],V34[3];
    if(iterations > 0)
    {
        V12[0] = (V1[0] + V2[0]) / 2;        
		V12[1] = (V1[1] + V2[1]) / 2;        
		V12[2] = (V1[2] + V2[2]) / 2;
        V23[0] = (V2[0] + V3[0]) / 2;        
		V23[1] = (V2[1] + V3[1]) / 2;        
		V23[2] = (V2[2] + V3[2]) / 2;
        V31[0] = (V3[0] + V1[0]) / 2;        
		V31[1] = (V3[1] + V1[1]) / 2;        
		V31[2] = (V3[2] + V1[2]) / 2;
        V14[0] = (V1[0] + V4[0]) / 2;        
		V14[1] = (V1[1] + V4[1]) / 2;        
		V14[2] = (V1[2] + V4[2]) / 2;
        V24[0] = (V2[0] + V4[0]) / 2;        
		V24[1] = (V2[1] + V4[1]) / 2;        
		V24[2] = (V2[2] + V4[2]) / 2;
        V34[0] = (V3[0] + V4[0]) / 2;        
		V34[1] = (V3[1] + V4[1]) / 2;        
		V34[2] = (V3[2] + V4[2]) / 2;

        divideTetra(V1,V12,V31,V14,iterations-1);
        divideTetra(V12,V2,V23,V24,iterations-1);
        divideTetra(V31,V23,V3,V34,iterations-1);
        divideTetra(V14,V24,V34,V4,iterations-1);
    }
    else 
	{
		drawTetra(V1,V2,V3,V4);
	}
}

/**
 * Handles keyboard input. If you press, '=' then the number of iterations redered will increase by 10.
 * If you press '-' then the number of iterations will decrease by 10.
 * 
 * @param key holds value of key pushed
 * @param x holds mouse x location when key is pushed
 * @param y holds mouse y location when key is pushed
 * 
 */
void keyboard(unsigned char key, int x, int y) {
	switch (key) {
		case '=':
			if (iterations < 10) 
			{
				iterations += 1;
			}
			display();
			break;
		case '-':
		if (iterations > 0) 
			{
				iterations -= 1;
			}
			display();
			break;
		case 'q':
			exit(0);
			break;
	}
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
			if (zoom - 0.1 > -1) zoom -= 0.05;
			zoomIn();
	   }
	   else 
	   {
			zoom += 0.05;
			zoomIn();
	   }
   }
}


/**
 * Controls zooming in and out of the scene
 */
void zoomIn() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0 - zoom, 1.0 + zoom, -1.0 - zoom, 1.0 + zoom, -20.0, 20.0);
	glMatrixMode(GL_MODELVIEW);
	display();
}

/**
 * Displays tetrahedron.
 */
void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	glRotatef(angley, 0, 1, 0);
	glRotatef(anglex, 1, 0.0, 0.0 );

	divideTetra(Tetra[0], Tetra[1], Tetra[2], Tetra[3], iterations);

	glPopMatrix();
    glutSwapBuffers();
	glutPostRedisplay();
}

/**
 * Sets initial values for openGL
 */
void init() {
	glColor3f(0.0, 0.0, 0.0);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-2.0, 2.0, -2.0, 2.0, -20.0, 20.0);
	glEnable(GL_DEPTH_TEST);
}


int main(int argc, char *argv[]) {
    glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Sierpinski Tetrahedron");
	glutPositionWindow(100, 100);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutMouseFunc(mouse);
	init();
	glutDisplayFunc(display);
	glutMainLoop();
}