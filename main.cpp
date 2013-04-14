// main.cpp
// Par Gabriel Le Breton aka GabLeRoux
// 3 Octobre 2011

#include <stdio.h>
#include <stdlib.h>

#include <time.h>
#include <fstream>
#include <string>
#include <cmath>
#include <iostream>

#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
// Windows users, you should try this:
// #include "glut/glut.h"

/* Global Variables (Configs) */
//Init options
int selectedObject = 3;
bool drawThatAxis = 0;
bool lightEffect = 1;
GLdouble sphereRadius = 0.4;
GLdouble cylinderRadius = 0.2;
GLint resolution = 100;
GLint slices = resolution, stacks = resolution;

//Viewer options (GluLookAt)
float fovy = 60.0, aspect = 1.0, zNear = 1.0, zFar = 100.0;

//Mouse modifiers
float depth = 8;
float phi=0, theta=0;
float downX, downY;
bool leftButton = false, middleButton = false;

//colors
GLfloat oxygen[3] =		{1.0, 0.0, 0.0};	// (O - Red)
GLfloat nitrogen[3] =	{0.0, 0.0, 1.0};	// (N - Blue)
GLfloat phosphate[3] =	{1.0, 0.5, 0.0};	// (P - Orange)
GLfloat carbon[3] =		{0.5, 0.5, 0.5};	// (C - Grey)
GLfloat hydrogen[3] =	{1.0, 1.0, 1.0};	// (H - White)
GLfloat white[3] =		{1.0, 1.0, 1.0};
GLfloat black[3] =		{1.0, 1.0, 1.0};

/* Prototypes */
void liaison(GLfloat color[3], GLfloat height);
void atome(GLfloat color[3]);
void setLightColor(GLfloat light_color[3]);
void renderCylinder(float x1, float y1, float z1, float x2,float y2, float z2, float radius, GLUquadricObj *quadrilc);
void drawAxis();
void reshape(int w, int h);
void mouseCallback(int button, int state, int x, int y);
void motionCallback(int x, int y);
void keyboardCallback(unsigned char ch, int x, int y);
void displayCallback(void);
void buildDisplayList();
void options_menu(int input);
void initMenu();

GLdouble width, height;   /* window width and height */
int wd;                   /* GLUT window handle */

/* Program initialization NOT OpenGL/GLUT dependent,
 as we haven't created a GLUT window yet */
void init(void)
{
	width  = 1280.0;                 /* initial window width and height, */
	height = 800.0;                  /* within which we draw. */
}

// Called when window is resized,
// also when window is first created,
// before the first call to display().
void reshape(int w, int h)
{
	/* save new screen dimensions */
	width = (GLdouble) w;
	height = (GLdouble) h;
	
  	/* tell OpenGL to use the whole window for drawing */
	glViewport(0, 0, (GLsizei) width, (GLsizei) height);
	
  	/* do an orthographic parallel projection with the coordinate
     system set to first quadrant, limited by screen/window size */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, width, 0.0, height);
	
	aspect = width/height;
	glutPostRedisplay();
}

int main(int argc, char *argv[])
{
	/* perform initialization NOT OpenGL/GLUT dependent,
	 as we haven't created a GLUT window yet */
	init();
	
	/* initialize GLUT, let it extract command-line 
	 GLUT options that you may provide 
	 - NOTE THE '&' BEFORE argc */
	glutInit(&argc, argv);
	
	/* specify the display to be single 
     buffered and color as RGBA values */
	
	glutInitDisplayMode(GLUT_DEPTH);
	
	/* set the initial window size */
	glutInitWindowSize((int) width, (int) height);
	
	/* create the window and store the handle to it */
	wd = glutCreateWindow("Super 3D Molecules - Funny OpenGL App by GabLeRoux" /* title */ );
	
	/* --- register callbacks with GLUT --- */
	
	/* register function to handle window resizes */
	glutReshapeFunc(reshape);
	
	setLightColor(white);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	
	buildDisplayList();
	
	/* register function that draws in the window */
	glutDisplayFunc(displayCallback);
	
	// motion
	glutMouseFunc(mouseCallback);
	glutMotionFunc(motionCallback);
	
	// exit with [esc] keyboard button
	glutKeyboardFunc(keyboardCallback);
	
	initMenu();
	
	/* start the GLUT main loop */
	glutMainLoop();
	
	return 0;
}

void liaison(GLfloat color[3], GLfloat height)
{
	//sans lumière:
	glColor3fv(color);
	//avec lumière
	setLightColor(color);
	
	GLUquadric *myQuad;
	myQuad=gluNewQuadric();
	
	//Création du cylindre
	gluCylinder(myQuad, cylinderRadius, cylinderRadius, height, slices, stacks);
}

