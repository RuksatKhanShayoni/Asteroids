/*
    * CSC 4118 : Computer graphics course final project
    * Section : I , Semester : Spring(21-22)
    * Made by
    * Group Members:
    * ANINDRA DAS BIVAS     20-41848-1
    * S M ASIF HOSSAIN      21-44421-1
    * RUKSAT KHAN SHAYONI   20-41922-1



    *--------------------------
    * References
    *--------------------------

    * https://github.com/RyGuyM/Asteroids/blob/master/Asteroids.c

    * Code-It-Yourself! Asteroids (Simple C++ and Maths!)
        -https://youtu.be/QgDR8LrRZhk

    * https://gamedev.net/forums/topic/439127-asteroids-opengl/439127/

    * https://www.khronos.org/opengl/


*/

#include<windows.h>
#include<math.h>
#include<vector>
#include<GL/glut.h>
#include<iostream>
#include<time.h>
#include<algorithm>
#include<stdlib.h>
#include<cstdlib>
#include<stdio.h>
#include<string.h>
#include<mmsystem.h>
#define EB_TIMER 10
#define ES_SPAWNTIMER 100
#define SM_SPAWNTIMER 300
#define S_HITCOUNT 3
#define SPACE 32

using namespace std;

/* timer methods */
void startMenuTimer(int);
void crashShipScene(int);
void gameTimer(int);
void gameOverTimer(int);
void instructionTimer(int);

/*
    Callback methods
*/
void menu();
void gameOverMenu();
void instructionMenu();
void stageSelector();

/* Helper Methods */
void displayText(char* text,void* font,double xpos,double ypos,
                 GLbyte r,GLbyte g,GLbyte b);
double generateRandD(double fmin,double fmax);
double toRadians(double angle);
double toDegrees(double rad);
void translate2D(double x,double y);
void rotate2D(double angle);
void scale2d(double xscale,double yscale);
void menuBar();
char* getScore(int scoreVal);


//spaceship object
struct spaceShip{
    bool isDead = false,thrustOn = false;
    double vx,vy,x,y,angle;
    vector<double> xcoords,ycoords;
};

//asteroids object
struct asteroid{
    int nVertices;
    bool isDestroyed = false;
    double vx,vy,x,y,aSize;
    vector<double> xcoords,ycoords;
};

//ship bullet object
struct bullet{
    bool isDestroyed = false;
    double bx,by,x,y;
};

//enemy ship bullet object
struct enemyBullet{
    bool isDestroyed = false;
    double ebx,eby,x,y;
    vector<double> xcoords,ycoords;
};


//enemy ship object
struct enemyShip{
    bool isDestroyed = false;
    double x,y,dx,dy,angle;
    vector<double> xcoords,ycoords;
};

//background stars
struct star{
    double x,y;
};


//moon stage circles
struct moonCircle{
    float moonx,moony,radius;
    int triangleAmount = 40;
    float pi = 2.0f * 3.1416f;
};

//space mine object
struct spaceMine{
    double x,y,dx,dy;
    bool isDestroyed = false;
    vector<double> xcoords,ycoords;

};



//global variables
spaceShip s;
vector<bullet> vecBullet;
vector<enemyBullet> vecEnemyBullet;
vector<asteroid> asteroids;
vector<moonCircle> circles;
vector<star> stars;
vector<enemyShip> eships;
vector<spaceMine> spaceMines;

//global variables
//initial configuration
const int w = 640, h = 480;
double maxW,maxH;
int score = 0,lives = 3;
bool gameOver = false,gameStart = false,stageSelect = false,ebHit = false,
exitGame = false,vIns = false;
int upKey=0,downKey = 0,leftKey = 0,rightKey = 0,crashTimer = 0,
stage = 1,pauseTimer = 0,highscore = 0,stageTimer = 0,
bulletTimer  = EB_TIMER,sHitCount = S_HITCOUNT,
esSpawnTimer = ES_SPAWNTIMER,gameOverOpt = 1,startMenuOpt = 3,
smSpawnTimer = SM_SPAWNTIMER;
float bgR,bgG,bgB;
void init(int,int);

/*
    Space objects initializations
*/
void initSpaceShip();
void initAsteroids(double xval,double yval,int numberOfAsteroids,double aSize);
void createRandomAsteroids(int noOfAsteroids);
void generateMoonCircles();
void generateStars();
void spawnEnemyShip();
void spawnExampleEnemyShip();
void spawnExampleSpaceMine();
void spawnSpaceMine();
void initEnemyBullets();

/*
    Collision Detection methods
*/
int detectCollisionAB(bullet* b,asteroid* a);
int detectCollisionAS(double xcoords,double ycoords,asteroid* a);
int detectCollisionSE(double xcoords,double ycoords,enemyShip* es);
int detectCollisionSEB(double xcoords,double ycoords,enemyBullet* eb);
int detectCollisionSSM(double xcoords,double ycoords,spaceMine* sm);

/* apply ship thrust method */
void accelerateShip();


/* Object removal checking methods */
bool removeSpaceMine(spaceMine sm);
bool removeEnemySpaceShip(enemyShip es);
bool removeBullets(bullet b);
bool removeAsteroid(asteroid a);
bool removeEnemyBullet(enemyBullet eb);

/* Glut shapes drawing methods */
void drawAsteroids(asteroid* a);
void drawMoonCircle(moonCircle* c);
void drawStars(star* s);
void drawSpaceMine(spaceMine* sm);
void drawMoonCircle(moonCircle* c);
void drawBullet(bullet* b);
void drawSpaceShipLives(int p);
void drawCurrSpaceShipLives(int p);
void drawSpaceShip(spaceShip* s);
void drawEnemySpaceShip(enemyShip* es);
void drawSpaceShipHit(spaceShip *s);

/* game start initializations */
void initGame();

/*keyboard key press methods */
void keyInput(int key,int x,int y);
void gameKeys(unsigned char key,int x,int y);


/* generates random double between two double */
double generateRandD(double fmin,double fmax){
    double f;
    f = fmin+(fmax-fmin)*(rand()%0x7fff)/32767.0;
    return f;
}

void displayText(char* text,void* font,double xpos,double ypos,GLbyte r,GLbyte g,GLbyte b){
    glColor3ub(r,g,b);
    glRasterPos2f(xpos,ypos);
    for(int i=0;i<strlen(text);i++){
        glutBitmapCharacter(font,text[i]);
    }
}

/* clears initial screen */
void initBg(){
     glClearColor(0.0f,0.0f,0.0f,1.0f);
}

double toRadians(double angle){
    return 3.1416/180 * angle;
}

double toDegrees(double rad){
    return 180/3.1416 * rad;
}

void translate2D(double x,double y){
    glTranslated(x,y,0.0);
}

void rotate2D(double angle){
    glRotated(toDegrees(angle),0.0,0.0,1.0);
}

void scale2d(double xscale,double yscale){
    glScaled(xscale,yscale,0.0);
}

void menuBar(){
glBegin(GL_QUADS);
        glVertex2d(15.0,5.0);
        glVertex2d(2.0,5.0);
        glVertex2d(2.0,2.0);
        glVertex2d(15.0,2.0);
glEnd();
}


char* getScore(int scoreVal){
    int length = snprintf(NULL,0,"%d",scoreVal);
    char* str = (char*) malloc(length+1);
    snprintf(str,length+1,"%d",scoreVal);
    free(str);
    return str;

}

