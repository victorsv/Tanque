#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>
#include <vector>
#include <ctime>
#include "png_texture.h"
using namespace std;

#define PI 3.1415
#define plano_size 10.0
#define tplano "grass.png"
#define tpredio "building.png"
#define ttank "plate.png"
#define tfront "front.png"
#define tback "back.png"
#define tleft "left.png"
#define tright "right.png"
#define ttop "top.png"
#define tbottom "bottom.png"

GLuint  iplano;
GLuint  ipredio;
GLuint  itanque;
GLuint  ifront;
GLuint  iback;
GLuint 	ileft;
GLuint  iright;
GLuint  itop;
GLuint  ibottom;

bool keys[4];

class Camera{
	private:
		GLfloat x;
		GLfloat y;
		GLfloat z;
		GLfloat yaw;

	public:
		void move(GLfloat x, GLfloat z, GLfloat yaw){
			this->x = x;
			this->z = z;
			//this->yaw = yaw;
		}
		void setX (GLfloat xx){ 
			x = xx;
		}
		void setY (GLfloat yy){
			y = yy;
		}
		void setZ (GLfloat zz){
			z = zz;
		}

		GLfloat getX(){
			return x;
		}
		GLfloat getY(){
			return y;
		}
		GLfloat getZ(){
			return z;
		}

		void moveTo (int xx, int yy, int zz){
			x = xx;
			y = yy;
			z = zz;
		}

		Camera(int xx, int yy, int zz){
			x = xx;
			y = yy;
			z = zz;
		}
};

class realObj{
	protected:
		GLfloat x, y, z;
		GLfloat angl;
		int id;

	public:
		void setX(GLfloat x){
			this->x = x;
		}
		void setY(GLfloat y){
			this->y = y;
		}
		void setZ(GLfloat z){
			this->z = z;
		}
		void setAngle(GLfloat a){
			angl = a;
		}
		GLfloat getX(){
			return x;
		}

		GLfloat getY(){
			return y;
		}

		GLfloat getZ(){
			return z;
		}

		GLfloat getAngle(){
			return angl;
		}

		GLfloat getID(){
			return id;
		}
};

class Tank: public realObj{
	protected:
		GLfloat yaw;
		GLfloat speed;
		GLfloat cannonAngle;
	public:
		void setCAngle(GLfloat a){
			cannonAngle = a;
		}
		GLfloat getCAngle(){
			return cannonAngle;
		}
		void setyaw(GLfloat a){
			yaw = a;
		}
		GLfloat getyaw(){
			return yaw;
		}
		GLfloat rotate(GLfloat a){
			yaw += a;
		}
		void moveUp(){
			x = fmax(fmin(x-speed*sin(yaw), 98.0f),-98.0f);
			z = fmax(fmin(z-speed*cos(yaw), 98.0f),-98.0f);
		}

		void moveBack(){
			x = fmax(fmin(x+speed*sin(yaw), 98.0f),-98.0f);
			z = fmax(fmin(z+speed*cos(yaw), 98.0f),-98.0f);
		}
		Tank(GLfloat x, GLfloat y, GLfloat z){
			this->x = x;
			this->y = y;
			this->z = z;
			speed = 0.2f;
			cannonAngle = 200;
		}
};

class Obstacle: public realObj{
	protected:
		bool destroyed;
		int height, width, length;
	public:
		void destroy(){
			destroyed = true;
		}
		void setDim(GLfloat h, GLfloat w, GLfloat l){
			height = h;
			width = w;
			length = l;
		}

		Obstacle(GLfloat x, GLfloat y, GLfloat z){
			this->x = x;
			this->y = y;
			this->z = z;
			destroyed = false;
		}
};

