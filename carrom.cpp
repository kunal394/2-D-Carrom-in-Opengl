#include <iostream>
#include <cstdio>
#include <cmath>
#include <GL/glut.h>
#include <string>
#include <cstring>
#include <unistd.h>
#include <stdlib.h>
#include "board.h"
using namespace std;

#define PI 3.141592653589
#define DEG2RAD(deg) (deg * PI / 180)
#define RAD2DEG(rad) (rad * 180 / PI)
#define ep 0.000001f
#define BLACK 1
#define BLUE 2

// Function Declarations
void drawScene();
void update(int value);
void initRendering();
void handleResize(int w, int h);
void handleKeypress1(unsigned char key, int x, int y);
void handleKeypress2(int key, int x, int y);
void handleMouseclick(int button, int state, int x, int y);
void handleMouseMove(int x, int y);
void handleMouseDrag(int x, int y);
void resetStryker();
void shot_power();
void renderBitmapString(float x, float y, float z, void *font,const char *string);
void timer(int value);
void selectColor(int button, int state, int x, int y);//GLUT_LEFT_BUTTON
void colorSelectWindow();//GLUT_LEFT_BUTTON
void printHelp();
int checkStrykerPlacement();

//Global Variables
int place = 1;//variable used to check if stryker is being currently placed
float stryker_line_angle = 0;//variable to rotate the angle by which stryker is to be hit
int pocketed_coins[10] = {0};// variable to keep track the pocketed coins
int flag[10][10] = {0};// variable to handle collision
int level = 0;// level of the shot power of stryker
int color = 0;// color of the ball selected
int enable = 1;// variable to check if stryke is correctly placed on the stryker line

class ScoreBoard
{
	public:
		int score;
		ScoreBoard(int s)
		{
			score = s;//score variable
		}
		void writeScore()
		{
			glPushMatrix();
			const int font = (int)GLUT_BITMAP_TIMES_ROMAN_24;
			glColor3f(1.0f, 0.0f, 0.0f);
			renderBitmapString(-5.0f, 0.0f, -2.0f, (void *)font, "Score");

			//Convert score to string 
			char *cur_score = (char *)malloc(100);
			string s = to_string(score);
			unsigned int i;
			for(i = 0; i < s.size(); i++) 
				 cur_score[i] = s[i];
			cur_score[i] = '\0';
			renderBitmapString(-5.0f, -0.4f, -2.0f, (void *)font, cur_score);
			glPopMatrix();
		}
};

Board B;
ScoreBoard score_board(30);

class Coin
{
	public:
		float radius, cx, cy, velx, vely, red_frac, green_frac, blue_frac, mass;
		int coin_no;

		//constructor used while declaration
		Coin()
		{
		}

		//Constructor used for initializing values
		Coin(float red, float green, float blue, float x, float y, int num, float r=0.1f, int m=1)
		{
			radius = r;
			cx = x;
			cy = y;
			red_frac = red;
			green_frac = green;
			blue_frac = blue;
			velx = 0.0f;
			vely = 0.0f;
			coin_no = num;
			mass = m;
		}

		//get the center coordinates of ball
		float getcx()
		{
			return cx;
		}
		float getcy()
		{
			return cy;
		}

		//draw coin on the board
		void draw() 
		{
			glPushMatrix();
			glTranslatef(cx, cy, 0.0f);
			glColor3f(red_frac, green_frac, blue_frac);
			glBegin(GL_TRIANGLE_FAN);
			for(int i = 0 ; i < 360 ; i++) 
			{
				glVertex2f(radius * cos(DEG2RAD(i)), radius * sin(DEG2RAD(i)));
			}
			glEnd();
			glPopMatrix();
		}