//initialize spaceship coordinates
void initSpaceShip(){
    s.angle = 0.0;
    s.vx = 0.0;
    s.vy = 0.0;
    s.x = maxW/2.0;
    s.y = maxH/2.0;
    int scaleX =4;
    int scaleY = 5.5;
    s.xcoords.push_back(cos(toRadians(90))*scaleX);
    s.ycoords.push_back(sin(toRadians(90))*scaleY);
    s.xcoords.push_back(cos(toRadians(225))*scaleX);
    s.ycoords.push_back(sin(toRadians(225))*scaleY);
    s.xcoords.push_back(cos(toRadians(315))*scaleX);
    s.ycoords.push_back(sin(toRadians(315))*scaleY);

}




//initialize asteroid coordinates
void initAsteroids(double xval,double yval,int numberOfAsteroids,double aSize){

    double theta,r,x,y;

    for(int i=0;i<numberOfAsteroids;i++){
        asteroid a;
        a.aSize = aSize;
        a.isDestroyed = false;
        a.x = xval;
        a.y = yval;
        a.vx = generateRandD(-0.8,0.8);
        a.vy = generateRandD(-0.8,0.8);
        a.nVertices = 6 + rand() % 5;
        for(int i=0;i<a.nVertices;i++){
            theta = 2.0*3.1416*i/a.nVertices;
            r = a.aSize * generateRandD(2.0,3.0);
            x = - r * sin(theta);
            y = r * cos(theta);
            a.xcoords.push_back(x);
            a.ycoords.push_back(y);
        }
        asteroids.push_back(a);
    }

}


//generate a random set of asteroids
void createRandomAsteroids(int noOfAsteroids){
    for(int i=0;i<noOfAsteroids;i++){
            initAsteroids(generateRandD(0.0,maxW),generateRandD(0.0,maxH),1,3.0);
    }
}


//generate a random set of moon circles for background
void generateMoonCircles(){
    if(circles.size() > 0){
        circles.clear();
    }
    for(int i=0;i<20;i++){
        moonCircle c;
        c.moonx = (float) generateRandD(-100.0,maxW);
        c.moony = (float) generateRandD(-100.0,maxH);
        c.radius = (float) generateRandD(5.0,10.0);
        circles.push_back(c);
    }

}

//generate random set of stars with x,y coordinates
//for background
void generateStars(){
    if(stars.size() > 0){
        stars.clear();
    }
    for(int i =0;i<100;i++){
        star s;
        s.x = generateRandD(0.0,maxW);
        s.y = generateRandD(0,maxH);
        stars.push_back(s);
    }

}

/* create a space ship object
and insert it onto a list */
void spawnEnemyShip(){

    if(eships.size() > 0){
        eships.clear();
    }

    enemyShip es;
     es.angle = 90;

     es.x = 160;

     es.y = generateRandD(10.0,90.0);

     es.dx = - 1.5 * sin(toRadians(es.angle));
     es.dy = 1.5 * cos(toRadians(es.angle));

    double scaleX = 4;
    double scaleY = 5.5;
    es.xcoords.push_back(cos(toRadians(90))*scaleX);
    es.ycoords.push_back(sin(toRadians(90))*scaleY);
    es.xcoords.push_back(cos(toRadians(225))*scaleX);
    es.ycoords.push_back(sin(toRadians(225))*scaleY);
    es.xcoords.push_back(cos(toRadians(315))*scaleX);
    es.ycoords.push_back(sin(toRadians(315))*scaleY);
    eships.push_back(es);

}

/* this enemy ship created for instruction */
void spawnExampleEnemyShip(){
    if(eships.size() > 0){
        eships.clear();
    }
    enemyShip es;

    es.x = (maxW/2.0)-20.0;

    es.y = maxH - 50.0;

    double scaleX = 4;
    double scaleY = 5.5;
    es.xcoords.push_back(cos(toRadians(90))*scaleX);
    es.ycoords.push_back(sin(toRadians(90))*scaleY);
    es.xcoords.push_back(cos(toRadians(225))*scaleX);
    es.ycoords.push_back(sin(toRadians(225))*scaleY);
    es.xcoords.push_back(cos(toRadians(315))*scaleX);
    es.ycoords.push_back(sin(toRadians(315))*scaleY);
    eships.push_back(es);

}


void spawnExampleSpaceMine(){
    if(spaceMines.size() > 1){
        spaceMines.clear();
    }
    spaceMine sm;
    sm.x = (maxW/2.0)-20.0;

    sm.y = maxH - 70.0;
    double scaleX = 4;
    double scaleY = 5.5;
    sm.xcoords.push_back(cos(toRadians(90))*scaleX);
    sm.ycoords.push_back(sin(toRadians(10))*scaleY);
    sm.xcoords.push_back(cos(toRadians(225))*scaleX);
    sm.ycoords.push_back(sin(toRadians(225))*scaleY);
    sm.xcoords.push_back(cos(toRadians(315))*scaleX);
    sm.ycoords.push_back(sin(toRadians(315))*scaleY);
    sm.xcoords.push_back(cos(toRadians(90))*scaleX);
    sm.ycoords.push_back(-sin(toRadians(10))*scaleY);
    sm.xcoords.push_back(cos(toRadians(315))*scaleX);
    sm.ycoords.push_back(-sin(toRadians(315))*scaleY);
    sm.xcoords.push_back(cos(toRadians(225))*scaleX);
    sm.ycoords.push_back(-sin(toRadians(225))*scaleY);

    spaceMines.push_back(sm);
}
/* create a space mine object
and insert it into list */
void spawnSpaceMine(){
    if(spaceMines.size() > 1){
        spaceMines.clear();
    }
    spaceMine sm;
    if(generateRandD(-1,1)< 0){
        sm.x = -0.1;
        sm.y = 100;
        sm.dx = -1.0 * sin(toRadians(generateRandD(200,300)));
        sm.dy = 1.0* cos(toRadians(220));
    }
    else{
        sm.x = 120;
        sm.y = 100;
        sm.dx = -1.0 * sin(toRadians(generateRandD(100,200)));
        sm.dy = 1.0* cos(toRadians(220));
    }
    double scaleX = 4;
    double scaleY = 5.5;
    sm.xcoords.push_back(cos(toRadians(90))*scaleX);
    sm.ycoords.push_back(sin(toRadians(10))*scaleY);
    sm.xcoords.push_back(cos(toRadians(225))*scaleX);
    sm.ycoords.push_back(sin(toRadians(225))*scaleY);
    sm.xcoords.push_back(cos(toRadians(315))*scaleX);
    sm.ycoords.push_back(sin(toRadians(315))*scaleY);
    sm.xcoords.push_back(cos(toRadians(90))*scaleX);
    sm.ycoords.push_back(-sin(toRadians(10))*scaleY);
    sm.xcoords.push_back(cos(toRadians(315))*scaleX);
    sm.ycoords.push_back(-sin(toRadians(315))*scaleY);
    sm.xcoords.push_back(cos(toRadians(225))*scaleX);
    sm.ycoords.push_back(-sin(toRadians(225))*scaleY);

    spaceMines.push_back(sm);


}