class Bullet: public realObj{
	private:
		GLfloat xspeed;
		GLfloat zspeed;
		GLfloat yspeed;
		bool hit;
	public:
		void setAngle(GLfloat a){
			angl = a;
		}
		void setSpeed(GLfloat speed, GLfloat bAngle){
			xspeed = ((speed*(cos(bAngle)))*sin(angl))*-1;
			zspeed = ((speed*(cos(bAngle)))*cos(angl))*-1;
			yspeed = speed*fabs(sin(bAngle));
			hit = false;
		}
		bool isHit(){
			return hit;
		}
		void update(){
			x += xspeed;
			y += yspeed;
			z += zspeed;
			if (yspeed >- 9.2)
				yspeed -= 0.02f;
			if (y < 0){
				y = 0;
				xspeed = xspeed*0.7f;
				zspeed = zspeed*0.7f;
				yspeed = (yspeed*-1)*0.7f;
			}

			if (fabs(yspeed) < 0.2f && fabs(zspeed) < 0.2f && fabs(xspeed) < 0.2f)
				hit = true;
		}
};

vector<Obstacle> predio;
vector<Bullet> tiros;
Tank player(0.0f,0.0f,0.0f);
Camera cam(0,0,0);

GLfloat coord_plano[4][2]={
	{-plano_size,-plano_size},
	{+plano_size,-plano_size},
	{+plano_size,+plano_size},
	{-plano_size,+plano_size}
};

GLfloat coord_predio[4][2]={
	{0,0},
	{1,0},
	{1,4},
	{0,4}
};

GLfloat coord_geral[4][2]={
	{0,0},
	{1,0},
	{1,1},
	{0,1}
};

GLfloat yLocation = 0.0f; // Keep track of our position on the y axis.
GLfloat yRotationAngle = 0.0f; // The angle of rotation for our object
bool movingUp = false; // Whether or not we are moving up or down

GLint WIDTH =800;
GLint HEIGHT=600;

void carregar_texturas(void){
	iplano = png_texture_load(tplano, NULL, NULL);
	ipredio = png_texture_load(tpredio, NULL, NULL);
	itanque = png_texture_load(ttank, NULL, NULL);
	ifront = png_texture_load(tfront, NULL, NULL);
	iback = png_texture_load(tback, NULL, NULL);
	ileft = png_texture_load(tleft, NULL, NULL);
	iright = png_texture_load(tright, NULL, NULL);
	itop = png_texture_load(ttop, NULL, NULL);
	ibottom = png_texture_load(tbottom, NULL, NULL);
}

void init(){
	int i;
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	carregar_texturas();
	glEnable(GL_TEXTURE_2D);
	//Insere uma quantidade aleatória de prédios na lista
	srand (time(NULL));
	for (i = 0; i < 50; i++){
		Obstacle obs(0, 0, 0);
		obs.setX((rand() % 150)-75);
		obs.setZ((rand() % 150)-75);
		predio.push_back(obs);
	}
}

