#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <fstream>
#include <vector>
#include <math.h>
#include <time.h>
#include <string>
#include <string.h>
#include "SOIL.h"



enum Color { RED, GREEN, BLUE, TEXT1, TEXT2, TEXT3 ,BLACK};


using namespace std;
float eyex = -80.0f;
float eyey = 20.0f;
float eyez = 0.0f;
float theta = 0.0f;
float anglex = 0.0f;
float angley = 0.0f;

int viewState = 0;
int transparency=0;
int Posx = 0;  // Posx,posz,Posy  hold the coordinates of the player
int Posz = 0; 
int Posy = 0;


float deltaAnglex = 0.0f;
float deltaAngley = 0.0f;
float deltaMove = 0;
float xOrigin = -1;
float yOrigin = -1;
float lx = 0.0f;
float lz = -1.0f;
float ly = 0.0f;

float camPlayerx;
float camPlayery;
float camPlayerz;

float prob;
int counter = 1;
FILE *fp;
GLfloat centerx = 0.0f;
GLfloat centery = 0.0f;
GLfloat centerz = -1.0f;

int levels, dimension;
int moves = 0;
int score = 0;
int totalHammers = 0;
int hammers;
GLUquadric* qobj;
vector<vector<vector<string> > > maze;

GLuint texture;
GLuint tex_2d;
GLuint tex_2d1;
GLuint tex_2d2;
GLuint tex_floor;

void readFile(){
	ifstream mazeFile;
	char dum;
	string word;
	int i, j;


	char* fileName = "file.maz";


	mazeFile.open(fileName, ios::in);

	if (mazeFile.fail()){
		cout << "could not open file";
	}
	mazeFile >> dum;
	mazeFile >> dum;
	mazeFile >> levels;
	cout << "The levels are : " << levels << "\n";
	maze.resize(levels);
	eyey = 20 * levels; //camera level of y axis to look on top of all the maze

	mazeFile >> dum;
	mazeFile >> dum;
	mazeFile >> dimension;
	cout << "The dimension is : " << dimension << "\n";

	mazeFile >> dum;
	mazeFile >> dum;
	mazeFile >> hammers;
	cout << "The hammers are : " << hammers;
	totalHammers = hammers;

	//create array
	for (i = 0; i < levels; ++i) {
		maze[i].resize(dimension);

		for (j = 0; j < dimension; ++j)
			maze[i][j].resize(dimension);
	}

	for (int i = 0; i<levels; i++){

		mazeFile >> dum;
		mazeFile >> dum;
		mazeFile >> dum;
		mazeFile >> dum;
		mazeFile >> dum;
		mazeFile >> dum;


		srand((unsigned int)time(NULL));
		for (int row = 0; row<dimension; ++row){
			for (int column = 0; column<dimension; ++column){
				mazeFile >> word;
				if (i == 0 && word == "E"){
					prob = ((double)rand() / (RAND_MAX));
					if (prob <= (1.0 / counter)){ //Reservoir Sampling
						Posx = row;
						Posz = column;
					}
					counter++;
				}
				maze[i][row][column] = word;
			}
		}


	}

}