void initEnemyBullets(){

    double sB = 0.5;
    for(int i=0;i<eships.size();i++){
        enemyBullet eb;
        eb.isDestroyed = false;
        if(eships[i].x >= maxW){
            eb.x = eships[i].x-20;
        }
        else{
            eb.x = eships[i].x;
        }
        eb.y = eships[i].y;
        eb.xcoords.push_back(-sB);
        eb.ycoords.push_back(-sB);
        eb.xcoords.push_back(sB);
        eb.ycoords.push_back(-sB);
        eb.xcoords.push_back(sB);
        eb.ycoords.push_back(sB);
        eb.xcoords.push_back(-sB);
        eb.ycoords.push_back(sB);
        eb.ebx = -5.0 * sin(toRadians(eships[i].angle));
        eb.eby = 5.0 * cos(toRadians(eships[i].angle));
        vecEnemyBullet.push_back(eb);
        break;
    }
}


int detectCollisionAB(bullet* b,asteroid* a){
    double x1,y1,x2,y2,bx,by,xi;
    int intersect_no = 0.0;
    int lines = a->nVertices;
    bx = b->x;
    by = b->y;
    for(int j=0;j<a->nVertices;j++){
            x1 = a->x + a->xcoords[j%(lines)];
            y1 = a->y + a->ycoords[j%(lines)];
            x2 = a->x + a->xcoords[(j+1)%(lines)];
            y2 = a->y + a->ycoords[(j+1)%(lines)];
            if((by <= y1 && by >= y2) || (by >= y1 && by <= y2)){
                xi = (((by-y1)/(y2-y1))*x2) + (((y2-by)/(y2-y1))*x1);
                if((xi>=bx) &&(((xi<=x1)&&(xi>=x2))||((xi>=x1)&&(xi<=x2)))){
                        intersect_no++;
                }
            }
    }
    return intersect_no % 2;
}


int detectCollisionAS(double xcoords,double ycoords,asteroid* a){
    int intersect_no = 0.0;
    int lines = a->nVertices;
    double x1,y1,x2,y2,sx,sy,xi;
    sx = xcoords + s.x;
    sy = ycoords + s.y;
    for(int j=0;j<a->nVertices;j++){
            x1 = a->x + a->xcoords[j%(lines)];
            y1 = a->y + a->ycoords[j%(lines)];
            x2 = a->x + a->xcoords[(j+1)%(lines)];
            y2 = a->y + a->ycoords[(j+1)%(lines)];
            if((sy <= y1 && sy >= y2) || (sy >= y1 && sy <= y2)){
                xi = (((sy-y1)/(y2-y1))*x2) + (((y2-sy)/(y2-y1))*x1);
                if((xi>=sx) &&(((xi<=x1)&&(xi>=x2))||((xi>=x1)&&(xi<=x2)))){
                        intersect_no++;

                }
            }
    }
    return intersect_no % 2;

}

int detectCollisionSE(double xcoords,double ycoords,enemyShip* es){
    int intersect_no = 0.0;
    int lines = 3;
    double x1,y1,x2,y2,sx,sy,xi;
    sx = xcoords + s.x;
    sy = ycoords + s.y;
    for(int j=0;j<3;j++){
            x1 = es->x + es->xcoords[j%(lines)];
            y1 = es->y + es->ycoords[j%(lines)];
            x2 = es->x + es->xcoords[(j+1)%(lines)];
            y2 = es->y + es->ycoords[(j+1)%(lines)];
            if((sy <= y1 && sy >= y2) || (sy >= y1 && sy <= y2)){
                xi = (((sy-y1)/(y2-y1))*x2) + (((y2-sy)/(y2-y1))*x1);
                if((xi>=sx) &&(((xi<=x1)&&(xi>=x2))||((xi>=x1)&&(xi<=x2)))){
                        intersect_no++;
                }
            }
    }
    return intersect_no % 2;
}

int detectCollisionSEB(double xcoords,double ycoords,enemyBullet* eb){
    int intersect_no = 0;
    int lines = 3;
    double x1,y1,x2,y2,bx,by,xi =0;
    bx = eb->x + xcoords;
    by = eb->y + ycoords;
    for(int j=0;j<lines;j++){
            x1 = s.x + s.xcoords[j%(lines)];
            y1 = s.y + s.ycoords[j%(lines)];
            x2 = s.x + s.xcoords[(j+1)%(lines)];
            y2 = s.y + s.ycoords[(j+1)%(lines)];
            if((by <= y1 && by >= y2) || (by >= y1 && by <= y2)){
                xi = (((by-y1)/(y2-y1))*x2) + (((y2-by)/(y2-y1))*x1);
                if((xi>=bx) &&(((xi<=x1)&&(xi>=x2))||((xi>=x1)&&(xi<=x2)))){
                        intersect_no++;
                }

            }

    }
    return intersect_no % 2;
}


int detectCollisionSSM(double xcoords,double ycoords,spaceMine* sm){
    int intersect_no = 0.0;
    int lines = 6;
    double x1,y1,x2,y2,sx,sy,xi;
    sx = xcoords + s.x;
    sy = ycoords + s.y;
    for(int j=0;j<6;j++){
            x1 = sm->x + sm->xcoords[j%(lines)];
            y1 = sm->y + sm->ycoords[j%(lines)];
            x2 = sm->x + sm->xcoords[(j+1)%(lines)];
            y2 = sm->y + sm->ycoords[(j+1)%(lines)];
            if((sy <= y1 && sy >= y2) || (sy >= y1 && sy <= y2)){
                xi = (((sy-y1)/(y2-y1))*x2) + (((y2-sy)/(y2-y1))*x1);
                if((xi>=sx) &&(((xi<=x1)&&(xi>=x2))||((xi>=x1)&&(xi<=x2)))){
                        intersect_no++;
                }
            }
    }
    return intersect_no % 2;

}

void accelerateShip(){
    double acc = 1.0;

    s.vx = - acc * sin(toRadians(s.angle));
    s.vy = acc * cos(toRadians(s.angle));


    if(s.vx > 3.0){
        s.vx = 3.0;
    }
    if(s.vx < -3.0){
         s.vx = -3.0;
    }
    if(s.vy > 3.0){
        s.vy = 3.0;
    }
    if(s.vy < -3.0){
        s.vy = -3.0;
    }
}

bool removeSpaceMine(spaceMine sm){
    return sm.isDestroyed;
}

bool removeEnemySpaceShip(enemyShip es){
    return es.isDestroyed;
}

bool removeBullets(bullet b){
   return b.isDestroyed;
}
bool removeAsteroid(asteroid a){
    return a.isDestroyed;
}

bool removeEnemyBullet(enemyBullet eb){
    return eb.isDestroyed;
}



void drawAsteroids(asteroid* a){
    glColor3ub(100,100,100);
    glBegin(GL_POLYGON);
    for(int j=0;j<a->nVertices;j++){
        glVertex2d(a->xcoords[j],a->ycoords[j]);
    }
    glEnd();

}


void drawMoonCircle(moonCircle* c){
    glColor3ub(120, 120, 120);
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(c->moonx,c->moony);
        for(int i=0;i<=c->triangleAmount;i++){
            glVertex2f(c->moonx+(c->radius * cos(i*c->pi/c->triangleAmount)),
                       c->moony+(c->radius * sin(i*c->pi/c->triangleAmount)));
        }
    glEnd();
}


void drawStars(star* s){
    glColor3ub(255,255,255);
    glBegin(GL_POINTS);
            glVertex2d(s->x,s->y);
    glEnd();
}