		//move coin
		void move()
		{
			float sin_angle;
			float cos_angle;
			if(velx == 0 && vely == 0)
			{
				cos_angle = sin_angle = 0;
			}
			else if(velx == 0)
			{
				sin_angle = 1;
				cos_angle = 0;
			}
			else if(vely == 0)
			{
				cos_angle = 1;
				sin_angle = 0;
			}
			else
			{
				sin_angle=fabs(vely/sqrt(velx*velx+vely*vely));
				cos_angle=fabs(velx/sqrt(velx*velx+vely*vely));
			}

			if(fabs(velx) > (B.friction * fabs(cos_angle)))
			{
				velx -= B.friction * fabs(cos_angle) * (velx/fabs(velx));
			}
			else 
			{
				velx = 0.0;	
			}

			if(fabs(vely) > (B.friction * fabs(sin_angle)))
			{
				vely -= B.friction * fabs(sin_angle) * (vely/fabs(vely));
			}
			else 
			{
				vely = 0.0;	
			}
			cx += velx;
			cy += vely;
		}

		//handle the collision of coin with the inner boundary of board
		void handle_boundary_collision()
		{
			if(cx + radius > B.inner_rec_size/2 || cx - radius < -B.inner_rec_size/2)
				velx *= -1;
			if(cy + radius > B.inner_rec_size/2 || cy - radius < -B.inner_rec_size/2)
				vely *= -1;
		}

		//function to rotate a vector by angle ang in anti-clockwise direction
		void rotate_vector(Coin &C, float ang)
		{
			float v1, v2;
			v1 = C.velx * cos(ang) - C.vely * sin(ang);
			v2 = C.velx * sin(ang) + C.vely * cos(ang);
			C.velx = v1;
			C.vely = v2;
			v1 = velx * cos(ang) - vely * sin(ang);
			v2 = velx * sin(ang) + vely * cos(ang);
			velx = v1;
			vely = v2;
		}

		void handle_pocketing()
		{
			float d1, d2, d3, d4;
			d1 = sqrt(pow(cx - 2.35, 2) + pow(cy - 2.35, 2));
			d2 = sqrt(pow(cx + 2.35, 2) + pow(cy - 2.35, 2));
			d3 = sqrt(pow(cx - 2.35, 2) + pow(cy + 2.35, 2));
			d4 = sqrt(pow(cx + 2.35, 2) + pow(cy + 2.35, 2));
			if(d1 < B.hole_radius || d2 < B.hole_radius || d3 < B.hole_radius || d4 < B.hole_radius)
			{
				velx = 0.0;
				vely = 0.0;
				pocketed_coins[coin_no] = 1;
				if(coin_no == 0)
					score_board.score -= 5;
				else if(coin_no == 1)
					score_board.score += 15;
				else if(coin_no == 2 || coin_no == 3 || coin_no == 4 || coin_no == 5)
				{
					if(color == BLACK)
						score_board.score += 10;
					else
						score_board.score -= 5;
				}
				else
				{
					if(color == BLUE)
						score_board.score += 10;
					else
						score_board.score -= 5;
				}
			}
		}

		//handle collision of one coin with another
		void handle_coin_coin_collision(Coin &C)
		{
			
			float slope_angle, center_dis, dx, dy;
			dx = cx - C.cx;
			dy = cy - C.cy;

			//center distance between the balls
			center_dis = sqrt(dx*dx + dy*dy);

			//slope of the line, connecting the center of the two colliding balls, with x-axis
			slope_angle = atan(dy/dx);

			// condition to check if two balls are colliding
			if(flag[coin_no][C.coin_no] == 0 && ((center_dis - radius - C.radius) <= ep))
			{
				flag[coin_no][C.coin_no] = 1;
				this->rotate_vector(C, -slope_angle);
				float v1, v2;
				v1 = (mass*velx + C.mass * C.velx + C.mass * B.coef_rest *(C.velx - velx)) / (mass + C.mass);
				v2 = (mass*velx + C.mass * C.velx + mass * B.coef_rest * (velx - C.velx)) / (mass + C.mass);
				velx = v1;
				velx *= 0.9;
				C.velx = v2;
				C.velx *= 0.9;
				this->rotate_vector(C, slope_angle);
			//	printf("velx:%f vely:%f C.velx:%f C.vely:%f\n", velx, vely, C.velx, C.vely);
			}
			else if(flag[coin_no][C.coin_no] == 1 && (center_dis > radius + C.radius))
				flag[coin_no][C.coin_no] = 0;
		}
};