void LoadGLTextures()					
{
	
	glEnable( GL_TEXTURE_2D );
	glGenTextures(1,&tex_2d);
    tex_2d = SOIL_load_OGL_texture
    (
    	"T1.png",
    	SOIL_LOAD_AUTO,
    	SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT);
    
    if (tex_2d==0){
		cout<<"fatal error while loading the texture";
	}
	
	glGenTextures(1,&tex_2d1);
	
    tex_2d1 = SOIL_load_OGL_texture
    (
    	"T2.png",
    	SOIL_LOAD_AUTO,
    	SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
    
    if (tex_2d1==0){
		cout<<"fatal error while loading the texture";
	} 
	
	glGenTextures(1,&tex_2d2);
	
    tex_2d2 = SOIL_load_OGL_texture
    (
    	"T3.png",
    	SOIL_LOAD_AUTO,
    	SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
    
    if (tex_2d2==0){
		cout<<"fatal error while loading the texture";
	} 
	
	glGenTextures(1,&tex_floor);
	//glEnable(GL_TEXTURE_2D);
    tex_floor = SOIL_load_OGL_texture
    (
    	"floor.jpg",
    	SOIL_LOAD_AUTO,
    	SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
    
    if (tex_floor==0){
		cout<<"fatal error while loading the texture";
	} 
	
	
	glDisable(GL_TEXTURE_2D );
	
	
	
	
	
								
}

void drawTextureCube(int x ,int y,int z,int textureType){
	
	glEnable( GL_TEXTURE_2D );
	
	glPushMatrix();
	if(viewState==0 && transparency==1){//case where trasparency is enabled
		glColor4f (0.5f,0.5f,0.5f,0.4f);
	}	
	else{
		glColor3f (0.5f,0.5f,0.5f);
	}
	if (textureType == TEXT1){
		glBindTexture ( GL_TEXTURE_2D, tex_2d);
	}else if (textureType == TEXT2){
		glBindTexture ( GL_TEXTURE_2D, tex_2d1);
	}else if (textureType == TEXT3){
		glBindTexture ( GL_TEXTURE_2D, tex_2d2);
	}
	
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  
	
	glBegin(GL_POLYGON); // Back
        glTexCoord2f(0,0);
        glVertex3f( x, y, z );
        glTexCoord2f(1,0);
        glVertex3f( x+5, y, z );
        glTexCoord2f(1,1);
        glVertex3f( x+5, y+5, z );
        glTexCoord2f(0,1);
        glVertex3f( x, y+5, z);
    glEnd();
    
      glBegin(GL_POLYGON); // Front
        glNormal3f( 0.0, 0.0, 1.0);
        glTexCoord2f(0,0);
        glVertex3f( x, y, z+5 );
        glTexCoord2f(1,0);
        glVertex3f( x+5, y, z+5 );
        glTexCoord2f(1,1);
        glVertex3f( x+5, y+5, z+5 );
        glTexCoord2f(0,1);
        glVertex3f( x, y+5,z+5 );
    glEnd();
    
    
    glBegin(GL_POLYGON); // Left
        glNormal3f( -1.0, 0.0, 0.0);
        glTexCoord2f(0,0);
        glVertex3f( x, y, z );
        glTexCoord2f(1,0);
        glVertex3f( x, y, z+5 );
        glTexCoord2f(1,1);
        glVertex3f( x, y+5, z+5 );
        glTexCoord2f(0,1);
        glVertex3f( x, y+5, z );
    glEnd();
    
    
    glBegin(GL_POLYGON); // Right
        glNormal3f( 1.0, 0.0, 0.0);
        glTexCoord2f(0,0);
        glVertex3f( x+5, y, z );
        glTexCoord2f(1,0);
        glVertex3f( x+5, y, z+5 );
        glTexCoord2f(1,1);
        glVertex3f( x+5, y+5, z+5 );
        glTexCoord2f(0,1);
        glVertex3f( x+5, y+5, z );
    glEnd();
    
    
     glBegin(GL_POLYGON); // Top
        glNormal3f( 0.0, 1.0, 0.0);
        glTexCoord2f(0,0);
        glVertex3f( x, y+5, z );
        glTexCoord2f(1,0);
        glVertex3f( x+5, y+5, z );
        glTexCoord2f(1,1);
        glVertex3f( x+5, y+5, z+5 );
        glTexCoord2f(0,1);
        glVertex3f( x, y+5, z+5 );
    glEnd();
    
    glBegin(GL_POLYGON); // Bottom
        glNormal3f( 0.0, -1.0, 0.0);
        glTexCoord2f(0,0);
        glVertex3f( x, y, z );
        glTexCoord2f(1,0);
        glVertex3f( x+5, y, z );
        glTexCoord2f(1,1);
        glVertex3f( x+5, y, z+5 );
        glTexCoord2f(0,1);
        glVertex3f( x, y, z+5 );
    glEnd();
    glDisable( GL_TEXTURE_2D );
	
	glPopMatrix();
	
}


void calculateScore(){
	score = (dimension*dimension) - (moves * 10) - (totalHammers - hammers) * 50;
}

void renderBitmapString(int x, int y, int z, char *string){//used for displaying text to screen
	char *c;
	glRasterPos3i(x, y, z);
	for (c = string; *c != '\0'; c++){
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
	}
}




void drawOutline(int x, int y, int z){  //This function is for the outline of the cubes

	glColor3f(0, 0, 0);

	glBegin(GL_LINES);   //Front 
	glVertex3f(x, y, z);
	glVertex3f(x + 5, y, z);
	glEnd();

	glBegin(GL_LINES);
	glVertex3f(x + 5, y, z);
	glVertex3f(x + 5, y + 5, z);
	glEnd();

	glBegin(GL_LINES);
	glVertex3f(x + 5, y + 5, z);
	glVertex3f(x, y + 5, z);
	glEnd();

	glBegin(GL_LINES);
	glVertex3f(x, y + 5, z);
	glVertex3f(x, y, z);
	glEnd();


	glBegin(GL_LINES); // Back
	glVertex3f(x, y, z + 5);
	glVertex3f(x + 5, y, z + 5);
	glEnd();

	glBegin(GL_LINES);
	glVertex3f(x + 5, y, z + 5);
	glVertex3f(x + 5, y + 5, z + 5);
	glEnd();

	glBegin(GL_LINES);
	glVertex3f(x + 5, y + 5, z + 5);
	glVertex3f(x, y + 5, z + 5);
	glEnd();

	glBegin(GL_LINES);
	glVertex3f(x, y + 5, z + 5);
	glVertex3f(x, y, z + 5);
	glEnd();



	glBegin(GL_LINES); // Right 
	glVertex3f(x + 5, y, z);
	glVertex3f(x + 5, y, z + 5);
	glEnd();

	glBegin(GL_LINES);
	glVertex3f(x + 5, y + 5, z + 5);
	glVertex3f(x + 5, y + 5, z);
	glEnd();



	glBegin(GL_LINES); // Left
	glVertex3f(x, y, z);
	glVertex3f(x, y, z + 5);
	glEnd();



	glBegin(GL_LINES);
	glVertex3f(x, y + 5, z + 5);
	glVertex3f(x, y + 5, z);
	glEnd();



}



void drawCube(int x, int y, int z, int color){//draw untextured cubes
	if (viewState==1 || transparency==0){
		if (color == RED){
			glColor3f(0.6f, 0.0f, 0.0f);
		}
		else if (color == GREEN){
			glColor3f(0.0f, 0.5f, 0.0f);
		}
		else if (color == BLUE){
			glColor3f(0.0f, 0.0f, 0.5f);
		}
		else if (color == BLACK){
			glColor3f(0.1f, 0.1f, 0.1f);
		}
	}
	else if(transparency==1){
		
			if (color == RED){
				glColor4f(0.6f, 0.0f, 0.0f,0.4f);
			}
			else if (color == GREEN){
				glColor4f(0.0f, 0.6f, 0.0f,0.4f);
			}
			else if (color == BLUE){
				glColor4f(0.0f, 0.0f, 0.6f,0.4f);
			}
			else if (color == BLACK){
				glColor4f(0.1f, 0.1f, 0.1f,0.4f);
			}
		}
		
	
	glPushMatrix();
	
	glBegin(GL_QUADS);  //Front 
	glNormal3f( 0.0, 0.0, 1.0);
	glVertex3f(x, y, z);
	glVertex3f(x + 5, y, z);
	glVertex3f(x + 5, y + 5, z);
	glVertex3f(x, y + 5, z);
	glEnd();



	glBegin(GL_QUADS);  //Back
	glNormal3f( 0.0, 0.0, -1.0);
	glVertex3f(x, y, z + 5);
	glVertex3f(x + 5, y, z + 5);
	glVertex3f(x + 5, y + 5, z + 5);
	glVertex3f(x, y + 5, z + 5);
	glEnd();

	glBegin(GL_QUADS);  //Right 
	        glNormal3f( 1.0, 0.0, 0.0);
	glVertex3f(x + 5, y, z);
	glVertex3f(x + 5, y, z + 5);
	glVertex3f(x + 5, y + 5, z + 5);
	glVertex3f(x + 5, y + 5, z);
	glEnd();

	glBegin(GL_QUADS); // Left
	 glNormal3f( -1.0, 0.0, 0.0);
	glVertex3f(x, y, z);
	glVertex3f(x, y, z + 5);
	glVertex3f(x, y + 5, z + 5);
	glVertex3f(x, y + 5, z);
	glEnd();

	glBegin(GL_QUADS); //Top
	  glNormal3f( 0.0, 1.0, 0.0); 
	glVertex3f(x, y + 5, z);
	glVertex3f(x + 5, y + 5, z);
	glVertex3f(x + 5, y + 5, z + 5);
	glVertex3f(x, y + 5, z + 5);
	glEnd();


	glBegin(GL_QUADS);
	glNormal3f( 0.0, -1.0, 0.0); 
	glVertex3f(x, y, z);
	glVertex3f(x + 5, y, z);
	glVertex3f(x + 5, y, z + 5);
	glVertex3f(x, y, z + 5);
	glEnd();

	if (viewState == 1 || transparency==0){
		drawOutline(x, y, z);
	}
	//glFlush();

	//glEnd();




	glPopMatrix();


}


void init(){
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_COLOR_MATERIAL);
	//glColorMaterial(GL_FRONT, GL_DIFFUSE);
	//glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_NORMALIZE);
	glLineWidth(1);
	glShadeModel(GL_SMOOTH);
	qobj = gluNewQuadric();
	gluQuadricNormals(qobj, GLU_SMOOTH);
}

void drawCylinder(int x, int y, int z){
	glPushMatrix();
	glColor3f(0.7, 0.8, 0.8);
	glTranslatef(x + 2.5, y + 5, z + 2.5);
	glRotatef(90, 1, 0, 0);
	gluCylinder(qobj, 2.5, 2.5, 5, 30, 32);

	glPopMatrix();


}
void renderScene(void) {

	//Clear Color and Depth Buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	

	glMatrixMode(GL_MODELVIEW);
	//Reset transformations

	glLoadIdentity();

	

	//Set the camera
	if (viewState == 0){
		
		GLfloat ambientLight[] = {0.5f, 0.5f,0.5f,1.0f }; //set DiffuseLight[] to the specified values
		float MatDiffuse[] = {0.8f, 0.5f, 0.5f,1.0f};
		//first light
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MatDiffuse);
		GLfloat lightPosition1[] = {-30.0f,15*levels+20,20.0f, 0.0f };
		

		glEnable (GL_LIGHT0);
		glEnable(GL_LIGHTING);
		
		gluLookAt(eyex, eyey, eyez,
			(dimension*5)/2, centery, (dimension*5)/2,
			0.0f, 1.0f, 0.0f);
	}
	else if (viewState == 1){
		//Lighting that follows the player
		GLfloat diffuseLight[] = {0.6f, 0.6f,0.6f};
		glLightfv(GL_LIGHT1, GL_DIFFUSE,diffuseLight);
		GLfloat lightPosition1[] = {camPlayerx,camPlayery+3,camPlayerz, 1.0f };
		glLightfv(GL_LIGHT1, GL_POSITION, lightPosition1);
		glEnable (GL_LIGHT1);
		
		
		gluLookAt(eyex, eyey, eyez,
			eyex - lx, eyey + ly, eyez - lz,
			0.0f, 1.0f, 0.0f);
	}
	else {
		//glDisable(GL_LIGHTING);
		gluLookAt(-100,levels*20.0f, -20,
			0, 0, 0,
			0.0f, 1.0f, 0.0f);

	}
	
	
	glEnable( GL_TEXTURE_2D );
	glBindTexture ( GL_TEXTURE_2D, tex_floor );

	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
	glNormal3f( 0.0, 1.0, 0.0);
    glTexCoord2f(0,0);
	glVertex3f(0, 0.0f, 0);
	glTexCoord2f(1,0);
	glVertex3f(dimension * 5, 0.0f, 0);
	glTexCoord2f(1,1);
	glVertex3f(dimension * 5, 0.0f, dimension * 5);
	glTexCoord2f(0,1);
	glVertex3f(0, 0.0f, dimension * 5);
	glEnd();
	glDisable( GL_TEXTURE_2D );
		
	for (int i = 0; i<levels; ++i){
		for (int r = 0, width = 0; r<dimension; ++r, width += 5){
			for (int c = 0, depth = 0; c<dimension; ++c, depth += 5){
				if (viewState == 0){
					if (r == Posx && c == Posz && Posy == i){
						drawCylinder(width, i * 3 , depth);
						camPlayerx = width;
						camPlayerz = depth;
						camPlayery = i * 3 + 3;//level number*standard y pos
					}
				}
				
				if (maze[i][r][c] == "R"){
					drawCube(width, i * 5, depth, RED);
				}
				else if (maze[i][r][c] == "B"){
					drawCube(width, i * 5, depth, BLUE);
				}
				else if (maze[i][r][c] == "G"){
					drawCube(width, i * 5, depth, GREEN);
				}
				
				else if (maze[i][r][c] == "T1")    {
					drawTextureCube(width,i*5,depth,TEXT1);
				}
				
				else if (maze[i][r][c] == "T2"){
					drawTextureCube(width,i*5,depth,TEXT2);
				}
				
				else if (maze[i][r][c] == "T3"){
					drawTextureCube(width,i*5,depth,TEXT3);
				}
				else if (maze[i][r][c] == "W"){
					drawCube(width, i * 5, depth, BLACK);
				}
				



			}
		}
	}
	if (viewState == 2){
		char screenScore[20] = "SCORE:  ";
		char StringScore[100];
		int score2 = score;
		sprintf(StringScore, "%d", score2);
		// Assemble the string
		strcat(screenScore, StringScore);
		glColor3f(0.0f, 0.0f, 1.0f);
		renderBitmapString(-60, 40,0, screenScore);
	}

	glutSwapBuffers();
}

void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if (h == 0)
		h = 1;
	float ratio = 1.0* w / h;

	// Use the Projection Matrix
	glMatrixMode(GL_PROJECTION);

	// Reset Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set the correct perspective.
	gluPerspective(45, ratio, 0.1, 10000);

	// Get Back to the Modelview
	glMatrixMode(GL_MODELVIEW);
}


