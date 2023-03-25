#include <gl/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

GLfloat pre1DeltaX = 0.0, pre2DeltaX = 0.0;
GLfloat pre1DeltaY = 0.0, pre2DeltaY = 0.0;
GLfloat DeltaX = 1.0, dx = 0.0025;
GLfloat DeltaY = 0.0, dy = 0.0025;
GLfloat barX = 0.0, barY = 0.0;
char Title[] = "< P O N G >";
char Score[20] = "";
char Health[20] = "";
char Gameover[] = "G A M E O V E R";
char GameoverMessage[] = "PRESS <R> TO REPLAY.";
char GameoverExit[] = "PRESS <E> TO EXIT.";

int score = 0;
int health = 3;
int gameover = 0;
int game_speed = 2;
float bar_rgb[3] = { 0 };
float sphere_rgb[3] = { 0.0, 0.5, 0.8 };
short darkmode = 0;

void Menu(int entityID)
{
	if (entityID == 1)
	{
		darkmode = 0;
	}
	else if (entityID == 2)
	{
		darkmode = 1;
	}
	glutPostRedisplay();
}

void Init() {
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 2.0, -1.0, 1.0, 1.0, -1.0);

	GLint MenuID = glutCreateMenu(Menu);
	glutAddMenuEntry("다크모드 OFF", 1);
	glutAddMenuEntry("다크모드 ON", 2);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

}
void renderStrokeString(float x, float y, float z, float R, float G, float B, float scale, void* font, char* string)
{
	char* c;
	glColor3f(R, G, B);
	glPushMatrix();
		glTranslatef(x, y, z);
		glScalef(scale, scale, scale);
		for (c = string; *c != '\0'; c++) {
			glutStrokeCharacter(font, *c);
		}
	glPopMatrix();
}

void renderBitmapString(float x, float y, float R, float G, float B, void* font, char* string)
{
	char* c;
	glColor3f(R, G, B);
	glPushMatrix();
		glRasterPos2f(x, y);
		for (c = string; *c != '\0'; c++) {
			glutBitmapCharacter(font, *c);
		}
	glPopMatrix();
}
void Disp() {
	glClear(GL_COLOR_BUFFER_BIT);
	glPushMatrix();
		glTranslatef(DeltaX, DeltaY, 0.0);
		glColor3f(sphere_rgb[0], sphere_rgb[1], sphere_rgb[2]);
		//glutSolidSphere(0.04, 30, 30);
		GLUquadric * sphere = gluNewQuadric();
		gluQuadricDrawStyle(sphere, GLU_FILL);
		gluQuadricNormals(sphere, GLU_SMOOTH);
		gluQuadricOrientation(sphere, GLU_OUTSIDE);
		gluSphere(sphere, 0.04, 20, 20);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(pre1DeltaX, pre1DeltaY, 0.0);
		glColor3f(sphere_rgb[0], sphere_rgb[1], sphere_rgb[2]);
		//glutSolidSphere(0.04, 30, 30);
		GLUquadric* sphere1 = gluNewQuadric();
		gluQuadricDrawStyle(sphere1, GLU_FILL);
		gluQuadricNormals(sphere1, GLU_SMOOTH);
		gluQuadricOrientation(sphere1, GLU_OUTSIDE);
		gluSphere(sphere1, 0.04, 20, 20);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(pre2DeltaX, pre2DeltaY, 0.0);
		glColor3f(sphere_rgb[0], sphere_rgb[1], sphere_rgb[2]);
		//glutSolidSphere(0.04, 30, 30);
		GLUquadric* sphere2 = gluNewQuadric();
		gluQuadricDrawStyle(sphere2, GLU_FILL);
		gluQuadricNormals(sphere2, GLU_SMOOTH);
		gluQuadricOrientation(sphere2, GLU_OUTSIDE);
		gluSphere(sphere2, 0.04, 20, 20);
	glPopMatrix();
	
	pre2DeltaX = pre1DeltaX;
	pre1DeltaX = DeltaX;

	pre2DeltaY = pre1DeltaY;
	pre1DeltaY = DeltaY;
	//glColor3f(0.0, 0.5, 0.8);
	//glRectf(0.0 + DeltaX, -0.025 + DeltaY, 
	//		0.05 + DeltaX, 0.025 + DeltaY);
	
	glColor3f(bar_rgb[0], bar_rgb[1], bar_rgb[2]);//bar
	glRectf(0.15, barY + 0.2,
			0.2, barY - 0.2);
	//renderStrokeString(0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, GLUT_STROKE_MONO_ROMAN, mystring);
	glRasterPos2f(0, 0.9);



	if (darkmode == 0)
	{
		if (health == 3) glClearColor(1.0, 1.0, 1.0, 1.0);
		else if (health == 2) glClearColor(1.0, 0.9, 0.9, 1.0);
		else if (health == 1) glClearColor(1.0, 0.7, 0.7, 1.0);
		else if (health == 0) glClearColor(1.0, 0.5, 0.5, 1.0);
	}
	else if (darkmode == 1)
	{
		if (health == 3) glClearColor(0.0, 0.0, 0.0, 1.0);
		else if (health == 2) glClearColor(0.1, 0.0, 0.0, 1.0);
		else if (health == 1) glClearColor(0.3, 0.0, 0.0, 1.0);
		else if (health == 0) glClearColor(0.5, 0.0, 0.0, 1.0);
	}
	renderBitmapString(0.85, 0.9, 0.5, 0.5, 0.5, GLUT_BITMAP_HELVETICA_18, Title);
	//glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'A');
	if (gameover == 0) {
		sprintf_s(Score, "SCORE : %d", score);
		renderBitmapString(0.85, 0.05, 0.5, 0.5, 0.5, GLUT_BITMAP_HELVETICA_18, Score);
		sprintf_s(Health, "Health : %d", health);
		renderBitmapString(0.85, -0.05, 0.5, 0.5, 0.5, GLUT_BITMAP_HELVETICA_18, Health);
	}
	else {
		renderBitmapString(0.79, 0.05, 0.5, 0.5, 0.5, GLUT_BITMAP_TIMES_ROMAN_24, Gameover);
		renderBitmapString(0.70, -0.05, 0.5, 0.5, 0.5, GLUT_BITMAP_TIMES_ROMAN_24, GameoverMessage);
		renderBitmapString(0.75, -0.8, 0.5, 0.5, 0.5, GLUT_BITMAP_TIMES_ROMAN_24, GameoverExit);
		game_speed = 100;
		glClearColor(0.0, 0.0, 0.0, 0.5);
	}
	
	
	//glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 'A');
	//glFlush();
	glutSwapBuffers();
}
void Key(unsigned char key, int x, int y)
{
	if (key == 'w' && barY <= 0.8)
		barY += 0.05;
	if (key == 's' && barY >= -0.8)
		barY -= 0.05;
	if (key == 'r')
	{
		score = 0;
		health = 3;
		gameover = 0;
		bar_rgb[0] = 0;
		bar_rgb[1] = 0;
		bar_rgb[2] = 0;
		sphere_rgb[0] = 0.0;
		sphere_rgb[1] = 0.5;
		sphere_rgb[2] = 0.8;

		pre1DeltaX = 0.0, pre2DeltaX = 0.0;
		pre1DeltaY = 0.0, pre2DeltaY = 0.0;
		DeltaX = 1.0, dx = 0.0025;
		DeltaY = 0.0, dy = 0.0025;
		barX = 0.0, barY = 0.0;

		game_speed = 2;
	}
	if (key == 'e') exit(0);

	glutPostRedisplay();
}
void Mouse(int x, int y)
{
	barY = -(float)y * 0.01 + 2.5;
	if(barY >= -0.8 && barY <= 0.8)
	{
		barY = -(float)y * 0.01 +2.5 ;
		
		//barY = 250.0 -((float)y)*0.05;
	}
	else if (barY < -0.8) barY = -0.8;
	else if (barY >  0.8) barY = 0.8;
	glutPostRedisplay();
}