void drawSpaceMine(spaceMine* sm){
    glColor3ub(255,0,0);

    glBegin(GL_TRIANGLES);
        glVertex2d(sm->xcoords[0],sm->ycoords[0]);
        glVertex2d(sm->xcoords[1],sm->ycoords[1]);
        glVertex2d(sm->xcoords[2],sm->ycoords[2]);
    glEnd();

    translate2D(0,-4.0);
    glBegin(GL_TRIANGLES);
        glVertex2d(sm->xcoords[3],sm->ycoords[3]);
        glVertex2d(sm->xcoords[4],sm->ycoords[4]);
        glVertex2d(sm->xcoords[5],sm->ycoords[5]);
    glEnd();

}

void drawBullet(bullet* b){
    glColor3ub(66, 135, 245);
    glBegin(GL_POINTS);
    glVertex2d(b->x,b->y);
    glEnd();
}

void drawEnemyBullet(enemyBullet* eb){
    translate2D(eb->x,eb->y);
    glColor3ub(255, 106, 0);
    glBegin(GL_POLYGON);
        glVertex2d(eb->xcoords[0],eb->ycoords[0]);
        glVertex2d(eb->xcoords[1],eb->ycoords[1]);
        glVertex2d(eb->xcoords[2],eb->ycoords[2]);
        glVertex2d(eb->xcoords[3],eb->ycoords[3]);
    glEnd();
}

void drawSpaceShip(spaceShip* s){


    if(s->thrustOn){
        glColor3ub(235,73,52);
        glBegin(GL_TRIANGLES);
            glVertex2d(s->xcoords[0],-(s->ycoords[0])-3.0);
            glVertex2d(s->xcoords[1]+1.0,s->ycoords[1]);
            glVertex2d(s->xcoords[2]-1.0,s->ycoords[2]);
        glEnd();

        glColor3ub(235, 140, 52);
        glBegin(GL_TRIANGLES);
            glVertex2d((s->xcoords[0]/1.5),-(s->ycoords[0]/1.5)-3.0);
            glVertex2d((s->xcoords[1]/1.5)+1.0,(s->ycoords[1]/1.5));
            glVertex2d((s->xcoords[2]/1.5)-1.0,(s->ycoords[2]/1.5));
        glEnd();
    }

    glColor3ub(100,100,100);
    glBegin(GL_TRIANGLES);
    glVertex2d(s->xcoords[0],s->ycoords[0]);
    glVertex2d(s->xcoords[1],s->ycoords[1]);
    glVertex2d(s->xcoords[2],s->ycoords[2]);
    glEnd();

    glPushMatrix();
    glColor3ub(66, 135, 245);
    translate2D(0.0,3);
    glBegin(GL_TRIANGLES);
    glVertex2d(cos(toRadians(90))*1.2,sin(toRadians(90))*1.5);
    glVertex2d(cos(toRadians(225))*1.2,sin(toRadians(225))*1.5);
    glVertex2d(cos(toRadians(315))*1.2,sin(toRadians(315))*1.5);
    glEnd();
    glPopMatrix();


    glPushMatrix();
    glColor3ub(66, 135, 245);
    translate2D(0.0,-2);
    glBegin(GL_QUADS);
    glVertex2d(-1,-1);
    glVertex2d(1,-1);
    glVertex2d(1,1);
    glVertex2d(-1,1);
    glEnd();
    glPopMatrix();


}

void drawSpaceShipLives(int p){
    translate2D(150.0 + p*7,92.0);
    scale2d(3.0,3.0);
    glColor3ub(0,255,0);
    glBegin(GL_LINE_LOOP);
    glVertex2d(cos(toRadians(90)),sin(toRadians(90)));
    glVertex2d(cos(toRadians(225)),sin(toRadians(225)));
    glVertex2d(cos(toRadians(315)),sin(toRadians(315)));
    glEnd();
}

void drawCurrSpaceShipLives(int p){
    translate2D(150.0 + p*7,92.0);
    scale2d(3.0,3.0);
    glColor3ub(0,255,0);
    glBegin(GL_TRIANGLES);
    glVertex2d(cos(toRadians(90)),sin(toRadians(90)));
    glVertex2d(cos(toRadians(225)),sin(toRadians(225)));
    glVertex2d(cos(toRadians(315)),sin(toRadians(315)));
    glEnd();
}

void drawEnemySpaceShip(enemyShip* es){

    glColor3ub(12, 102, 247);
    glBegin(GL_TRIANGLES);
        glVertex2d(es->xcoords[0],-(es->ycoords[0])-3.0);
        glVertex2d(es->xcoords[1]+1.0,es->ycoords[1]);
        glVertex2d(es->xcoords[2]-1.0,es->ycoords[2]);
    glEnd();

    glColor3ub(79, 136, 227);
    glBegin(GL_TRIANGLES);
        glVertex2d((es->xcoords[0]/1.5),-(es->ycoords[0]/1.5)-3.0);
        glVertex2d((es->xcoords[1]/1.5)+1.0,(es->ycoords[1]/1.5));
        glVertex2d((es->xcoords[2]/1.5)-1.0,(es->ycoords[2]/1.5));
    glEnd();

    glColor3ub(250,106, 0);
    glBegin(GL_TRIANGLES);
        glVertex2d(es->xcoords[0],es->ycoords[0]);
        glVertex2d(es->xcoords[1],es->ycoords[1]);
        glVertex2d(es->xcoords[2],es->ycoords[2]);
    glEnd();

    glPushMatrix();
    glColor3ub(66, 135, 245);
    translate2D(0.0,3.5);
    glBegin(GL_TRIANGLES);
    glVertex2d(cos(toRadians(90))*1.2,sin(toRadians(90))*1.5);
    glVertex2d(cos(toRadians(225))*1.2,sin(toRadians(225))*1.5);
    glVertex2d(cos(toRadians(315))*1.2,sin(toRadians(315))*1.5);
    glEnd();
    glPopMatrix();


    glPushMatrix();
    glColor3ub(66, 135, 245);
    translate2D(0.0,-2);
    glBegin(GL_QUADS);
    glVertex2d(-1,-1);
    glVertex2d(1,-1);
    glVertex2d(1,1);
    glVertex2d(-1,1);
    glEnd();
    glPopMatrix();

}

void drawSpaceShipHit(spaceShip *s){
    glColor4ub(255,0,0,50);
    glBegin(GL_TRIANGLES);
    glVertex2d(s->xcoords[0],s->ycoords[0]);
    glVertex2d(s->xcoords[1],s->ycoords[1]);
    glVertex2d(s->xcoords[2],s->ycoords[2]);
    glEnd();

}



