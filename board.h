#include <iostream>
#include <cstdio>
#include <cmath>
#include <GL/glut.h>
#include <string>
#include <cstring>
#include <unistd.h>
#include <stdlib.h>
using namespace std;

#define PI 3.141592653589
#define DEG2RAD(deg) (deg * PI / 180)
#define RAD2DEG(rad) (rad * 180 / PI)
#define ep 0.000001f
#define BLACK 1
#define BLUE 2

class Board
{
	public:
		//Board variables
		float hole_radius, friction, inner_rec_size, outer_rec_size, center_circle_radius, side_circle_radius, coef_rest,
		place_line_size, queen_circle_radius;

		//Board Contructor
		Board(
			float hr = 0.15f, float f = 0.0004f, float irs = 5.0f, float ors = 5.6f, float ccr = 0.5f, float scr = 0.1f, 
			float e = 1, float pls = 4.0f, float qcr = 0.1f
		     )
		{
			hole_radius = hr;
			friction = f;
			inner_rec_size = irs;
			outer_rec_size = ors;
			center_circle_radius = ccr;
			side_circle_radius = scr;
			coef_rest = e;
			place_line_size = pls;
			queen_circle_radius = qcr;
		}

		//draw a color filled rectangle given the color and width of rectangle
		void drawFilledRec(float len, float r, float g, float b)
		{
			glPushMatrix();
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glColor3f(r, g, b);
			glRectf(-len/2, -len/2, len/2, len/2);
			glPopMatrix();
		}

		//draw the boundary of a rectangle given its color and width 
		void drawEmptyRec(float len, float r, float g, float b)
		{
			glPushMatrix();
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glColor3f(r, g, b);
			glRectf(-len/2, -len/2, len/2, len/2);
			glPopMatrix();
		}

		//draw a color filled polygon
		void drawFilledPolygon(float len, float r, float g, float b)
		{
			glPushMatrix();
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glBegin(GL_POLYGON);
			glColor3f(r, g, b);
			glVertex2f(-len / 2, -len / 2 + hole_radius);
			glColor3f(r, g, b);
			glVertex2f(-len / 2 + hole_radius, -len / 2);
			glColor3f(r, g, b);
			glVertex2f(len / 2 - hole_radius, -len / 2);
			glColor3f(r, g, b);
			glVertex2f(len / 2, -len / 2 + hole_radius);
			glColor3f(r, g, b);
			glVertex2f(len / 2, len / 2 - hole_radius);
			glColor3f(r, g, b);
			glVertex2f(len / 2 - hole_radius, len / 2);
			glColor3f(r, g, b);
			glVertex2f(-len / 2 + hole_radius, len / 2);
			glColor3f(r, g, b);
			glVertex2f(-len / 2, len / 2 - hole_radius);
			glEnd();
			glPopMatrix();
		}

		//draw the circumference of a circle given its center coordinates and radius
		void drawEmptyCircle(float radius, float cx, float  cy, float  r, float  g, float  b)
		{
			glPushMatrix();
			glTranslatef(cx, cy, 0.0f);
			glColor3f(r, g, b);
			glBegin(GL_LINE_LOOP);
			for(int i = 0 ; i < 360 ; i++) 
			{
				glVertex2f(radius * cos(DEG2RAD(i)), radius * sin(DEG2RAD(i)));
			}
			glEnd();
			glPopMatrix();
		}

		//draw a color filled circle given its center coordinates and radius and color
		void drawFilledCircle(float radius, float cx, float  cy, float  r, float  g, float  b)
		{
			glPushMatrix();
			glTranslatef(cx, cy, 0.0f);
			glColor3f(r, g, b);
			glBegin(GL_TRIANGLE_FAN);
			for(int i = 0 ; i < 360 ; i++) 
			{
				glVertex2f(radius * cos(DEG2RAD(i)), radius * sin(DEG2RAD(i)));
			}
			glEnd();
			glPopMatrix();
		}

		// function to draw the sidecircles of board
		void drawSideCircles()
		{
			drawEmptyCircle(0.15f, -1.85f, -1.85f, 128.0/255.0, 0.0f, 0.0f);
			drawEmptyCircle(0.15f, -1.85f, 1.85f, 128.0/255.0, 0.0f, 0.0f);
			drawEmptyCircle(0.15f, 1.85f, -1.85f, 128.0/255.0, 0.0f, 0.0f);
			drawEmptyCircle(0.15f, 1.85f, 1.85f, 128.0/255.0, 0.0f, 0.0f);
		}

		//function to draw a line given end coordinates
		void drawLine(float x1, float  y1, float x2, float y2, float z1 = 0, float z2 = 0)
		{
			glBegin(GL_LINES);
			glVertex3f(x1, y1, z1);
			glVertex3f(x2, y2, z2);
			glEnd();
		}

		//declaration of the function to draw the line visible above stryker during its placement
		void drawStrykerLine();

		//initialize all the coins 
		void initializeCoins();

		//draw the place lines of board
		void drawPlaceLines()
		{
			drawLine(-1.85f, -2.0f, 1.85f, -2.0f);
			drawLine(2.0f, -1.85f, 2.0f, 1.85f);
			drawLine(1.85f, 2.0f, -1.85f, 2.0f);
			drawLine(-2.0f, 1.85f, -2.0f, -1.85f);
		}

		//resize the board
		void resize(int w, int h)
		{
			glViewport(0, 0, w, h);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluPerspective(45.0f, (float)w / (float)h, 0.1f, 200.0f);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
		}

		//function which draws the complete board
		void draw()
		{
			//draw outer rectangle
			drawFilledRec(outer_rec_size, 0.65f, 0.5f, 0.33f);

			//draw inner rectangle
			drawFilledPolygon(inner_rec_size, 255.0/255.0, 255.0/255.0, 224.0/255.0);

			//draw holes
			drawFilledCircle(hole_radius, 2.5f - hole_radius, 2.5f - hole_radius, 0.0f, 0.0f, 0.0f);
			drawFilledCircle(hole_radius, -2.5f + hole_radius, 2.5f - hole_radius, 0.0f, 0.0f, 0.0f);
			drawFilledCircle(hole_radius, -2.5f + hole_radius, -2.5f + hole_radius, 0.0f, 0.0f, 0.0f);
			drawFilledCircle(hole_radius, 2.5f - hole_radius, -2.5f + hole_radius, 0.0f, 0.0f, 0.0f);

			//draw center circle
			drawEmptyCircle(center_circle_radius, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);

			//draw queen circle
			drawEmptyCircle(queen_circle_radius, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
			
			//draw side circles
			drawSideCircles();

			glColor3f(128.0/255.0, 0.0/255.0, 0.0/255.0);

			//draw place lines
			drawPlaceLines();
		}
};