Coin coins[10]; // coins of the carrom board

int main(int argc, char **argv)
{
	// Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glClearColor( 1.0, 1.0, 1.0, 1.0);

	int w = glutGet(GLUT_SCREEN_WIDTH);
	int h = glutGet(GLUT_SCREEN_HEIGHT);
	int windowWidth = w;
	int windowHeight = h;

	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition((w - windowWidth) / 2, (h - windowHeight) / 2);
	glutCreateWindow("Carrom");  // Setup the window
	
	pid_t pid = vfork();
	if(pid == 0)//child
	{
		initRendering();
		glutDisplayFunc(colorSelectWindow);
		glutIdleFunc(colorSelectWindow);
		glutReshapeFunc(handleResize);
		glutMouseFunc(selectColor);
		glutMainLoop();
	}
	else//parent
	{
		initRendering();
		B.initializeCoins();

		// Register callbacks
		glutDisplayFunc(drawScene);

		glutIdleFunc(drawScene);
		glutKeyboardFunc(handleKeypress1);
		glutSpecialFunc(handleKeypress2);
		glutMouseFunc(handleMouseclick);
		glutPassiveMotionFunc(handleMouseMove);//track mouse when no button is pressed
		glutMotionFunc(handleMouseDrag);//track mouse when any button is pressed
		glutReshapeFunc(handleResize);
		glutTimerFunc(1000, timer, 0);
		glutTimerFunc(10, update, 0);

		glutMainLoop();
	}

	return 0;
}

//draw the first window
void colorSelectWindow()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);   // Setting a background color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, -8.0f);
	printHelp();
	B.drawFilledCircle(1.0f, -3.0f, -2.0f, 0.0f, 0.0f, 0.0f);//r,cx,cy,r,g,b
	B.drawFilledCircle(1.0f, 3.0f, -2.0f, 0.0f, 0.0f, 1.0f);//r,cx,cy,r,g,b
	glPopMatrix();
	glutSwapBuffers();
}

void printHelp()
{
	glPushMatrix();
	const int font = (int)GLUT_BITMAP_TIMES_ROMAN_24;
	glColor3f(0.0f, 1.0f, 0.0f);
	renderBitmapString(-0.8f, 3.5f, -2.0f, (void *)font, "GAME INFO");
	renderBitmapString(-4.0f, 3.25f, -2.0f, (void *)font, "KEYBAORD CONTROLS");
	renderBitmapString(-4.0f, 3.0f, -2.0f, (void *)font, "'a' or 'A'-rotate the direction of stryker counter clockwise");
	renderBitmapString(-4.0f, 2.75f, -2.0f, (void *)font, "'c' or 'C'-rotate the direction of stryker counter clockwise");
	renderBitmapString(-4.0f, 2.50f, -2.0f, (void *)font, "'left' and 'right' arrow keys to move the stryker");
	renderBitmapString(-4.0f, 2.25f, -2.0f, (void *)font, "'up' and 'down' arrow keys to decide the power of hit");
	renderBitmapString(-4.0f, 1.75f, -2.0f, (void *)font, "MOUSE CONTROLS");
	renderBitmapString(-4.0f, 1.5f, -2.0f, (void *)font, "Click on the board to decide the direction of stryker");
	renderBitmapString(-4.0f, 1.25f, -2.0f, (void *)font, "Click on the power bar to decide the power of the hit");
	renderBitmapString(-4.0f, 0.75f, -2.0f, (void *)font, "GAME RULES");
	renderBitmapString(-4.0f, 0.25f, -2.0f, (void *)font, "Scoring queen - +15  Get stryker into hole - -5");
	renderBitmapString(-4.0f, 0.0f, -2.0f, (void *)font, "Scoring coin of your color - +10  Other color - -5");
	renderBitmapString(-4.0f, -0.5f, -2.0f, (void *)font, "Select The Color Of Your Coin to start the game");
	glPopMatrix();
}