void renderGame(){

    glClearColor(bgR,bgG,bgB,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);


    if(stage == 1){
       for(int i=0;i<circles.size();i++){
            glLoadIdentity();
            drawMoonCircle(&circles[i]);
       }
    }
    else{
        for(int i=0;i<stars.size();i++){
            glLoadIdentity();
            drawStars(&stars[i]);
        }
    }


    if(ebHit == true){
        glLoadIdentity();
        translate2D(s.x,s.y);
        rotate2D(toRadians(s.angle));
        drawSpaceShipHit(&s);
        ebHit = false;
    }
    else{
        glLoadIdentity();
        translate2D(s.x,s.y);
        rotate2D(toRadians(s.angle));
        drawSpaceShip(&s);
    }


    for(int i=0;i<asteroids.size();i++){
        if(!asteroids[i].isDestroyed){
            glLoadIdentity();
            translate2D(asteroids[i].x,asteroids[i].y);
            drawAsteroids(&asteroids[i]);
        }

    }

    glLoadIdentity();
    glPointSize(5.0);

    for(int i=0;i<vecBullet.size();i++){
        if(!vecBullet[i].isDestroyed){
            glLoadIdentity();
            drawBullet(&vecBullet[i]);
        }
    }
    for(int i=0;i<vecEnemyBullet.size();i++){
        if(!vecEnemyBullet[i].isDestroyed){
            glLoadIdentity();
            drawEnemyBullet(&vecEnemyBullet[i]);
        }
    }

    if(eships.size() > 0){
        for(int i=0;i<eships.size();i++){
            glLoadIdentity();
            translate2D(eships[i].x,eships[i].y);
            rotate2D(toRadians(eships[i].angle));
            drawEnemySpaceShip(&eships[i]);
       }
    }

    for(int i=0;i<spaceMines.size();i++){
        glLoadIdentity();
        translate2D(spaceMines[i].x,spaceMines[i].y);
        drawSpaceMine(&spaceMines[i]);
    }

    glLoadIdentity();
    char * scoreText = "SCORE : ";
    displayText(scoreText,GLUT_BITMAP_HELVETICA_18,5,90,0,255,0);

    glLoadIdentity();
    char * currScore = getScore(score);
    displayText(currScore,GLUT_BITMAP_HELVETICA_18,20,90,0,255,0);

    glLoadIdentity();
    char * livesText = "LIVES : ";
    displayText(livesText,GLUT_BITMAP_HELVETICA_18,140,90,0,255,0);

    for(int i=1;i<4;i++){
        glLoadIdentity();
        drawSpaceShipLives(i);
    }

    for(int i=1;i<lives+1;i++){
        glLoadIdentity();
        drawCurrSpaceShipLives(i);
    }

    glutSwapBuffers();
}


void initGame(){
    if(asteroids.size()>0){
        asteroids.clear();
    }
    if(vecBullet.size()>0){
        vecBullet.clear();
    }
    if(eships.size() > 0){
        eships.clear();
    }
    if(vecEnemyBullet.size() > 0){
        vecEnemyBullet.clear();
    }
    if(spaceMines.size() > 0){
        spaceMines.clear();
    }
    initSpaceShip();
    sHitCount = S_HITCOUNT;
    bulletTimer = EB_TIMER;
    createRandomAsteroids(3);
    esSpawnTimer = ES_SPAWNTIMER;
    smSpawnTimer = SM_SPAWNTIMER;
}



void keyInput(int key,int x,int y){
    if(key == GLUT_KEY_RIGHT){
        rightKey = 1;
    }
    else if(key == GLUT_KEY_LEFT){
        leftKey = 1;
    }
    else if(key == GLUT_KEY_UP){
        if(stageSelect == true){
            stage++;
            if(stage > 2){
                stage = 1;
            }
        }
        else if(gameOver == true){
            gameOverOpt++;
            if(gameOverOpt>2){
                gameOverOpt = 1;
            }
        }
        else if(gameStart == false){
            startMenuOpt++;
            if(startMenuOpt > 3){
                startMenuOpt = 1;
            }
        }
        else{
            upKey = 1;
        }
    }
    else if(key == GLUT_KEY_DOWN){
        if(stageSelect == true){
            stage--;
            if(stage < 1){
                stage = 2;
            }
        }
        else if(gameOver == true){
            gameOverOpt--;
            if(gameOverOpt<1){
                gameOverOpt = 2;
            }
        }else if(gameStart == false){
            startMenuOpt--;
            if(startMenuOpt < 1){
                startMenuOpt = 3;
            }
        }
        else{
            downKey = 1;
        }
    }
}

void releseInput(int key,int x,int y){
     if(key == GLUT_KEY_RIGHT){
        rightKey = 0;
    }
    else if(key == GLUT_KEY_LEFT){
        leftKey = 0;
    }
    else if(key == GLUT_KEY_UP){
        upKey = 0;
    }
    else if(key == GLUT_KEY_DOWN){
        downKey = 0;
    }
}

void gameKeys(unsigned char key,int x,int y){
    if(gameStart == true){
        if(key == SPACE){
            bullet nb;
            nb.isDestroyed = false;
            nb.x = s.x;
            nb.y = s.y;
            nb.bx = -5*sin(toRadians(s.angle));
            nb.by = 5*cos(toRadians(s.angle));
            vecBullet.push_back(nb);
            PlaySound("E:\\Spring (21-22)\\Computer Graphics\\Asteroids\\laserShoot.wav",NULL,SND_FILENAME|SND_ASYNC);
        }
    }
    if(stageSelect == true){
        if(key == 13){
            stageSelect = false;
            gameStart = true;
        }
    }

    if(vIns == true){
        if(key == 8){
            vIns = false;
        }
    }
    if(stageSelect == false && gameStart == false){
        if(startMenuOpt == 3){
            if(key == 13){
                stageSelect = true;
            }
        }
    }
    if(exitGame == false && gameStart == false){
        if(startMenuOpt == 1){
            if(key == 13){
                exitGame = true;
            }
        }
    }
    if(vIns == false && gameStart == false){
        if(startMenuOpt == 2){
            if(key == 13){
                vIns = true;
            }
        }
    }
    if(gameOver == true){
        if(key == 13){
            gameOver = false;
            gameStart = false;
        }
    }
}