//Sphere
void atome(GLfloat color[3])
{
	//sans lumière:
	glColor3fv(color);
	//avec lumière
	setLightColor(color);
	
	GLUquadric *myQuad;
	myQuad=gluNewQuadric();
	
	//Création de la sphere
	gluSphere(myQuad , sphereRadius , slices , stacks);
}

void setLightColor(GLfloat light_color[3])
{
	//Light Options
	GLfloat mat_specular[]={ 1.0, 1.0, 1.0, 1.0 };
	GLfloat shine[] = {100.0};
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular );
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_color );
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_color );
	glMaterialfv(GL_FRONT, GL_SHININESS, shine);
}

//SRC: http://lifeofaprogrammergeek.blogspot.com/2008/07/rendering-cylinder-between-two-points.html
//will make a cylender between 2 pts :D
void renderCylinder(float x1, float y1, float z1, float x2,float y2, float z2, float radius, GLUquadricObj *quadric)
{
	float vx = x2-x1;
	float vy = y2-y1;
	float vz = z2-z1;
	float ax,rx,ry,rz;
	float len = sqrt( vx*vx + vy*vy + vz*vz );
	
	glPushMatrix();
	glTranslatef( x1,y1,z1 );
	if (fabs(vz) < 0.0001)
	{
		glRotatef(90, 0,1,0);
		ax = 57.2957795*-atan( vy / vx );
		if (vx < 0)
		{
			
		}
		rx = 1;
		ry = 0;
		rz = 0;
	}
	else
	{
		ax = 57.2957795*acos( vz/ len );
		if (vz < 0.0)
			ax = -ax;
		rx = -vy*vz;
		ry = vx*vz;
		rz = 0;
	}
	glRotatef(ax, rx, ry, rz);
	gluQuadricOrientation(quadric,GLU_OUTSIDE);
	gluCylinder(quadric, radius, radius, len, slices, stacks);
	glPopMatrix();
}

void drawAxis()
{
	
	float originAxis[3] = {0,0,0};	//Origine
	float xAxis[3] = {1, 0, 0};		// L'axe des x
	float yAxis[3] = {0, 1, 0};		// L'axe des y
	float zAxis[3] = {0, 0, 1};		// L'axe des z
	
	//Temp: Désactivation de la lumière
	glDisable(GL_LIGHTING);  
	glPushMatrix ();
	glLineWidth (10.0);
	
	//x = rouge, y = vert, z = bleu
	
	glBegin (GL_LINES);
	glColor3f(1.0, 0.0, 0.0);
	glVertex3fv(originAxis);
	glVertex3fv(xAxis);
	glColor3f(0.0, 1.0, 0.0);
	glVertex3fv(originAxis);
	glVertex3fv(yAxis);
	glColor3f(0.0, 0.0, 1.0);
	glVertex3fv(originAxis);
	glVertex3fv(zAxis);
	glEnd();
	
	glPopMatrix ();
	//Réactivation de la lumière
	glEnable(GL_LIGHTING);
}

/* Callbacks */
void mouseCallback(int button, int state, int x, int y)
{
	downX = x; downY = y;
	leftButton = ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN));
	middleButton = ((button == GLUT_MIDDLE_BUTTON) &&  (state == GLUT_DOWN));
}

void motionCallback(int x, int y)
{
	if (leftButton) //Rotate
	{
		phi += (x-downX)/4.0;
		theta += (downY-y)/4.0;
	}
	if (middleButton) //Scale
	{
		if (depth + (downY - y)/10.0 < zFar-10 && depth + (downY - y)/10.0 > zNear+3)
			depth += (downY - y)/10.0;
	}
	downX = x;
	downY = y;
	
	glutPostRedisplay();
}

void keyboardCallback(unsigned char ch, int x, int y)
{
	switch (ch)
	{
			//Esc button will exit app
		case 27:	
			exit(0);
			break;
	}
	glutPostRedisplay();
}

void displayCallback(void)
{	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, aspect, zNear, zFar);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	//gluLookAt(eyex, eyey, eyez, atx, aty, atz, upx, upy, upz);
	gluLookAt(0, 0, 2, 0, 0, 0, 0, 1, 0);
	
	
	//Motion Options
	glTranslatef(0.0, 0.0, -depth);
	glRotatef(-theta, 1.0, 0.0, 0.0);
	glRotatef(phi, 0.0, 1.0, 0.0);
	
	//Axis x, y and z Toggle
	if (drawThatAxis)
	{
		drawAxis();
	}
	
	//Light Effect Toggle :)
	if (lightEffect) {
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
	}
	else
	{
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
	}
	
	switch (selectedObject)
	{
		//Molecule 1
		case (1):
			glCallList(1);
			break;
		//Molecule 2
		case (2):
			glCallList(2);
			break;
		//Molecule 3
		case (3):
			glCallList(3);
		default:
			break;
	}
	
	glFlush(); 
}