//mouse handler function for first screen,selects color of the ball in first window
void selectColor(int button, int state, int x, int y)
{
	double w = (double)glutGet(GLUT_WINDOW_WIDTH);
	double h = (double)glutGet(GLUT_WINDOW_HEIGHT);
	double r = w/h;
	double X = ( ((double)x / w) * 2.0 - 1.0 ) * 3.3 * r;
	double Y = ( ((double)y / h) * 2.0 - 1.0 ) * 3.3 * (-1.0);
	float d1 = sqrt(pow(X - 3.0, 2) + pow(Y + 2.0, 2)); 
	float d2 = sqrt(pow(X + 3.0, 2) + pow(Y + 2.0, 2)); 
        if(button == GLUT_LEFT_BUTTON && state == 1)
        {
                if(d1 <= 1.0f)
                {
                        color = BLACK;
                        _exit(0);
                }
                else if(d2 <= 1.0f)
                {
                        color = BLUE;
                        _exit(0);
                }
        }
}

void Board::drawStrykerLine()
{
        float x, y;
        glColor3f(0.0, 0.0, 0.0);
        x = coins[0].getcx();
        y = coins[0].getcy();
        float angle_in_radian = DEG2RAD(stryker_line_angle);
        B.drawLine(x, y, x + 1.5*sin(angle_in_radian), y + 1.5*cos(angle_in_radian));
  }

void Board::initializeCoins()
{
        coins[0] = Coin(50.0/255.0, 135.0/255.0, 193.0/255.0, 0.0f, -1.85f, 0, 0.15f, 2);//stryker
        coins[1] = Coin(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1);//queen
        coins[2] = Coin(0.0f, 0.0f, 0.0f, 0.0f, -0.4f, 2);
        coins[3] = Coin(0.0f, 0.0f, 0.0f, 0.0f, 0.4f, 3);
        coins[4] = Coin(0.0f, 0.0f, 0.0f, 0.4f, 0.0f, 4);
        coins[5] = Coin(0.0f, 0.0f, 0.0f, -0.4f, 0.0f, 5);
        coins[6] = Coin(0.0f, 0.0f, 1.0f, 0.3f, 0.3f, 6);
        coins[7] = Coin(0.0f, 0.0f, 1.0f, -0.3f, 0.3f, 7);
        coins[8] = Coin(0.0f, 0.0f, 1.0f, 0.3f, -0.3f, 8);
        coins[9] = Coin(0.0f, 0.0f, 1.0f, -0.3f, -0.3f, 9);
}

void timer(int value)
{
        score_board.score--;
        glutTimerFunc(1000, timer, 0);
}

// Function to draw objects on the screen
void drawScene()
{
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glPushMatrix();
        glTranslatef(0.0f, 0.0f, -8.0f);
        B.draw();
        for(int i = 0; i < 10; i++)
        {
                if(pocketed_coins[i] == 0)
                        coins[i].draw();
        }
        if(place == 1)
        {
                B.drawStrykerLine();
                shot_power();
        }
        score_board.writeScore();
        glPopMatrix();
        glutSwapBuffers();
}