//main game loop
void gameTimer(int){


    if(eships.size() == 1){
             bulletTimer = bulletTimer - 1;
             esSpawnTimer = ES_SPAWNTIMER;
    }
    else{
        esSpawnTimer--;
    }
    if(spaceMines.size() == 1){
        smSpawnTimer = SM_SPAWNTIMER;
    }
    else{
        smSpawnTimer--;
    }

    if(s.isDead == true){
        crashTimer = crashTimer + 1;
        esSpawnTimer = ES_SPAWNTIMER;
        smSpawnTimer = SM_SPAWNTIMER;
    }else{

    if(gameStart == true && gameOver == false){
        if(leftKey == 1){
            s.angle += 10;
        }
        if(rightKey == 1){
            s.angle -= 10;
        }
        if(upKey == 1){
            s.thrustOn = true;
            accelerateShip();
        }
        else{
            s.thrustOn = false;
        }
    }

    if(s.x > maxW){
        s.x = 0;
    }
    if(s.x < 0){
        s.x += maxW;
    }
    if(s.y > maxH){
        s.y = 0;
    }
    if(s.y < 0){
        s.y += maxH;
    }
    s.x += s.vx;
    s.y += s.vy;

    if(eships.size() > 0){
        for(int i=0;i<eships.size();i++){
            eships[i].x += eships[i].dx;
            eships[i].y += eships[i].dy;

        }

        for(int i=0;i<eships.size();i++){
            if(eships[i].x > maxW){
                eships[i].isDestroyed = true;
            }
            if(eships[i].x < 0){
                eships[i].isDestroyed = true;
            }
            if(eships[i].y > maxH){
                eships[i].isDestroyed = true;
            }
            if(eships[i].y < 0){
                eships[i].isDestroyed = true;
            }
        }
    }


    for(int i=0;i<asteroids.size();i++){
        if(asteroids[i].isDestroyed == false){
            asteroids[i].x += asteroids[i].vx;
            asteroids[i].y += asteroids[i].vy;
            if(asteroids[i].x > maxW){
                asteroids[i].x = 0;
            }
            if(asteroids[i].x < 0){
                asteroids[i].x += maxW;
            }
            if(asteroids[i].y > maxH){
                asteroids[i].y = 0;
            }
            if(asteroids[i].y < 0){
                asteroids[i].y += maxH;
            }
        }
    }

    for(int i=0;i<vecBullet.size();i++){
        if(!vecBullet[i].isDestroyed){
            vecBullet[i].x += vecBullet[i].bx;
            vecBullet[i].y += vecBullet[i].by;

            if(vecBullet[i].x > maxW || vecBullet[i].x < 0 ||
               vecBullet[i].y > maxH || vecBullet[i].y < 0
               )
            {

                vecBullet[i].isDestroyed = true;
            }
        }
    }

    for(int i=0;i<vecEnemyBullet.size();i++){
        if(!vecEnemyBullet[i].isDestroyed){
            vecEnemyBullet[i].x += vecEnemyBullet[i].ebx;
            vecEnemyBullet[i].y += vecEnemyBullet[i].eby;
        }
    }
    for(int i=0;i<vecEnemyBullet.size();i++){
            if(vecEnemyBullet[i].x < 0){
                vecEnemyBullet[i].isDestroyed = true;
            }
            if(vecEnemyBullet[i].x > maxW){
                vecEnemyBullet[i].isDestroyed = true;
            }
            if(vecEnemyBullet[i].y < 0){
               vecEnemyBullet[i].isDestroyed = true;
            }
            if(vecEnemyBullet[i].y > maxH){
               vecEnemyBullet[i].isDestroyed = true;
            }
    }

    for(int i=0;i<spaceMines.size();i++){
        spaceMines[i].x += spaceMines[i].dx;
        spaceMines[i].y += spaceMines[i].dy;
    }

    for(int i=0;i<spaceMines.size();i++){
        if(spaceMines[i].x > maxW){
            spaceMines[i].isDestroyed = true;
        }
        if(spaceMines[i].x < 0){
            spaceMines[i].isDestroyed = true;
        }
        if(spaceMines[i].y > maxH){
            spaceMines[i].isDestroyed = true;
        }
        if(spaceMines[i].y < 0){
            spaceMines[i].isDestroyed = true;
        }
    }


    for(int i=0;i<vecBullet.size();i++){
        if(vecBullet[i].isDestroyed == false){
            for(int j=0;j<asteroids.size();j++){
                if(asteroids[i].isDestroyed == false){
                    if(detectCollisionAB(&vecBullet[i],&asteroids[j])){
                        asteroids[j].isDestroyed = true;
                        vecBullet[i].isDestroyed = true;
                        if(asteroids[j].aSize == 3.0){
                            score += 5;
                            PlaySound("E:\\Spring (21-22)\\Computer Graphics\\Asteroids\\asteroidHit.wav",NULL,
                                      SND_FILENAME|SND_ASYNC);
                            initAsteroids(asteroids[j].x,asteroids[j].y,1,2.0);
                            initAsteroids(asteroids[j].x,asteroids[j].y,1,2.0);
                        }
                        else if(asteroids[j].aSize == 2.0){
                            score += 3;
                            PlaySound("E:\\Spring (21-22)\\Computer Graphics\\Asteroids\\asteroidHit.wav",NULL,
                                      SND_FILENAME|SND_ASYNC);
                            initAsteroids(asteroids[j].x,asteroids[j].y,1,1.5);
                            initAsteroids(asteroids[j].x,asteroids[j].y,1,1.5);
                        }
                        else{
                            PlaySound("E:\\Spring (21-22)\\Computer Graphics\\Asteroids\\asteroidDestroy.wav",NULL,
                                      SND_FILENAME|SND_ASYNC);
                            score += 1;
                        }
                        if(asteroids.size() <= 5){
                            if(generateRandD(-1,1)<0){
                                initAsteroids(0,generateRandD(0.0,maxH),1,3.0);
                            }
                            else{
                                initAsteroids(generateRandD(0.0,maxW),0,1,3.0);
                            }
                        }
                        break;
                    }
                }
            }
        }
    }
    if(vecEnemyBullet.size() > 0){
        for(int i=0;i<vecEnemyBullet.size();i++){
            for(int j=0;j<4;j++){
                if(vecEnemyBullet[i].isDestroyed == false){
                    if(detectCollisionSEB(vecEnemyBullet[i].xcoords[j],vecEnemyBullet[i].ycoords[j],&vecEnemyBullet[i])){
                        PlaySound("E:\\Spring (21-22)\\Computer Graphics\\Asteroids\\enemyBulletHit.wav",NULL,SND_ASYNC|SND_FILENAME);
                        ebHit = true;
                        vecEnemyBullet[i].isDestroyed = true;
                        sHitCount--;
                        if(sHitCount < 1){
                            vecEnemyBullet[i].isDestroyed = true;
                            s.isDead = true;
                            lives--;
                            PlaySound("E:\\Spring (21-22)\\Computer Graphics\\Asteroids\\shipDestroy.wav",
                                    NULL,SND_FILENAME|SND_ASYNC);
                        }
                    }
                }
            }

        }
    }

    for(int i=0;i<3;i++){
        if(s.isDead == false){
            for(int j=0;j<asteroids.size();j++){
                if(asteroids[j].isDestroyed == false){
                    if(detectCollisionAS(s.xcoords[i],s.ycoords[i],&asteroids[j])){
                        PlaySound("E:\\Spring (21-22)\\Computer Graphics\\Asteroids\\shipDestroy.wav",
                                  NULL,SND_FILENAME|SND_ASYNC);
                        lives--;
                        s.isDead = true;
                    }
                }
            }
        }
    }

    if(eships.size() > 0){
        for(int i=0;i<3;i++){
        for(int j=0;j<eships.size();j++){
            if(detectCollisionSE(s.xcoords[i],s.ycoords[i],&eships[j])){
                PlaySound("E:\\Spring (21-22)\\Computer Graphics\\Asteroids\\shipDestroy.wav",
                                  NULL,SND_FILENAME|SND_ASYNC);
                lives--;
                s.isDead = true;
            }
        }
        }
    }

    if(spaceMines.size()>0){
        for(int i=0;i<3;i++){
            if(s.isDead == false){
                for(int j=0;j<spaceMines.size();j++){
                    if(spaceMines[j].isDestroyed == false){
                        if(detectCollisionSSM(s.xcoords[i],s.ycoords[i],&spaceMines[j])){
                             PlaySound("E:\\Spring (21-22)\\Computer Graphics\\Asteroids\\shipDestroy.wav",
                                  NULL,SND_FILENAME|SND_ASYNC);
                            lives--;
                            s.isDead = true;
                        }
                    }
                }
            }
        }
    }


    }



    auto it = remove_if(vecBullet.begin(),vecBullet.end(),removeBullets);
    if(it != vecBullet.end()){
        vecBullet.erase(it);
    }

    auto it3 = remove_if(vecEnemyBullet.begin(),vecEnemyBullet.end(),removeEnemyBullet);
    if(it3 != vecEnemyBullet.end()){
        vecEnemyBullet.erase(it3);
    }


    auto it1 = remove_if(asteroids.begin(),asteroids.end(),removeAsteroid);
    if(it1 != asteroids.end()){
        asteroids.erase(it1);
    }


    auto it2 = remove_if(eships.begin(),eships.end(),removeEnemySpaceShip);
    if(it2 != eships.end()){
        eships.erase(it2);
    }

    auto it4 = remove_if(spaceMines.begin(),spaceMines.end(),removeSpaceMine);
    if(it4 != spaceMines.end()){
        spaceMines.erase(it4);
    }


    glutPostRedisplay();

    if(esSpawnTimer < 0){
        eships.clear();
        vecEnemyBullet.clear();
        spawnEnemyShip();
    }

    if(smSpawnTimer < 0){
        spaceMines.clear();
        spawnSpaceMine();
    }

    if(bulletTimer < 0){
        bulletTimer = EB_TIMER;
        initEnemyBullets();
    }

    if(crashTimer > 50){
        crashTimer = 0;
        s.isDead = false;
        if(lives == 0){
            glutDisplayFunc(gameOverMenu);
            glutTimerFunc(33,gameOverTimer,0);
            highscore = score;
            gameOver = true;
        }
        else{
            glutDisplayFunc(renderGame);
            glutTimerFunc(33,gameTimer,0);
            initGame();
        }
    }
    else if(gameOver == false){
        glutTimerFunc(33,gameTimer,0);
    }


}