void renderBuldings(){
	int i;
	for(i = 0; i < predio.size(); i++){
		glPushMatrix();
		glBindTexture(GL_TEXTURE_2D,ipredio);
		glTranslatef(predio.at(i).getX(), 0, predio.at(i).getZ());
  		glBegin(GL_QUADS);

		// Topo
		glColor3f(1.0f, 1.0f, 1.0f);
		glTexCoord2fv(coord_predio[0]);
		glVertex3f( 2.0f, 12.0f, -2.0f);
		glTexCoord2fv(coord_predio[1]);
		glVertex3f(-2.0f, 12.0f, -2.0f);
		glTexCoord2fv(coord_predio[2]);
		glVertex3f(-2.0f, 12.0f,  2.0f);
		glTexCoord2fv(coord_predio[3]);
		glVertex3f( 2.0f, 12.0f,  2.0f);
 
		// Fundo
		glTexCoord2fv(coord_predio[0]);
		glVertex3f( 2.0f, -2.0f,  2.0f);
		glTexCoord2fv(coord_predio[1]);
		glVertex3f(-2.0f, -2.0f,  2.0f);
		glTexCoord2fv(coord_predio[2]);
		glVertex3f(-2.0f, -2.0f, -2.0f);
		glTexCoord2fv(coord_predio[3]);
		glVertex3f( 2.0f, -2.0f, -2.0f);
		
		// Frente
		glTexCoord2fv(coord_predio[0]);
		glVertex3f( 2.0f,  12.0f, 2.0f);
		glTexCoord2fv(coord_predio[1]);
		glVertex3f(-2.0f,  12.0f, 2.0f);
		glTexCoord2fv(coord_predio[2]);
	 	glVertex3f(-2.0f, -2.0f, 2.0f);
	 	glTexCoord2fv(coord_predio[3]);
		glVertex3f( 2.0f, -2.0f, 2.0f);
 
		//Verso
		glTexCoord2fv(coord_predio[0]);
		glVertex3f(-2.0f,  12.0f, -2.0f);
		glTexCoord2fv(coord_predio[1]);
		glVertex3f( 2.0f,  12.0f, -2.0f);
		glTexCoord2fv(coord_predio[2]);
		glVertex3f( 2.0f, -2.0f, -2.0f);
		glTexCoord2fv(coord_predio[3]);
		glVertex3f(-2.0f, -2.0f, -2.0f);
 
		// Esquerda
		glTexCoord2fv(coord_predio[0]);
		glVertex3f(-2.0f,  12.0f,  2.0f);
		glTexCoord2fv(coord_predio[1]);
		glVertex3f(-2.0f,  12.0f, -2.0f);
		glTexCoord2fv(coord_predio[2]);
		glVertex3f(-2.0f, -2.0f, -2.0f);
		glTexCoord2fv(coord_predio[3]);
		glVertex3f(-2.0f, -2.0f,  2.0f);
 
		// Direita
		glTexCoord2fv(coord_predio[0]);
		glVertex3f(2.0f,  12.0f, -2.0f);
		glTexCoord2fv(coord_predio[1]);
		glVertex3f(2.0f,  12.0f,  2.0f);
		glTexCoord2fv(coord_predio[2]);
		glVertex3f(2.0f, -2.0f,  2.0f);
		glTexCoord2fv(coord_predio[3]);
		glVertex3f(2.0f, -2.0f, -2.0f);
  		glEnd();
		glPopMatrix();
	}
}

void renderSkybox(){
	glPushMatrix();
	glTranslatef(player.getX(), 50, player.getZ());
	glBindTexture(GL_TEXTURE_2D,itop);
 	glBegin(GL_QUADS);
	// Topo
	glColor3f(1.0f, 1.0f, 1.0f);
	glTexCoord2fv(coord_geral[3]); glVertex3f(-200.0f, 100.0f,  200.0f);
	glTexCoord2fv(coord_geral[2]); glVertex3f( 200.0f, 100.0f,  200.0f);
	glTexCoord2fv(coord_geral[1]); glVertex3f( 200.0f, 100.0f, -200.0f);
	glTexCoord2fv(coord_geral[0]); glVertex3f(-200.0f, 100.0f, -200.0f);
	glEnd();

	glBindTexture(GL_TEXTURE_2D,ibottom);
	glBegin(GL_QUADS);
	// Fundo
	glTexCoord2fv(coord_geral[3]); glVertex3f(-200.0f, -200.0f, -200.0f);
	glTexCoord2fv(coord_geral[2]); glVertex3f( 200.0f, -200.0f, -200.0f);
	glTexCoord2fv(coord_geral[1]); glVertex3f( 200.0f, -200.0f,  200.0f);
	glTexCoord2fv(coord_geral[0]); glVertex3f(-200.0f, -200.0f,  200.0f);
	glEnd();

	glBindTexture(GL_TEXTURE_2D,ifront); 
	glBegin(GL_QUADS);
	// Frente
	glTexCoord2fv(coord_geral[3]); glVertex3f( 200.0f,  100.0f, 200.0f);
	glTexCoord2fv(coord_geral[2]); glVertex3f(-200.0f,  100.0f, 200.0f);
	glTexCoord2fv(coord_geral[1]); glVertex3f(-200.0f, -200.0f, 200.0f);
 	glTexCoord2fv(coord_geral[0]); glVertex3f( 200.0f, -200.0f, 200.0f);
	glEnd();

	glBindTexture(GL_TEXTURE_2D,iback);
	glBegin(GL_QUADS);
	// Verso
	glTexCoord2fv(coord_geral[0]); glVertex3f(-200.0f, -200.0f, -200.0f);
	glTexCoord2fv(coord_geral[3]); glVertex3f(-200.0f,  100.0f, -200.0f);
	glTexCoord2fv(coord_geral[2]); glVertex3f( 200.0f,  100.0f, -200.0f);
	glTexCoord2fv(coord_geral[1]); glVertex3f( 200.0f, -200.0f, -200.0f);
	glEnd();

	glBindTexture(GL_TEXTURE_2D,ileft); 
	glBegin(GL_QUADS);
	// Esquerda
	glTexCoord2fv(coord_geral[3]); glVertex3f(-200.0f,  100.0f,  200.0f);
	glTexCoord2fv(coord_geral[2]); glVertex3f(-200.0f,  100.0f, -200.0f);
	glTexCoord2fv(coord_geral[1]); glVertex3f(-200.0f, -200.0f, -200.0f);
	glTexCoord2fv(coord_geral[0]); glVertex3f(-200.0f, -200.0f,  200.0f);
	glEnd();


	glBindTexture(GL_TEXTURE_2D,iright); 
	glBegin(GL_QUADS);
	// Direita
	glTexCoord2fv(coord_geral[3]); glVertex3f(200.0f,  100.0f, -200.0f);
	glTexCoord2fv(coord_geral[2]); glVertex3f(200.0f,  100.0f,  200.0f);
	glTexCoord2fv(coord_geral[1]); glVertex3f(200.0f, -200.0f,  200.0f);
	glTexCoord2fv(coord_geral[0]); glVertex3f(200.0f, -200.0f, -200.0f);
	glEnd();
	glPopMatrix();
}