// Function to handle all calculations in the scene
// updated evry 10 milliseconds
void update(int value)
{
        int i, j;
        enable = checkStrykerPlacement();
        checkStrykerPlacement();
        for(i = 0; i < 10; i++)
        {
                coins[i].move();
 		coins[i].handle_boundary_collision();
	}
	for(i = 0; i < 10; i++)
	{
		if(pocketed_coins[i] == 0)
			coins[i].handle_pocketing();
	}
	for(i = 0; i < 10; i++)
	{
		for(j = i + 1; j < 10; j++)
		{
			if(pocketed_coins[i] != 1 && pocketed_coins[j] != 1)
				coins[i].handle_coin_coin_collision(coins[j]);
		}
	}
	int resetcond = 0;
	resetcond = resetcond || place;
	for(i = 0; i < 10; i++)
	{
		if(pocketed_coins[i] == 0)
			resetcond = resetcond || coins[i].velx || coins[i].vely;
	}
	if(resetcond == 0)
		resetStryker();	
	glutTimerFunc(10, update, 0);
}

//reset the stryker after every coin comes to stop
void resetStryker()
{
	pocketed_coins[0] = 0;
	place = 1;
	level = 0;
	coins[0].cx = 0.0f;
	coins[0].cy = -1.85f;
}

// Initializing some openGL 3D rendering options
void initRendering()
{
	glEnable(GL_DEPTH_TEST);        // Enable objects to be drawn ahead/behind one another
	glEnable(GL_COLOR_MATERIAL);    // Enable coloring
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);   // Setting a background color
}

// Function called when the window is resized
void handleResize(int w, int h)
{
	B.resize(w, h);
}