void Timer(int Value)
{
	float rx, ry;

	if (DeltaX >= 1.95)
	{
		rx = (float)(rand() % 5);
		DeltaX = DeltaX + dx + rx * 0.001;
	}
	else {
		DeltaX = DeltaX + dx;
	}
	if (DeltaY >= 0.95 || DeltaY <= -0.95)
	{
		ry = (float)(rand() % 5);
		DeltaY = DeltaY + dy + ry * 0.001;
	}
	else {
		DeltaY = DeltaY + dy;
	}

	if (DeltaX >= 1.95) 
	{
		dx = -0.0025;
	}
	if (DeltaX <= 0.24 && DeltaY <= barY + 0.2 && DeltaY >= barY - 0.2)
	{
		dx = 0.0025;
		score += 5;
		if(darkmode == 0)
		{
			bar_rgb[0] = (float)(rand() % 5) * 0.1;
			bar_rgb[1] = (float)(rand() % 5) * 0.1;
			bar_rgb[2] = (float)(rand() % 5) * 0.1;

			sphere_rgb[0] = (float)(rand() % 5) * 0.1;
			sphere_rgb[1] = (float)(rand() % 5) * 0.1;
			sphere_rgb[2] = (float)(rand() % 5) * 0.1;
		}
		else
		{
			bar_rgb[0] = 1.0 - (float)(rand() % 5) * 0.1;
			bar_rgb[1] = 1.0 - (float)(rand() % 5) * 0.1;
			bar_rgb[2] = 1.0 - (float)(rand() % 5) * 0.1;

			sphere_rgb[0] = 1.0 - (float)(rand() % 5) * 0.1;
			sphere_rgb[1] = 1.0 - (float)(rand() % 5) * 0.1;
			sphere_rgb[2] = 1.0 - (float)(rand() % 5) * 0.1;
		}
	}
	if (DeltaX <= 0.0) 
	{
		dx = 0.05;
		if (score >= 0)
		{
			score -= 10;
			if (score <= 0) score = 0;
		}
		else score = 0;

		health -= 1;
		if (health < 0) {
			gameover = 1;
		}
	}
	if (DeltaY >= 0.95) 
	{
		dy = -0.002;
	}
	if (DeltaY <= -0.95) 
	{
		dy = 0.002;
	}
	glutPostRedisplay();


	glutTimerFunc(game_speed, Timer, 0);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(500, 10);
	glutCreateWindow("                                                                                     P  O  N  G");
	Init();
	glutDisplayFunc(Disp);
	glutKeyboardFunc(Key);
	glutPassiveMotionFunc(Mouse);
	glutTimerFunc(game_speed, Timer, 1);
	glutMainLoop();
	return 0;
}