void renderFloor(){
	glPushMatrix();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL );
	glBindTexture(GL_TEXTURE_2D, iplano);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBegin(GL_QUADS);
	glTexCoord2fv(coord_plano[0]);  glVertex3f(-100,0.0f,100);
	glTexCoord2fv(coord_plano[1]);  glVertex3f(100,0.0f,100);
	glTexCoord2fv(coord_plano[2]);  glVertex3f(100,0.0f,-100);
	glTexCoord2fv(coord_plano[3]);  glVertex3f(-100,0.0f,-100);
	glEnd();

	glPopMatrix();
}

void renderPlayer(){
	GLUquadricObj *quadric;

	//Corpo
	glColor3f(0.3f, 0.5f, 0.0f);
	quadric = gluNewQuadric();
	gluQuadricTexture(quadric, GL_TRUE);
	glPushMatrix();
	glTranslatef(0.0f, -3.5f, -6.0f);
	glBindTexture(GL_TEXTURE_2D, itanque);
	gluSphere(quadric, 2,20,20);
	glPopMatrix();
	//Canhão
	quadric = gluNewQuadric();
	gluQuadricTexture(quadric, GL_TRUE);
	glPushMatrix();
	glTranslatef(0,-2.0f,-6.0f);
	glRotatef(player.getCAngle(),1,0,0);
	gluCylinder(quadric, 0.5, 0.3, 3.5, 12, 3);
	glPopMatrix();
}

void renderBullet(){
	GLUquadricObj *quadric;
	for(int i = 0; i < tiros.size(); i++){
		glColor3f(0.4f, 0.4f, 0.4f);
		quadric = gluNewQuadric();
		gluQuadricTexture(quadric, GL_TRUE);
		glPushMatrix();
		glTranslatef(tiros.at(i).getX(), tiros.at(i).getY(), tiros.at(i).getZ());
		glBindTexture(GL_TEXTURE_2D, itanque);
		gluSphere(quadric, 0.5f,20,20);
		glPopMatrix();
	}
}

GLfloat Distance(GLfloat x, GLfloat y, GLfloat xx, GLfloat yy){
	return sqrt(pow((y-yy), 2)+pow((x-xx), 2));
}