/* Our display items to prepare */
void buildDisplayList()
{
	GLuint id;
	
	// 3 elements
	id = glGenLists( 3 );
	
	// 1st molecule (H2O)
	glNewList( id, GL_COMPILE );
	glPushMatrix();
	GLUquadric *myQuad;
	myQuad=gluNewQuadric();
	
	// Begin drawing
	
	// molecules
	glPushMatrix();
	glTranslatef(-1, 0, 0);
	atome(oxygen);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(0, -.5, 0);
	atome(hydrogen);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(1, 0, 0);
	atome(oxygen);
	glPopMatrix();
	
	// links
	glPushMatrix();
	glTranslatef(0, -.5, 0);
	glRotatef(-25, 0, 0, 1);
	glRotatef(-90, 0, 1, 0);
	liaison(white, 1.1180);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(0, -.5, 0);
	glRotatef(25, 0, 0, 1);
	glRotatef(90, 0, 1, 0);
	liaison(white, 1.1180);
	glPopMatrix();
	
	// End drawing
	
	glPopMatrix();
	glEndList();
	
	// 2nd molecule (CH4 tethra)
	id++;
	glNewList( id, GL_COMPILE );
	glPushMatrix();
	
	// Begin drawing
	glPushMatrix();
	glTranslatef(0, 0, 0);
	atome(carbon);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(0, -1, 1.5);
	atome(hydrogen);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(-1, -1, -1);
	atome(hydrogen);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(1.3, -1, -0.3);
	atome(hydrogen);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(0, 1.5, 0);
	atome(hydrogen);
	glPopMatrix();
	
	// links
	renderCylinder(0, 0, 0, 0, -1, 1.5, cylinderRadius, myQuad);
	renderCylinder(0, 0, 0, -1, -1, -1, cylinderRadius, myQuad);
	renderCylinder(0, 0, 0, 1.3, -1, -0.3, cylinderRadius, myQuad);
	renderCylinder(0, 0, 0, 0, 1.5, 0, cylinderRadius, myQuad);
	
	// End drawing
	
	glPopMatrix();
	glEndList();
	
	// 3rd molecule
	id++;
	glNewList( id, GL_COMPILE );
	glPushMatrix();
	
	// Begin drawing
	
	// will draw from file
	std::ifstream file("caffeine.xyz");
	
	//If file is open
	if(!file.fail())
	{
		std::cout << "caffeine.xyz file opened successfully" << std::endl;

		char atomeType;
		GLfloat x;
		GLfloat y;
		GLfloat z;

		int nbElements;

		file >> nbElements;

		std::string atomeName;
		file >> atomeName;
		std::cout << atomeName << "\n";

		//for every elements, push, draw, pop
		for (int i = 0; i < nbElements; i++)
		{
			file >> atomeType;
			file >> x;
			file >> y;
			file >> z;

			glPushMatrix();

			glTranslatef(x, y, z);
			switch (atomeType)
			{
			case 'O':
				atome(oxygen);
				break;
			case 'N':
				atome(nitrogen);
				break;
			case 'P':
				atome(phosphate);
				break;
			case 'C':
				atome(carbon);
				break;
			case 'H':
				atome(hydrogen);
				break;
			default:
				break;
			}
			glPopMatrix();
		}
	file.close();  
	}
	else std::cerr << "Can't open file caffeine.xyz!\n";

	// Note October 3, 2011:
	// This part of the code should be made from an algorythm
	// I did not take the time to code it so I hardcoded links with
	// alot of imagination and coffee ;)
	// Links
	
	// First hex
	renderCylinder(-1.3751742, -1.0212243, -0.0570552, -1.2615018, 0.2590713, 0.5234135, cylinderRadius, myQuad);
	renderCylinder(-1.2615018, 0.2590713, 0.5234135, 0.0567293, 0.8527195, 0.3923156, cylinderRadius, myQuad);
	renderCylinder(0.0567293, 0.8527195, 0.3923156, 1.1394235, 0.1874122, -0.2700900, cylinderRadius, myQuad);
	renderCylinder(1.1394235, 0.1874122, -0.2700900, 0.9668296, -1.0737425, -0.8198227, cylinderRadius, myQuad);
	renderCylinder(0.9668296, -1.0737425, -0.8198227, -0.3068337, -1.6836331, -0.7169344, cylinderRadius, myQuad);
	renderCylinder(-0.3068337, -1.6836331, -0.7169344, -1.3751742, -1.0212243, -0.0570552, cylinderRadius, myQuad);
	// Oxygenes
	renderCylinder(-0.3068337, -1.6836331, -0.7169344, -0.4926797, -2.8180554, -1.2094732, cylinderRadius, myQuad);
	renderCylinder(-1.2615018, 0.2590713, 0.5234135, -2.2301338, 0.7988624, 1.0899730, cylinderRadius, myQuad);
	// Second hex
	renderCylinder(0.0567293, 0.8527195, 0.3923156, 0.5602627, 2.0839095, 0.8251589, cylinderRadius, myQuad);
	renderCylinder(0.5602627, 2.0839095, 0.8251589, 1.8934096, 2.1181245, 0.4193193, cylinderRadius, myQuad);
	renderCylinder(1.8934096, 2.1181245, 0.4193193, 2.2861252, 0.9968439, -0.2440298, cylinderRadius, myQuad);
	renderCylinder(2.2861252, 0.9968439, -0.2440298, 1.1394235, 0.1874122, -0.2700900, cylinderRadius, myQuad);
	// top right H
	renderCylinder(1.8934096, 2.1181245, 0.4193193, 2.5496990, 2.9734977, 0.6229590, cylinderRadius, myQuad);
	// top CH3
	renderCylinder(-0.1518450, 3.0970046, 1.5348347, -0.1687028, 4.0436553, 0.9301094, cylinderRadius, myQuad);
	renderCylinder(-0.1518450, 3.0970046, 1.5348347, 0.3535322, 3.2979060, 2.5177747, cylinderRadius, myQuad);
	renderCylinder(-0.1518450, 3.0970046, 1.5348347, -1.2074498, 2.7537592, 1.7203047, cylinderRadius, myQuad);
	renderCylinder(-0.1518450, 3.0970046, 1.5348347, 0.5602627, 2.0839095, 0.8251589, cylinderRadius, myQuad);
	// bottom-right CH3
	renderCylinder(2.0527432, -1.7360887, -1.4931279, 2.9176101, -1.8481516, -0.7857866, cylinderRadius, myQuad);
	renderCylinder(2.0527432, -1.7360887, -1.4931279, 2.3787863, -1.1211917, -2.3743655, cylinderRadius, myQuad);
	renderCylinder(2.0527432, -1.7360887, -1.4931279, 1.7189877, -2.7489920, -1.8439205, cylinderRadius, myQuad);
	renderCylinder(2.0527432, -1.7360887, -1.4931279, 0.9668296, -1.0737425, -0.8198227, cylinderRadius, myQuad);
	// bottom-left CH3
	renderCylinder(-2.6328073, -1.7303959, -0.0060953, -3.3804130, -1.1272367, 0.5733036, cylinderRadius, myQuad);
	renderCylinder(-2.6328073, -1.7303959, -0.0060953, -2.4807715, -2.7269528, 0.4882631, cylinderRadius, myQuad);
	renderCylinder(-2.6328073, -1.7303959, -0.0060953, -3.0089039, -1.9025254, -1.0498023, cylinderRadius, myQuad);
	renderCylinder(-2.6328073, -1.7303959, -0.0060953, -1.3751742, -1.0212243, -0.0570552, cylinderRadius, myQuad);
	
	//End drawing
	
	glPopMatrix();
	glEndList();
	
	glutSwapBuffers();
}

void options_menu(int input)
{
	if (input == 1 || input == 2 || input == 3)
	{
		selectedObject = input;
	}
	else if (input == 4)
	{
		if (lightEffect == 1)
		{
			lightEffect = 0;
		}
		else
		{
			lightEffect = 1;
		}
	}
	else if (input == 5)
	{
		if (drawThatAxis == 1)
		{
			drawThatAxis = 0;
		}
		else
		{
			drawThatAxis = 1;
		}
	}
	glutPostRedisplay();	
}

void initMenu()
{
	//Création des menus et appel des callback
	glutCreateMenu(options_menu);
	glutAddMenuEntry("Molecule H2O", 1);
	glutAddMenuEntry("Molecule CH4", 2);	
	glutAddMenuEntry("caffeine.xyz File!", 3);
	glutAddMenuEntry("Toggle LightEffect", 4);
	glutAddMenuEntry("Toggle Axis", 5);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}