int calculatePlayerPositionx(int eyex){
	int x, i;
	x = eyex - (int)eyex % 5;
	i = x / 5;
	return i;
}

int calculatePlayerPositionz(int eyez){
	int z, j;
	z = eyez - (int)eyez % 5;
	j = z / 5;
	return j;
}
void processNormalKeys(unsigned char key, int x, int y) {

	if (key == 27){
		exit(0);
	}

	else if ((key == 'r' || key == 'R') && viewState == 0){
		theta += 0.1;
		eyex = (dimension*5)/2 + 2.5*levels * 15 * sin(theta);
		eyez = (dimension*5)/2 + 2.5*levels * 15 * (-cos(theta));
		eyey = levels * 20;
	}

	else if (key == 'V' || key == 'v' && viewState != 2){
		viewState = 1 - viewState;

		if (viewState == 0){//out view
			eyex = -60.0f;
			eyey = 20.0*levels;
			eyez = 20.0f;
			centerx = 0.0f;
			centery = 0.0f;
			centerz = 0.0f;
			glutPostRedisplay();

		}
		else {
			eyex = camPlayerx;
			eyez = camPlayerz;
			eyey = camPlayery;
			glutPostRedisplay();
		}


	}
	if ((key == 'w' || key == 'W') && viewState == 1){
		//used for finding the next block that the player will go
		float tempx, tempz;
		//used for finding the current block that the player is
		int curplayeri, curplayerj;
		int i, j;
		tempx = eyex - 3 * lx; // Calculates how far the player will go in the next block
		if (lz == -5.0){
			tempz = eyez;
		}
		else{
			tempz = eyez - 3 * lz;
		}

		
		curplayeri = calculatePlayerPositionx(eyex);
		curplayerj = calculatePlayerPositionz(eyez);

		i = calculatePlayerPositionx(tempx);  //Calculates the position in front of the player
		j = calculatePlayerPositionz(tempz);




		if (!(tempx<0 || tempz<0 || tempx>dimension * 5 || tempz>dimension * 5)){  //Checks for posible collision
			if (maze[Posy][i][j] == "E"){
				eyex -= lx*0.5;//sets the velocity(speed)
				eyez -= lz*0.5;
				camPlayerx = eyex;
				camPlayerz = eyez;
				camPlayery = eyey;
				//the new block that the player moved
				int newi = calculatePlayerPositionx(eyex);
				int newj = calculatePlayerPositionx(eyez);
				Posx = newi; // Posx and Posz hold the coordinates of the player
				Posz = newj;

				//if player moved to a new block
				if (newi != curplayeri || newj != curplayerj){
					moves++;
					int fall = 1; // Fall determines how many blocks are empty so the player falls down
					while (Posy - fall >= 0 && maze[Posy - fall][newi][newj] == "E"){
						Posy--;
						eyey -= 5;
						fall++;
					}
				}



			}
			else if (maze[Posy][i][j] == "W"){
				for (int k = 0; k < dimension; k++){
					for (int l = 0; l < dimension; l++){
						if ("W" == maze[Posy][k][l]){
							if (k != i || l != j){
								Posx = k;
								Posz = l;
								eyex = k * 5;
								eyez = l * 5;
								camPlayerx = k * 5;
								camPlayerz = l * 5;
							}
						}
					}
				}

			}
		}

	}
	else if ((key == 's' || key == 'S') && viewState == 1){

		float tempx, tempz;
		int i, j;
		tempx = eyex + lx * 3; // Calculates how far the player will go in the next block
		if (lz == -5.0){
			tempz = eyez;
		}
		else{
			tempz = eyez + 3 * lz;
		}


		int curplayeri = calculatePlayerPositionx(eyex);
		int curplayerj = calculatePlayerPositionz(eyez);

		i = calculatePlayerPositionx(tempx);
		j = calculatePlayerPositionz(tempz);

		if (!(tempx<0 || tempz<0 || tempx>dimension * 5 || tempz>dimension * 5)){
			if (maze[Posy][i][j] == "E"){
				eyex += lx*0.5;
				eyez += lz*0.5;
				camPlayerx = eyex;
				camPlayerz = eyez;
				camPlayery = eyey;
				int newi = calculatePlayerPositionx(eyex);
				int newj = calculatePlayerPositionx(eyez);
				Posx = newi;
				Posz = newj;


				if (newi != curplayeri || newj != curplayerj){
					moves++;
					int fall = 1; // Fall determines how many blocks are empty so the player falls down
					while (Posy - fall >= 0 && maze[Posy - fall][newi][newj] == "E"){
						Posy--;
						eyey -= 5;
						fall++;
					}
				}


			}
		}
	}

	else if ((key == 'h' || key == 'H') && viewState == 1 && hammers>0){
		int y1 = 0; //keeps the current level

		int x1 = calculatePlayerPositionx(eyex);
		int z1 = calculatePlayerPositionz(eyez);
		if (ly > 0.5 && Posy != levels - 1){//if player looks up
			y1 = 1;//increment the levels variable by 1
		}

		if (ly>-0.95){  // Check how high or low the player looks 


			if (lx >= 0.95){//camera looks back

				if (x1 - 1 >= 0){ // To be inside the range of the array
					if (maze[Posy + y1][x1 - 1][z1] == "E") {
						if (x1 - 2 >= 0){//check to be inside of bounds
							if (maze[Posy + y1][x1 - 2][z1] != "E"){
								maze[Posy + y1][x1 - 2][z1] = "E";  //Delete the block at distance 2
								hammers--;
							}
						}
					}
					else {
						maze[Posy + y1][x1 - 1][z1] = "E"; // Else delete the block at distance 1
						hammers--;
					}
				}
			}
			else if (lx < 0.95 && lx >= 0.25 && lz < 0){//camera looks back and right

				if (x1 - 1 >= 0 && z1 + 1 < dimension && maze[Posy + y1][x1 - 1][z1 + 1] != "E"){ // Check to be inside of bounds

					maze[Posy + y1][x1 - 1][z1 + 1] = "E";
					hammers--;
				}

			}
			else if (lx <0.95 && lx >= 0.25 && lz >0){//camera looks back and left

				if (x1 - 1 >= 0 && z1 - 1 >= 0 && maze[Posy][x1 - 1][z1 - 1] != "E"){ // Check to be inside of bounds

					maze[Posy + y1][x1 - 1][z1 - 1] = "E";
					hammers--;
				}
			}
			else if (lx <= -0.95){//camera looks in front
		
				if (x1 + 1 < dimension){ // To be inside the range of the array

					//TODO : CHECK FOR BLACK BLOCKS
					if (maze[Posy + y1][x1 + 1][z1] == "E") {
						if (x1 + 2 < dimension && maze[Posy + y1][x1 + 2][z1] != "E"){
							maze[Posy + y1][x1 + 2][z1] = "E";//destroy in distance 2
							hammers--;
						}
					}
					else {
						maze[Posy + y1][x1 + 1][z1] = "E";//destroy in distance 1
						hammers--;
					}
				}
			}
			else if (lx > -0.95 && lx <= -0.25 && lz < 0){//camera looks front and right
		
				if (x1 + 1 < dimension && z1 + 1 < dimension && maze[Posy + y1][x1 + 1][z1 + 1] != "E"){


					maze[Posy + y1][x1 + 1][z1 + 1] = "E";
					hammers--;
				}

			}
			else if (lx > -0.95 && lx <= -0.25 && lz > 0) {//camera looks front and left

				if (x1 + 1 < dimension && z1 - 1 >= 0 && maze[Posy + y1][x1 + 1][z1 - 1] != "E"){

					maze[Posy + y1][x1 + 1][z1 - 1] = "E";
					hammers--;
				}
			}
			else if (lz >= 0){//camera looks left
	
				if (z1 - 1 >= 0){
					if (maze[Posy + y1][x1][z1 - 1] == "E") {
						if (z1 - 2 >= 0 && maze[Posy + y1][x1][z1 - 2] != "E"){
							maze[Posy + 1][x1][z1 - 2] = "E";
							hammers--;
						}
					}
					else {
						maze[Posy + y1][x1][z1 - 1] = "E";
						hammers--;
					}
				}

			}
			else if (lz <= 0){//camera looks right
		
				if (z1 + 1 < dimension){
					if (maze[Posy + y1][x1][z1 + 1] == "E") {
						if (z1 + 2 < dimension && maze[Posy + y1][x1][z1 + 2] != "E"){
							maze[Posy + y1][x1][z1 + 2] = "E";
							hammers--;
						}
					}
					else {
						maze[Posy + y1][x1][z1 + 1] = "E";
						hammers--;
					}
				}

			}
		}
		glutPostRedisplay();
	}
	else if (key == 32 && viewState == 1){ //For Space Key


		int x1 = calculatePlayerPositionx(eyex);
		int y1 = calculatePlayerPositionz(eyez);
		if (Posy < levels - 1){

			if (maze[Posy + 1][x1][y1] == "E"){
				eyey += 5;
				Posy++;

			}

		}
		else {
			calculateScore();
			viewState = 2; //Game ends
		}


	}
	else if ((key == 'E' || key == 'e') && Posy == (levels - 1)){  // Game ends also when player is at last level and presses "E"
		calculateScore();
		viewState = 2;
	}
	else if ((key == 'x' || key == 'X' )&& viewState==1){// When user press x the game ends and the score becoms 0
		viewState = 2; // The game has ended so the user can't press or click
		score = 0;
	}
	
	else if ((key == 'A' || key == 'a' )&& viewState==0){// When user press x the game ends and the score becoms 0
		transparency=1-transparency;
		
	}
	
}