void updatePositions(){
	cam.setX(player.getX()+6*sin(player.getyaw()));
	cam.setZ(player.getZ()+6*cos(player.getyaw()));

	//Implementação feia de colisões
	//Verifica a distancia entre o tiro e o predio
	//As classes não tem deconstrutores
	//A remoção é feita ao mesmo tempo que ocorre a colisão
	//MELHORAR: Deletar apenas DEPOIS de todas as checagens ocorrerem
	if (tiros.size() > 0){
		for(int j = 0; j < tiros.size(); j++){
			tiros.at(j).update();
			//Checar colisões
			if(tiros.at(j).isHit()){
				tiros.erase(tiros.begin() + j);
				break;
			}
			for(int i = 0; i < predio.size(); i++){
				if (Distance(tiros.at(j).getX(), tiros.at(j).getZ(), predio.at(i).getX(), predio.at(i).getZ())< 2.5f){
					cout << "Hit! " << predio.at(i).getX() << " " << predio.at(i).getZ() << "\n";
					predio.erase(predio.begin() + i);
					tiros.erase(tiros.begin() + j);
					break;
				}
			}
		}
	}
	if (keys[0] == true){
		player.moveUp();
	}

	if (keys[1] == true){
		//player.moveBack();
	}

	if (keys[2] == true && keys[3] == false){
		player.rotate(0.05f);
	}

	if (keys[3] == true && keys[2] == false){
		player.rotate(-0.05f);
	}
}

void display(void){

	int i = 0;
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0,1.0,1.0,1.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glShadeModel(GL_SMOOTH);

	glPushMatrix();
	updatePositions();
	gluLookAt(
		cam.getX(),
		3.0,
		cam.getZ(),

		player.getX(),
		3.0,
		player.getZ(),

		0.0,
		1.0,
		0.0);

	renderSkybox();
	renderFloor();
	renderBuldings();
	renderBullet();
	glPopMatrix();
	renderPlayer();
	glutSwapBuffers();
}

void reshape(int width, int height){
	WIDTH = width;
	HEIGHT = height;
	glViewport(0, 0,(GLint)width, (GLint)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70.0, width/(GLfloat)height, 0.1, 10000.0);
	glMatrixMode(GL_MODELVIEW);
}

void playerShoot(){
	Bullet pshot;
	pshot.setX(player.getX());
	pshot.setY(player.getY());
	pshot.setZ(player.getZ());
	pshot.setAngle(player.getyaw());
	pshot.setSpeed(0.9f,33*(PI/180));
	tiros.push_back(pshot);
}

void special(int key, int x, int y){
	switch (key) {
	case GLUT_KEY_UP:
		keys[0] = true;
		glutPostRedisplay();
		break;
	case GLUT_KEY_DOWN:
		keys[1] = true;	
		playerShoot();
		glutPostRedisplay();
		break;
	case GLUT_KEY_LEFT:
		keys[2] = true;
		glutPostRedisplay();
		break;
	case GLUT_KEY_RIGHT:
		keys[3] = true;
		glutPostRedisplay();
		break;
	}
}

void specialrelease(int key, int x, int y){
	switch (key) {
	case GLUT_KEY_UP:
		keys[0] = false;
		glutPostRedisplay();
		break;
	case GLUT_KEY_DOWN:
		keys[1] = false;
		glutPostRedisplay();
		break;
	case GLUT_KEY_LEFT:
		keys[2] = false;
		glutPostRedisplay();
		break;
	case GLUT_KEY_RIGHT:
		keys[3] = false;
		glutPostRedisplay();
		break;
	}
}

void keyboard(unsigned char key, int x, int y){
	switch (key) {
	case 27:
		exit(0);
		break;
	}
}

int main(int argc,char **argv){
	glutInitWindowPosition(0,0);
	glutInitWindowSize(WIDTH,HEIGHT);
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGBA|GLUT_DEPTH|GLUT_DOUBLE|GLUT_ALPHA/*|GLUT_MULTISAMPLE*/);

	if(!glutCreateWindow("Tanque")) {
		fprintf(stderr,"Erro.\n");
		exit(-1);
	}

	init();

	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutSpecialUpFunc(specialrelease);
	glutDisplayFunc(display);
	glutIdleFunc(display);
	glutReshapeFunc(reshape);
	glutMainLoop();

	return(0);
}