void instructionMenu(){

    glClearColor(0.0f,0.0f,0.0f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glLoadIdentity();
    char* title = "INSTRUCTIONS";
    displayText(title,GLUT_BITMAP_HELVETICA_18,(maxW/2.0)-20.0,maxH-10.0,0,255,0);

    glLoadIdentity();
    char* ins1 = "Up arrow key to apply thrust";
    displayText(ins1,GLUT_BITMAP_HELVETICA_18,(maxW/2.0)-20.0,maxH-20.0,0,255,0);

    glLoadIdentity();
    char* ins2 = "Left/Right arrow key to rotate ship";
    displayText(ins2,GLUT_BITMAP_HELVETICA_18,(maxW/2.0)-20.0,maxH-30.0,0,255,0);

    glLoadIdentity();
    char* ins3 = "Space to shoot bullet";
    displayText(ins3,GLUT_BITMAP_HELVETICA_18,(maxW/2.0)-20.0,maxH-40.0,0,255,0);

    glLoadIdentity();
    translate2D(eships[0].x,eships[0].y);
    scale2d(0.5,0.5);
    drawEnemySpaceShip(&eships[0]);

    glLoadIdentity();
    char* es1 = "Enemy ship";
    displayText(es1,GLUT_BITMAP_HELVETICA_18,(maxW/2.0)-15.0,maxH-50.0,0,255,0);

    glLoadIdentity();
    char* es2 = "- Non destructable";
    displayText(es2,GLUT_BITMAP_HELVETICA_18,(maxW/2.0)-15.0,maxH-55.0,0,255,0);

    glLoadIdentity();
    char* es3 = "- Upon spawn will shoot bullets. If player hits bullet 3 times it is game over";
    displayText(es3,GLUT_BITMAP_HELVETICA_18,(maxW/2.0)-15.0,maxH-60.0,0,255,0);

    glLoadIdentity();
    char* es4 = "- Crashing onto the ship will result in game over aswell";
    displayText(es4,GLUT_BITMAP_HELVETICA_18,(maxW/2.0)-15.0,maxH-65.0,0,255,0);

    glLoadIdentity();
    translate2D(spaceMines[0].x,spaceMines[0].y);
    scale2d(0.5,0.5);
    drawSpaceMine(&spaceMines[0]);

    glLoadIdentity();
    char* sm1 = "Space mine";
    displayText(sm1,GLUT_BITMAP_HELVETICA_18,(maxW/2.0)-15.0,maxH-70.0,0,255,0);
    glLoadIdentity();

    glLoadIdentity();
    char* sm2 = "- Non Destructible";
    displayText(sm2,GLUT_BITMAP_HELVETICA_18,(maxW/2.0)-15.0,maxH-75.0,0,255,0);
    glLoadIdentity();

    glLoadIdentity();
    char* sm3 = "- Upon crashing into it will result in game over";
    displayText(sm3,GLUT_BITMAP_HELVETICA_18,(maxW/2.0)-15.0,maxH-80.0,0,255,0);
    glLoadIdentity();



    char* txt = "Press BackSpace to go back to main menu";
    displayText(txt,GLUT_BITMAP_HELVETICA_18,(maxW/2.0)-20.0,maxH-95.0,0,255,0);

    glutSwapBuffers();
}

void instructionTimer(int){
    if(vIns == false){
        glutTimerFunc(33,startMenuTimer,0);
        glutDisplayFunc(menu);
        eships.clear();
        spaceMines.clear();
    }
    else{
        glutPostRedisplay();
        glutTimerFunc(33,instructionTimer,0);
    }

}


void stageSelector(){

    glClearColor(0.0f,0.0f,0.0f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glLoadIdentity();
    glPointSize(5.0);

    glLoadIdentity();
    char * title = "Select Stage :";
    displayText(title,GLUT_BITMAP_HELVETICA_18,(maxW/2.0)-10.0,maxH-30.0,0,255,0);

    if(stage == 1){

        glLoadIdentity();
        glColor3ub(0,255,0);
        translate2D((maxW/2.0)-25.0,(maxH/2.0));
        scale2d(3.0,2.0);

        menuBar();


        glLoadIdentity();
        char * title = "Stage 1: Moon";
        displayText(title,GLUT_BITMAP_HELVETICA_18,(maxW/2.0)-15.0,(maxH/2.0)+6,0,0,0);
    }
    else{
        glLoadIdentity();
        char * title = "Stage 1: Moon";
        displayText(title,GLUT_BITMAP_HELVETICA_18,(maxW/2.0)-15.0,(maxH/2.0)+6,0,255,0);
    }
    if(stage == 2){
        glLoadIdentity();
        glColor3ub(0,255,0);
        translate2D((maxW/2.0)-25.0,(maxH/2.0)-10.0);
        scale2d(3.0,2.0);

        menuBar();

        glLoadIdentity();
        char * title = "Stage 2: Milky Way";
        displayText(title,GLUT_BITMAP_HELVETICA_18,(maxW/2.0)-15.0,(maxH/2.0)-4,0,0,0);

    }
    else{
        glLoadIdentity();
        char * title = "Stage 2: Milky Way";
        displayText(title,GLUT_BITMAP_HELVETICA_18,(maxW/2.0)-15.0,(maxH/2.0)-4,0,255,0);
    }

    glLoadIdentity();
    char * nav = "Up and down arrow key to navigate ,Press Enter to select";
    displayText(nav,GLUT_BITMAP_HELVETICA_18,65.0,10.0,0,255,0);

    glutSwapBuffers();
}




void selectStageTimer(int){
    if(gameStart == true){
        if(stage == 1){
            bgR = 60/255.0f;
            bgG = 60/255.0f;
            bgB = 60/255.0f;
            glutTimerFunc(33,gameTimer,0);
            glutDisplayFunc(renderGame);
            initGame();
            generateMoonCircles();
        }
        else{
            bgR = 0.0f;
            bgG = 0.0f;
            bgB = 0.0f;
            glutTimerFunc(33,gameTimer,0);
            glutDisplayFunc(renderGame);
            initGame();
            generateStars();
        }
    }
    else{
        glutPostRedisplay();
        glutTimerFunc(33,selectStageTimer,0);
    }

}


void gameOverMenu(){
    glClearColor(0.0f,0.0f,0.0f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glLoadIdentity();
    char * title = "GAME OVER";
    displayText(title,GLUT_BITMAP_HELVETICA_18,(maxW/2.0)-5.0,maxH-20.0,0,255,0);

    glLoadIdentity();
    char * startIns = "HIGH SCORE : ";
    displayText(startIns,GLUT_BITMAP_HELVETICA_18,(maxW/2.0)-10.0,maxH-30.0,0,255,0);

    glLoadIdentity();
    char * highscoretxt = getScore(highscore);
    displayText(highscoretxt,GLUT_BITMAP_HELVETICA_18,(maxW/2.0)+10.0,maxH-30.0,0,255,0);

    if(gameOverOpt == 1){
        glLoadIdentity();
        glColor3ub(0,255,0);
        translate2D((maxW/2.0)-25.0,maxH-56.0);
        scale2d(3.0,2.0);

        menuBar();

        glLoadIdentity();
        char* restartGametxt = "Restart game";
        displayText(restartGametxt,GLUT_BITMAP_HELVETICA_18,(maxW/2.0)-10.0,maxH-50.0,0,0,0);
    }
    else{
        glLoadIdentity();
        char* restartGametxt = "Restart game";
        displayText(restartGametxt,GLUT_BITMAP_HELVETICA_18,(maxW/2.0)-10.0,maxH-50.0,0,255,0);
    }

    if(gameOverOpt == 2){
        glLoadIdentity();
        glColor3ub(0,255,0);
        translate2D((maxW/2.0)-25.0,maxH-66.0);
        scale2d(3.0,2.0);

        menuBar();

        glLoadIdentity();
        char* restartGametxt = "Go to main menu";
        displayText(restartGametxt,GLUT_BITMAP_HELVETICA_18,(maxW/2.0)-10.0,maxH-60.0,0,0,0);
    }
    else{
        glLoadIdentity();
        char* restartGametxt = "Go to main menu";
        displayText(restartGametxt,GLUT_BITMAP_HELVETICA_18,(maxW/2.0)-10.0,maxH-60.0,0,255,0);
    }

    glLoadIdentity();
    char * nav = "Up and down arrow key to navigate ,Press Enter to select";
    displayText(nav,GLUT_BITMAP_HELVETICA_18,65.0,10.0,0,255,0);

    glutSwapBuffers();
}

void gameOverTimer(int){

    if(gameOver == false){
        highscore = 0;
        score = 0;
        lives = 3;
        gameOver = false;
        gameStart = false;

        if(gameOverOpt == 1){
            gameStart = true;
            if(stage == 1){
                bgR = 60/255.0f;
                bgG = 60/255.0f;
                bgB = 60/255.0f;
                glutTimerFunc(33,gameTimer,0);
                glutDisplayFunc(renderGame);
                initGame();
                generateMoonCircles();
            }
            else{
                bgR = 0.0f;
                bgG = 0.0f;
                bgB = 0.0f;
                glutTimerFunc(33,gameTimer,0);
                glutDisplayFunc(renderGame);
                initGame();
                generateStars();
            }
        }
        else{
            glutTimerFunc(33,startMenuTimer,0);
            glutDisplayFunc(menu);
        }

    }
    else{
         glutPostRedisplay();
         glutTimerFunc(33,gameOverTimer,0);
    }
}



void startMenuTimer(int){

    if(stageSelect == true){
            vIns = false;
            exitGame =false;
            glutTimerFunc(33,selectStageTimer,0);
            glutDisplayFunc(stageSelector);
            upKey = 0;
            downKey = 0;

    }
    else if(vIns == true){
         stageSelect = false;
         exitGame =false;
         glutTimerFunc(33,instructionTimer,0);
         glutDisplayFunc(instructionMenu);
         spawnExampleEnemyShip();
         spawnExampleSpaceMine();
    }
    else if(exitGame == true){
        stageSelect = false;
        vIns = false;
        exitGame =false;
        exit(0);
    }
    else{
            glutPostRedisplay();
            glutTimerFunc(33,startMenuTimer,0);
    }
}


void menu(){

    glClear(GL_COLOR_BUFFER_BIT);


    if(startMenuOpt == 3){
        glLoadIdentity();
        glColor3ub(0,255,0);
        translate2D((maxW/2.0)-25.0,maxH-56.0);
        scale2d(3.0,2.0);

        menuBar();

        glLoadIdentity();
        char* restartGametxt = "Start game";
        displayText(restartGametxt,GLUT_BITMAP_HELVETICA_18,(maxW/2.0)-10.0,maxH-50.0,0,0,0);
    }
    else{
        glLoadIdentity();
        char* restartGametxt = "Start game";
        displayText(restartGametxt,GLUT_BITMAP_HELVETICA_18,(maxW/2.0)-10.0,maxH-50.0,0,255,0);
    }

    if(startMenuOpt == 2){
        glLoadIdentity();
        glColor3ub(0,255,0);
        translate2D((maxW/2.0)-25.0,maxH-66.0);
        scale2d(3.0,2.0);

        menuBar();

        glLoadIdentity();
        char* restartGametxt = "Instructions";
        displayText(restartGametxt,GLUT_BITMAP_HELVETICA_18,(maxW/2.0)-10.0,maxH-60.0,0,0,0);
    }
    else{
        glLoadIdentity();
        char* restartGametxt = "Instructions";
        displayText(restartGametxt,GLUT_BITMAP_HELVETICA_18,(maxW/2.0)-10.0,maxH-60.0,0,255,0);
    }

    if(startMenuOpt == 1){
        glLoadIdentity();
        glColor3ub(0,255,0);
        translate2D((maxW/2.0)-25.0,maxH-76.0);
        scale2d(3.0,2.0);

        menuBar();

        glLoadIdentity();
        char* restartGametxt = "Quit Game";
        displayText(restartGametxt,GLUT_BITMAP_HELVETICA_18,(maxW/2.0)-10.0,maxH-70.0,0,0,0);
    }
    else{
        glLoadIdentity();
        char* restartGametxt = "Quit Game";
        displayText(restartGametxt,GLUT_BITMAP_HELVETICA_18,(maxW/2.0)-10.0,maxH-70.0,0,255,0);
    }

    glLoadIdentity();
    char * title = "ASTEROIDS";
    displayText(title,GLUT_BITMAP_HELVETICA_18,(maxW/2.0)-5.0,maxH-20.0,0,255,0);
    glLoadIdentity();

    glLoadIdentity();
        char * nav = "Up and down arrow key to navigate ,Press Enter to select";
        displayText(nav,GLUT_BITMAP_HELVETICA_18,65.0,10.0,0,255,0);

    glutSwapBuffers();

}

int main(int argc,char** argv){

    srand((unsigned int)time(NULL));
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
    glutInitWindowSize(w,h);
    glutCreateWindow("Asteroids");
    glutFullScreen();
    glutDisplayFunc(menu);
    glutReshapeFunc(init);
    glutIgnoreKeyRepeat(1);
    glutKeyboardFunc(gameKeys);
    glutSpecialFunc(keyInput);
    glutSpecialUpFunc(releseInput);
    glutTimerFunc(33,startMenuTimer,0);
    initBg();

    glutMainLoop();

    return 0;
}


void init(int wd,int hi){
    maxW = 100.0*wd/hi;
    maxH = 100.0;
    glViewport(0,0,wd,hi);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0,maxW,0.0,maxH,-1.0,1.0);
    glMatrixMode(GL_MODELVIEW);
}