void mouseButton(int button, int state, int x, int y){
	if (viewState != 2){// The game has ended so the user can't press or click
		if (button == GLUT_LEFT_BUTTON) {

			// when the button is released
			if (state == GLUT_UP) {
				anglex += deltaAnglex;
				xOrigin = -1;
				yOrigin = -1;
				angley += deltaAngley;
			}
			else  {// state = GLUT_DOWN
				xOrigin = x;
				yOrigin = y;
			}
		}
	}

}


void mouseMove(int x, int y) {

	// this will only be true when the left button is down
	// this will only be true when the left button is down
	if (viewState == 1){  // The game has ended so the user can't press or click
		if (xOrigin >= 0) {

			// update deltaAngle
			deltaAnglex = (x - xOrigin) * 0.001f;
			// update camera's direction
			lx = sin(anglex + deltaAnglex);
			lz = -cos(anglex + deltaAnglex);

		}
		if (yOrigin >= 0){
			deltaAngley = (y - yOrigin) * 0.001f;
			ly = -sin(angley + deltaAngley);
		}
	}
}




int main(int argc, char **argv) {

	//init Glut and create window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glEnable( GL_CULL_FACE );
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(840, 624);
	glutCreateWindow("Maze");
	
	//register callbacks
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutIdleFunc(renderScene);

	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMove);

	//Keyboard functions
	glutKeyboardFunc(processNormalKeys);
	readFile();
	init();
	LoadGLTextures();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_DEPTH_BUFFER_BIT);
	//enter Glut event processing cycle
	glutMainLoop();



	return 1;
}