//Function to handle the shot power of stryker
void shot_power()
{
	glPushMatrix();
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor3f(1.0f, 1.0f, 1.0f);
	glRectf(3.5f, 2.0f, 4.5f, -2.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	for(float i = 1; i <= level ;i++)
	{
		glColor3f(i/10, 1 - (i/10), 0.0f);
		glRectf(3.5, -2 + 2*i/5, 4.5, -2 + 2*(i-1)/5);
	}
	glPopMatrix();
}

void renderBitmapString(float x, float y, float z, void *font,const char *string)
{ 
	const char *c; 
	glRasterPos3d(x, y, z); 
	for(c = string; *c != '\0'; c++) 
		glutBitmapCharacter(font, *c); 
}

//to check if any other coin is present on the placement line
int checkStrykerPlacement()
{
	int i;
	if(place == 0)
		return 0;
	for(i = 1; i < 10; i++)
	{
		float d = sqrt(pow(coins[0].cx - coins[i].cx, 2) + pow(coins[0].cy - coins[i].cy, 2));
		if(d < (coins[0].radius + coins[i].radius))
		{
			coins[0].red_frac = 1.0f;
			coins[0].green_frac = 0.0f;
			coins[0].blue_frac = 0.0f;
			return 0;
		}
	}
	coins[0].red_frac = 50.0/255.0;
	coins[0].green_frac = 135.0/255.0;
	coins[0].blue_frac = 193.0/255.0;
	return 1;
}

//handle the direction of stryker using keyboard
void handleKeypress1(unsigned char key, int x, int y)
{
	if (key == 27)
	{
		exit(0);     // escape key is pressed
	}
	if (key == 67 || key == 99)
	{
		if(stryker_line_angle <= 90)
			stryker_line_angle += 1;
	}
	if (key == 65 || key == 97)
	{
		if(stryker_line_angle >= -90)
			stryker_line_angle -= 1;
	}
	if (key == 32)
	{
		if(enable == 1)
		{
			coins[0].velx = (0.04 + (0.08 * level/10)) * sin(DEG2RAD(stryker_line_angle));
			coins[0].vely = (0.04 + (0.08 * level/10)) * cos(DEG2RAD(stryker_line_angle));
			place = 0;
		}
	}
	
}

//handle keyboard functionalities of the game, move stryker usig keyboard
void handleKeypress2(int key, int x, int y)
{
	if (key == GLUT_KEY_LEFT)
		if(place == 1 && ((coins[0].cx + coins[0].radius) >= -(B.place_line_size/2 - 0.35f)))
			coins[0].cx -= 0.05f;
	if (key == GLUT_KEY_RIGHT)
		if (place == 1 && ((coins[0].cx + coins[0].radius) <= (B.place_line_size/2 - 0.05)))
			coins[0].cx += 0.05f;
	if (key == GLUT_KEY_UP)
		if(level < 10)
			level += 1;
	if (key == GLUT_KEY_DOWN)
		if(level > 0)
			level -= 1;
}

//handle mouse move, to manage the direction of the stryker using mouse
void handleMouseMove(int x, int y)
{
	if(place == 1)
	{
		double w = (double)glutGet(GLUT_WINDOW_WIDTH);
		double h = (double)glutGet(GLUT_WINDOW_HEIGHT);
		double r = w/h;
		double X = ( ((double)x / w) * 2.0 - 1.0 ) * 3.3 * r;
		double Y = ( ((double)y / h) * 2.0 - 1.0 ) * 3.3 * (-1.0);
		float sin_ang = (X - coins[0].cx) / sqrt(pow(coins[0].cx - X, 2) + pow(coins[0].cy - Y, 2));
		//float cos_ang = (Y - coins[0].cy) / sqrt(pow(coins[0].cx - X, 2) + pow(coins[0].cy - Y, 2));
		glColor3f(0.0, 0.0, 0.0);
		if(X < 2.0 && X > -2.0 && Y > -1.7 && Y < 2.5)
		{
			level = 1 + int(10.0*(Y + 1.7) / 4.2);
			stryker_line_angle = RAD2DEG(asin(sin_ang));
		}
	//	printf("x:%.6f y:%.6f getting mouse\n",x,y);
	}
}

//handle mouse drag while muse button is pressed down, to move stryker using mouse
void handleMouseDrag(int x, int y)
{
	if(place == 1)
	{
		double w = (double)glutGet(GLUT_WINDOW_WIDTH);
		double h = (double)glutGet(GLUT_WINDOW_HEIGHT);
		double r = w/h;
		double X = ( ((double)x / w) * 2.0 - 1.0 ) * 3.3 * r;
		double Y = ( ((double)y / h) * 2.0 - 1.0 ) * 3.3 * (-1.0);
		if(X < 1.85 && X > -1.85 && Y > -2.5 && Y < -1.85)
			coins[0].cx = X;
	}
}

//handle ,ouse click to manage level of the power and to shoot the stryker
void handleMouseclick(int button, int state, int x, int y)
{
	double w = (double)glutGet(GLUT_WINDOW_WIDTH);
	double h = (double)glutGet(GLUT_WINDOW_HEIGHT);
	double r = w/h;
	double X = ( ((double)x / w) * 2.0 - 1.0 ) * 3.3 * r;
	double Y = ( ((double)y / h) * 2.0 - 1.0 ) * 3.3 * (-1.0);
	float sin_ang = (X - coins[0].cx) / sqrt(pow(coins[0].cx - X, 2) + pow(coins[0].cy - Y, 2));
	float cos_ang = (Y - coins[0].cy) / sqrt(pow(coins[0].cx - X, 2) + pow(coins[0].cy - Y, 2));
//	if(button == GLUT_LEFT_BUTTON && state == 1)//state=1=>mouse clicked and up,state=0=>mouse down
	if(state == GLUT_DOWN && button == GLUT_LEFT_BUTTON)
	{
		if(X >= 3.5 && X <= 4.5 && Y >= -2.0 && Y <= 2.0)
		{
			level += 1;
			level %= 11;
		}
		if(X >= -2.5 && X <= 2.5 && Y >= -1.7 && Y <= 2.5)
		{
			if(enable == 1)
			{
				coins[0].velx = (0.04 + (0.08 * level/10)) * sin_ang;
				coins[0].vely = (0.04 + (0.08 * level/10)) * cos_ang;
				place = 0;
			}
		}	
	}
}
