#include <GL/glu.h>
#include <GL/freeglut.h>
#include <math.h>
#include <unistd.h>
#include <time.h>

#define true 1
#define false 0

#define sizeg1 sizeof( ghost1_path)/sizeof( ghost1_path[0]) //Taille du tableau du fantome 1
#define sizeg2 sizeof( ghost2_path)/sizeof( ghost2_path[0]) //Taille du tableau du fantome 2

GLsizei ww, hh; //Taille de la fenetre
int startgame=false; // Permet de lancer le jeu
int gameover = false; //Permet de savoir si le jeu est finis
int over = false; //Permet de savoir si le joueur a perdu une vie durant la partie
int win = false; // Permet de savoir si le jeu est gagné
int ticks=0; // temps 0
//Temps d'image par seconde
uint32_t getTick()
{
    struct timespec ts;
    unsigned theTick = 0U;
    clock_gettime( CLOCK_REALTIME, &ts );
    theTick  = ts.tv_nsec / 10000;
    theTick += ts.tv_sec * 1000;
    return theTick;
};

int keyStates[256]; //Touche presser
float squareSize = 50.0; //Taille d'un item du jeu
float xx, yy, zz; // vision de la camera
int map_level =1;
int mode = 1; //Projection 1:Frustum 2:Ortho
double viewer[] = { 0, 0,1}; //Initial viewer location
float lft=-0.5,  rght=2.5,  bottom=1.5, top=-1.5, near=5; // Orientation de la camera
//Couleur
float blue[3] = {0,0.2,0.4};
float green[3] = {0,1,0};
float black[3] = {0,0,0};
float red[3] = {1,0,0};
float grey[3] = {0.1f, 0.1f, 0.1f};
float white[3] = {1.0f, 1.0f, 1.0f, 0.0f};
enum
{wall=0, dot=1,superdot=2,empty=3} Cell; // case du Labyrinth

int bitmap[11][11]=  // labyrinthe
{
    { wall, wall,  wall,  wall,  wall,  wall,  wall,  wall,  wall,    wall,      wall },
    { wall, empty,  dot,   dot,   dot,   wall,  dot,   dot,   dot,   superdot,	wall },
    { wall, dot,    wall,  wall,  dot,   wall,  dot,   wall,  wall,	  dot,       wall },
    { wall, dot,    wall,  dot,    dot,   dot,  dot,   dot,   wall,   dot,	   wall },
    { wall, dot,    wall,  dot,    wall,  wall,  wall,  dot,   wall,  dot,	      wall },
    { wall, dot,    dot,   dot,    dot,    dot,   dot,  dot,    dot,  dot,	     wall },
    { wall, dot,    wall,  dot,    wall,  wall,   wall,  dot,   wall,  dot,    wall },
    { wall, dot,    wall,  dot,    dot,    dot,    dot,  dot,   wall,  dot,	    wall },
    { wall, dot,    wall,  wall,   dot,    wall,  dot,  wall,   wall,  dot,    wall },
    { wall, superdot, dot, dot,    dot,   wall,  dot,   dot,     dot,  dot,	    wall },
    { wall, wall,     wall,wall,  wall,   wall,  wall,  wall,  wall,   wall,	    wall }
};
int map[11][11];  // copy du labyrinthe

struct
{
    float squareSize; // taille pacman
    int x, y; // coordonnee du pacman
    float life; //vie du pacman
    enum {right=0, down=1, left=2, up=3 } orientation;
    int points;  //Total points collectée
    int died;//Compte le nombre de fois que  Pacman meurt
    int superpower; // Pouvoir de manger les fantomes
    int tickspower; // temps du pouvoir

} Pacman;

struct
{
    float squareSize; // taille pac-gommes
    int x,y; // Coordonné des  pac-gommes
} Dot;

struct
{
    float squareSize; // taille du fantome
    int x,y, //  Coordonné du fantome
        life, // Vie du fantome
        detection,orientation, // Indice si le fantome detecte le pacman
        path; // Course du fantome

} Ghost[2];

// Course du fantome 2
int ghost2_path [] = { 5,5,6,5,7,5,7,4,7,3,6,3,5,3,4,3,3,3,3,4,3,5,3,6,3,7,4,7,5,7,6,7,7,7,7,6,7,5,8,5,9,5,9,4,9,3,9,2,9,1,8,1,7,1,6,1,6,2,6,3,7,3,7,4,7,5,7,6,7,7,6,7,6,8,6,9,7,9,8,9,9,9,9,8,9,7,9,6,9,5,8,5,7,5,6,5,5,5,4,5,3,5,2,5,1,5,1,4,1,3,1,2,1,1,2,1,3,1,4,1,4,2,4,3,3,3,3,4,3,5,3,6,3,7,4,7,4,8,4,9,3,9,2,9,1,9,1,8,1,7,1,6,1,5,2,5,3,5,4,5};
// Course du fantome 1
int ghost1_path [] = { 5, 5,4, 5,3, 5,3, 4,3, 3,4, 3,4, 2,4, 1,3, 1,2,1,1,1,1,2,1,3,1,4,1,5,2,5,3,5,3,6,3,7,4,7,4,8,4,9,3,9,2,9,1,9,1,8,1,7, 1,6,1,5,2,5,3,5,4,5,5,5,6,5,7,5,7,4,7,3,6,3,6,2,6,1,7,1,8,1,9,1,9,2,9,3,9,4,9,5,8,5,7,5,7,6,7,7,6,7,6,8,6,9,7,9,8,9,9,9,9,8,9,7,9,6,9,5,8,5,7,5,6,5,};

// Dessine les fantomes en 2D
void drawGhost2D(int n)
{
    int x, y;
    if(Ghost[n].life == 0)
    {
        return;
    }
    glBegin(GL_LINE_LOOP);
    if(Pacman.superpower == 1)
    {
        glColor4f (1.0f, 0.0f, 0.0f, 0.0f);
    }
    else
    {
        glColor3f (1.0, 0.0, 1.0);   //Magenta
    }

    //Draw head
    for (int k = 0; k < 32; k++)
    {
        x = (float)k / 2.0 * cos(360 * M_PI / 180.0) + (Ghost[n].x*squareSize+Ghost[n].squareSize/2);
        y = (float)k / 2.0 * sin(360 * M_PI / 180.0) + (Ghost[n].y*squareSize+Ghost[n].squareSize/2);
        for (int i = 180; i <= 360; i++)
        {
            glVertex2f(x, y);
            x = (float)k / 2.0 * cos(i * M_PI / 180.0) + (Ghost[n].x*squareSize+Ghost[n].squareSize/2);
            y = (float)k / 2.0 * sin(i * M_PI / 180.0) + (Ghost[n].y*squareSize+Ghost[n].squareSize/2);
            glVertex2f(x, y);
        }

    }
    glEnd();

    //Draw body
    glRectf((Ghost[n].x*squareSize+Ghost[n].squareSize/2) - 17, Ghost[n].y*squareSize+Ghost[n].squareSize/2, (Ghost[n].x*squareSize+Ghost[n].squareSize/2) + 15, (Ghost[n].y*squareSize+Ghost[n].squareSize/2) + 15);

    glBegin(GL_POINTS);
    glColor3f(0, 0.2, 0.4);
    glPointSize(2.0);
    //Draw legs
    glVertex2f((Ghost[n].x*squareSize+Ghost[n].squareSize/2) - 11, (Ghost[n].y*squareSize+Ghost[n].squareSize/2) + 14);
    glVertex2f((Ghost[n].x*squareSize+Ghost[n].squareSize/2) - 1, (Ghost[n].y*squareSize+Ghost[n].squareSize/2) + 14);
    glVertex2f((Ghost[n].x*squareSize+Ghost[n].squareSize/2) + 8, (Ghost[n].y*squareSize+Ghost[n].squareSize/2) + 14);
    //Draw eyes
    glVertex2f((Ghost[n].x*squareSize+Ghost[n].squareSize/2) + 4, (Ghost[n].y*squareSize+Ghost[n].squareSize/2) - 3);
    glVertex2f((Ghost[n].x*squareSize+Ghost[n].squareSize/2) - 7, (Ghost[n].y*squareSize+Ghost[n].squareSize/2) - 3);
    glEnd();
}

// Permet de chrnomometrer le temps que le pacman a un super pouvoir pour manger les fantomes
void pacman_timepower()
{
    if (Pacman.superpower ==1 && Pacman.tickspower > 10)
    {
        Pacman.superpower=0;
        Pacman.tickspower=0;
    }
    else if( Pacman.superpower== 1)
    {
        Pacman.tickspower++;
    }

}
// Les fantomes recherche le pacman après l'avoir perdu de vue
void backtosearch(int nb, int x1,int y1, int x2, int y2)
{
    if(nb==0)
    {
        for (int i=0; i< sizeg1 ; i=i+2)
        {
            if (x1== ghost1_path[i] && y1== ghost1_path[i+1])
            {
                Ghost[0].path =i;
                if(i == sizeg1-2)
                {
                    if( x2== ghost1_path[0] && y2== ghost1_path[1])
                        Ghost[0].orientation=0;
                    else
                    {
                        Ghost[0].orientation=1;

                    }
                }
                else
                {
                    if( x2== ghost1_path[i+2] && y2== ghost1_path[i+3] )
                        Ghost[0].orientation=0;
                    else
                    {
                        Ghost[0].orientation=1;
                    }

                }
            }
        }

    }
    else
    {
        for (int i=0; i< sizeg1 ; i=i+2)
        {
            if (x1== ghost2_path[i] && y1== ghost2_path[i+1])
            {
                Ghost[1].path =i;
                if(i == sizeg2-2)
                {
                    if( x2== ghost2_path[0] && y2== ghost2_path[1])
                        Ghost[1].orientation=0;
                    else
                    {
                        Ghost[1].orientation=1;

                    }
                }
                else
                {
                    if( x2== ghost2_path[i+2] && y2== ghost2_path[i+3] )
                        Ghost[1].orientation=0;
                    else
                    {
                        Ghost[1].orientation=1;
                    }

                }
            }
        }
    }

}
// Deplacement des fantomes
void moveGhost()
{
    int x0,y0, detect0;
    int x1,y1, detect1;
    detect0=Ghost[0].detection;
    x0=Ghost[0].x;
    y0=Ghost[0].y;
    detect1=Ghost[1].detection;
    x1=Ghost[1].x;
    y1=Ghost[1].y;

    if( !startgame )
    {
        return;
    }
    if(poursuit_Pacman_detection(0) == true)
    {
        Ghost[0].detection=1;
    }
    else
    {
        Ghost[0].detection=0;
        if( detect0 == true )  // Ghost run away
        {
            backtosearch(0,x0,y0,Ghost[0].x,Ghost[0].y);
        }
        else
        {
            searchPacman(0,Ghost[0].orientation);
        }
    }
    if(poursuit_Pacman_detection(1) == true)
    {
        Ghost[1].detection=1;
    }
    else
    {
        Ghost[1].detection=0;
        if( detect1 == true )  // Ghost run away
        {
            backtosearch(1,x1,y1,Ghost[1].x,Ghost[1].y);
        }
        else
        {
            searchPacman(1,Ghost[1].orientation);
        }
    }

}
// Poursuit le pacman si il la croiser durant son parcours
int poursuit_Pacman_detection(int nb )
{
    int min,max, newpostion;
    if( Pacman.x== Ghost[nb].x && Pacman.y== Ghost[nb].y)
    {
        return true;
    }
    if(Pacman.x == Ghost[nb].x )
    {
        if( Pacman.y < Ghost[nb].y)
        {
            min=Pacman.y;
            max=Ghost[nb].y;
            newpostion=Ghost[nb].y-1;
        }
        else
        {
            max=Pacman.y;
            min=Ghost[nb].y;
            newpostion=Ghost[nb].y+1;
        }
        for (int i=min; i< max; i ++ )
        {
            if( map[i][Pacman.x] == wall)
            {
                return false;
            }
        }
        Ghost[nb].y=newpostion;
        return true;
    }
    if(Pacman.y == Ghost[nb].y )
    {
        if( Pacman.x < Ghost[nb].x)
        {
            min=Pacman.x;
            max=Ghost[nb].x;
            newpostion=Ghost[nb].x-1;
        }
        else
        {
            max=Pacman.x;
            min=Ghost[nb].x;
            newpostion=Ghost[nb].x+1;
        }
        for (int i=min; i< max; i ++ )
        {
            if( map[Pacman.y][i] == wall)
            {
                return false;

            }
        }
        Ghost[nb].x=newpostion;
        return true;
    }

    return false;
}
// Cherche le pacman grace au parcours prédéfinis
void searchPacman(int nb, int orient)
{
    if (nb == 0 && orient == 0)
    {
        Ghost[0].x = ghost1_path[Ghost[0].path];
        Ghost[0].y = ghost1_path[Ghost[0].path+1];
        Ghost[0].path = Ghost[0].path +2 ;
        if (Ghost[0].path >= sizeg1) Ghost[0].path = 0;
    }
    else if( nb ==0 && orient ==1)
    {
        Ghost[0].x = ghost1_path[Ghost[0].path];
        Ghost[0].y = ghost1_path[Ghost[0].path+1];
        Ghost[0].path = Ghost[0].path -2 ;
        if (Ghost[0].path < 0) Ghost[0].path = sizeg1-2;
    }
    if(nb == 1 && orient == 0)
    {
        Ghost[1].x = ghost2_path[Ghost[1].path];
        Ghost[1].y = ghost2_path[Ghost[1].path+1];
        Ghost[1].path = Ghost[1].path +2 ;
        if (Ghost[1].path >=sizeg2 ) Ghost[1].path = 0;
    }
    else if( nb ==1 && orient ==1)
    {
        Ghost[1].x = ghost2_path[Ghost[1].path];
        Ghost[1].y = ghost2_path[Ghost[1].path+1];
        Ghost[1].path = Ghost[1].path -2 ;
        if (Ghost[1].path < 0) Ghost[1].path = sizeg2-2;
    }
}
// Dessine les fantome en 3D
void drawGhost3D()
{
    drawGhost3D1(0);
    drawGhost3D1(1);
}
void drawGhost3D1(int i)
{
    if(Ghost[i].life == 0)
    {
        return;
    }
    glPushMatrix();
    if(Pacman.superpower == 1)
    {
        glColor4f (1.0f, 0.0f, 0.0f, 0.0f);
    }
    else if(Ghost[i].detection == 1)
    {
        glColor3fv (green);
    }
    else
    {
        glColor3f (1.0, 0.0, 1.0);
    } //Magenta
    //glTranslated(x * squareSize,  * squareSize, 0); // Move world coordinate to ghost coordinate
    glTranslatef(Ghost[i].x*squareSize+Ghost[i].squareSize/2, Ghost[i].y*squareSize+Ghost[i].squareSize/2, Ghost[i].squareSize/2);
    glScaled(1, 1, 2); //Make sphere ellipse
    glutSolidSphere(16, 50, 50);
    glPopMatrix();
}

//Permet de savoir si le pacman a mangé un pac-gommes et si oui pacman gagne des points
int dotEaten(j,i)
{
    if (map[j][i] == dot )
    {

        Pacman.points++;
        map[j][i]= empty;
        return true;
    }
    if (map[j][i] == superdot)
    {
        //
        Pacman.points++;
        Pacman.superpower++;
        map[j][i]= empty;
        return true;
    }
    return false;
}
// Initialise le pacman
pacman_Init()
{
    Pacman.squareSize = 50.0;
    Pacman.x = 1;
    Pacman.y = 1;
    Pacman.life = 3;
    Pacman.orientation=right;
    Pacman.life=3;
    Pacman.points = 0; //Total Pacman.points collected
    Pacman.died = 0;//Count Pacman.died number of time
    Pacman.superpower=0;
}
//Initialise les fantomes
ghost_Init()
{
    Ghost[0].squareSize = 50.0;
    Ghost[0].x = 4;
    Ghost[0].y = 5;
    Ghost[0].life = 1;
    Ghost[1].squareSize = 50.0;
    Ghost[1].x = 4;
    Ghost[1].y = 5;
    Ghost[1].life = 1;
    Ghost[1].detection=0;
    Ghost[0].detection=0;
    Ghost[1].orientation=1;
    Ghost[0].orientation=1;
    Ghost[0].path=0;
    Ghost[1].path=0;

}
//Place le pacman
void pacman_setPacman(float x, float y)
{
    Pacman.x = x  ;
    Pacman.y = y  ;
}
//Dessin du pacman 2D
void pacman_drawPacman2D()
{
    float x, y;
    float rotation = 0;
    glBegin(GL_LINES);
    glColor3f(1.0, 1.0, 0.0); //Yellow
    //Improved double for statement with one for statement
    for (int i = 0 ; i < 360 ; i++) //Mouth_angle's range is 0~45degree
    {
        glVertex2f(Pacman.x*squareSize+Pacman.squareSize/2, Pacman.y*squareSize + Pacman.squareSize/2);
        x = 16 * cos((i + 90 * rotation) * M_PI / 180.0) + Pacman.x*squareSize + Pacman.squareSize/2 ;
        y = 16 * sin((i + 90 * rotation) * M_PI / 180.0) + Pacman.y*squareSize + Pacman.squareSize/2 ;
        glVertex2f(x, y);
    }



    glEnd();
}
// Dessin des vies du pacman
void pacman_drawLife()
{
    int x, y;
    glBegin(GL_LINES);
    glColor3f(1.0, 1.0, 0.0);
    for (int k = 0; k < 32; k++)
    {
        x = (float)k / 2.5 * cos((30) * M_PI / 180.0);
        y = (float)k / 2.5* sin((30) * M_PI / 180.0);
        for (int i = 30; i < 330; i++)
        {
            glVertex2f(x, y);
            x = (float)k / 2.5 * cos((i)* M_PI / 180.0);
            y = (float)k / 2.5 * sin((i)* M_PI / 180.0);
            glVertex2f(x, y);
        }
    }
    glEnd();
}
// Permet au fantome de manger le pacman si il y a collision
// Pacman peut manger les fantomes si il a un super pouvoir
void collision()
{
    if(startgame)
    {
        if (!over && !win && !gameover)
        {
            if (Pacman.x == Ghost[0].x && Pacman.y == Ghost[0].y && Ghost[0].life==1 && Pacman.superpower == 1)
            {
                Ghost[0].life = 0;
            }
            if (Pacman.x == Ghost[1].x && Pacman.y == Ghost[1].y && Ghost[1].life==1 && Pacman.superpower == 1)
            {
                Ghost[1].life = 0;
            }

            if (Pacman.x == Ghost[0].x && Pacman.y == Ghost[0].y && Ghost[0].life==1 && Pacman.superpower == 0||
                    Pacman.x == Ghost[1].x && Pacman.y == Ghost[1].y && Ghost[1].life==1 && Pacman.superpower == 0)
            {
                if(Pacman.life < 0)
                {
                    Pacman.life=0;
                }
                else
                {
                    Pacman.life--;
                    Pacman.died++;
                }

                if( Pacman.life == 0)
                {
                    win=false;
                    gameover=true;
                }
                else
                {
                    win=false;
                    over=true;
                }
            }
        }
    }
}
// Dessine Pacman en 3D
void pacman_drawPacman3D()
{
    glPushMatrix();

    glColor3f(1.0, 1.0, 0.0);
    glTranslatef(Pacman.x*squareSize+Pacman.squareSize/2, Pacman.y*squareSize+Pacman.squareSize/2, Pacman.squareSize/2);
    glutSolidSphere(15, 50, 50);

    glPopMatrix();
}
// Initialise la map
void Map()
{
    squareSize = 50.0;
    map_level = 1;
    memcpy(map, bitmap, sizeof map);
}

//Dessine le sol de la map
void drawFloor()
{
    glPushMatrix();
    glScalef(11, 11, 0.01);
    glTranslated(50/2, 50/2, -5);
    glColor3fv(white);
    glutSolidCube(50);
    glPopMatrix();
}
// Verifie si toutes les pac-gommes sont mangés
int endoflevel ()
{
    for (int x=0; x <11; x++)
    {
        for (int y=0; y <11; y++)
        {
            if((map[y][x] == dot) || (map[y][x] == superdot))
            {
                return false;
            }
        }
    }
    return true;
}
//Dessine le labyrinthe
void  drawLabyrinth()
{

    for (int x=0; x <11; x++)
    {
        for (int y=0; y <11; y++)
        {
            float obstacleX = x * squareSize  + squareSize / 2;
            float obstacleY = y * squareSize + squareSize / 2;
            float obstacleZ = squareSize / 2.0;
            if (map[y][x] == wall)   // mur
            {
                glPushMatrix();
                glTranslated(obstacleX, obstacleY, obstacleZ);
                glColor3fv(blue);
                glutSolidCube(50);
                glPopMatrix();
            }
            if (map[y][x] == dot)
            {

                glPushMatrix();
                glTranslatef(x * squareSize+squareSize/2, y * squareSize +squareSize/2, squareSize/2);
                glColor3fv(green);
                glutSolidSphere(3, 10, 10);
                glPopMatrix();
                glVertex2f(x * squareSize+squareSize/2, y* squareSize +squareSize/2);
                glColor3fv(green);
            }
            else if (map[y][x] == superdot)
            {
                glPushMatrix();
                glTranslatef(x* squareSize+squareSize/2, y * squareSize +squareSize/2, squareSize/2);
                glColor3fv(red);
                glutSolidSphere(3, 10, 10);
                glPopMatrix();
                glVertex2f(x * squareSize+squareSize/2, y * squareSize +squareSize/2);
                glColor3fv(red);
            }
        }
    }
}
// Initiliase le jeu personnage et map et place la lumière
void init()
{
    float light0_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
    float light0_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    float light0_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    float light0_position[] = { 1.0, -1.0, 1.0, 0.0 };
    Map();
    pacman_Init();
    ghost_Init();
    glClearColor(0.0, 0.0, 0.0, 1.0);

    for (int i = 0; i < 256; i++)
    {
        keyStates[i] = false;
    }
    //Using light
    glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
    glEnable(GL_COLOR_MATERIAL);//Set to use color of object
    glShadeModel(GL_SMOOTH);
}
// Relance le jeu selon si le joueur a perdu le jeu ou gagner ou si il a perdu une partie
void resetGame()
{
    int x=Pacman.x;
    int y=Pacman.y;
    int points=Pacman.points;
    int life=Pacman.life;
    for (int i = 0; i < 256; i++)
    {
        keyStates[i] = false;
    }
    pacman_Init();
    ghost_Init();
    if( over==true)
    {
        Pacman.x=x;
        Pacman.y=y;
        Pacman.points=points;
        Pacman.life=life;
        over=false;
    }
    else if( win == true )
    {
        Pacman.points=points;
        Pacman.life=life;
        win=false;
    }

}
// Permet de savoir si une touche du jeu a été presser
void keyPressed(unsigned char key, int x, int y)
{
    keyStates[key] = true;
}
//Permet de liberer la touche saisie
void keyUp(unsigned char key, int x, int y)
{
    keyStates[key] = false;
}
// Mets a jours les déplacements du pacman selon les touches pressée par l'utilisateur
void keyOperations()
{
    int move=0;
    if (keyStates[GLUT_KEY_LEFT] )
    {
        keyStates[GLUT_KEY_LEFT] = false;
        Pacman.orientation = left;
        move=1;
    }

    if (keyStates[GLUT_KEY_RIGHT] )
    {
        keyStates[GLUT_KEY_RIGHT] = false;
        Pacman.orientation = right;
        move=1;
    }

    if (keyStates[GLUT_KEY_DOWN] )
    {
        keyStates[GLUT_KEY_DOWN] = false;
        Pacman.orientation = down ;
        move=1;
    }
    if (keyStates[GLUT_KEY_UP] )
    {
        keyStates[GLUT_KEY_UP] = false;
        Pacman.orientation = up ;
        move=1;
    }
    if (move==0)
    {

    }
    else if (Pacman.orientation == right)
    {
        int isObstacle1 = map[(Pacman.y)][Pacman.x+1];

        if (!(isObstacle1 == wall))
        {
            pacman_setPacman((Pacman.x)+1, Pacman.y);

        }
    }
    else if (Pacman.orientation == down)
    {
        int isObstacle1 = map[(Pacman.y+1)][Pacman.x];
        if (!(isObstacle1 == wall))
        {
            pacman_setPacman((Pacman.x), (Pacman.y)+1);

        }
    }
    else if (Pacman.orientation == left)
    {
        int isObstacle1 = map[Pacman.y][(Pacman.x)-1];

        if (!(isObstacle1 == wall))
        {
            pacman_setPacman((Pacman.x-1), (Pacman.y));

        }
    }
    else
    {
        int isObstacle1 = map[(Pacman.y-1)][Pacman.x];
        if (!(isObstacle1 == wall))
        {
            pacman_setPacman((Pacman.x), (Pacman.y)-1);

        }
    }

    if (keyStates[27])
    {
        exit(-1);
    }
    /*
        if (keyStates['i']) //
        {
            keyStates['i'] = false;
            viewer[0]++;
        }
        if (keyStates['j']) //
        {
            keyStates['j'] = false;
            viewer[1]++;
        }
        if (keyStates['k']) //
        {
            keyStates['k'] = false;
            viewer[1]--;
        }
        if (keyStates[',']) //
        {
            keyStates[','] = false;
            viewer[0]--;
        }
        if (keyStates['l']) //
        {
            keyStates['l'] = false;
            viewer[2]--;
        }
        if (keyStates['m']) //
        {
            keyStates['m'] = false;
            viewer[2]++;
        }
        if (keyStates['b']) //
        {
            keyStates['b'] = false;
            xx++;
        }
        if (keyStates['n']) //
        {
            keyStates['n'] = false;
            xx--;
        }
        if (keyStates['c']) //
        {
            keyStates['c'] = false;
            yy++;
        }
        if (keyStates['v']) //
        {
            keyStates['v'] = false;
            yy--;
        }
        if (keyStates['o']) //
        {
            keyStates['o'] = false;
            lft--;
        }
        if (keyStates['p']) //
        {
            keyStates['p'] = false;
            lft++;
        }
        if (keyStates['a']) //
        {
            keyStates['a'] = false;
            rght++;
        }
        if (keyStates['r']) //
        {
            keyStates['r'] = false;
            rght--;
        }
        if (keyStates['z']) //
        {
            keyStates['z'] = false;
            bottom--;
        }
        if (keyStates['d']) //
        {
            keyStates['d'] = false;
            bottom++;
        }
        if (keyStates['t']) //
        {
            keyStates['t'] = false;
            top++;
        }
        if (keyStates['h']) //
        {
            keyStates['h'] = false;
            top--;
        }
        if (keyStates['f']) //
        {
            keyStates['f'] = false;
            near--;
        }
        if (keyStates['x']) //
        {
            keyStates['x'] = false;
            near++;
        }
    */
    if (keyStates[' '])
    {
        gameState();
    }
    glutPostRedisplay();
}
//Permet de savoir le statue du jeu / Jeu démarer / Gagner/perdu / partie perdu
void gameState()
{
    if(!startgame)
    {
        win=false;
        over=false;
        map_level=1;
        Map();
        resetGame();
        startgame=true;
    }
    else if (gameover )
    {
        win=false;
        over=false;
        map_level=1;
        Map();
        resetGame();
        startgame=true;
        gameover=false;
    }
    else if(win )
    {
        map_level=1;
        Map();
        resetGame();
    }
    else if( over)
    {
        resetGame();
    }
}
// Affiche une page selon l'état du jeu
void resultScreen()
{
    glClearColor(0, 0.2, 0.4, 1.0);
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);

    if (win)
    {

        char *message = "FELICITATION, TU AS GAGNE! ";
        glColor3f(1, 1, 1);
        glRasterPos2f(200, 300);
        while (*message)
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *message++);

        message = "POUR RECOMMENCER LE JEU APPUIE SUR LA TOUCHE ESPACE.";
        glRasterPos2f(170, 550);
        while (*message)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *message++);
    }

    else if (gameover)
    {
        char *message = "OH NON TU AS PERDU ... ";
        glColor3f(1, 1, 1);
        glRasterPos2f(250, 300);
        while (*message)
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *message++);

        message = "TU AS : ";
        glRasterPos2f(260, 400);
        while (*message)
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *message++);
        char str[10] = "";
        sprintf(str, "%d", Pacman.points);
        message = str;
        glRasterPos2f(350, 400);
        while (*message)
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *message++);
        message = " points!";
        glRasterPos2f(385, 400);
        while (*message)
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *message++);

        message = "POUR (RE)COMMENCER LE JEU APPUIE SUR LA TOUCHE ESPACE";
        glRasterPos2f(170, 550);
        while (*message)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *message++);
    }
    else if (over)
    {
        char* message = " Continuer? ";
        glRasterPos2f(320, 350);
        glColor3f(1, 1, 1);
        while (*message)
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *message++);

        message = "POUR (RE)COMMENCER LE JEU APPUIE SUR LA TOUCHE ESPACE";
        glRasterPos2f(200, 410);
        glColor3f(1, 1, 1);
        while (*message)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *message++);
    }
}
// Page de lancement du programme
void welcomeScreen()
{
    glClearColor(0, 0.2, 0.4, 1.0); //Navy
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);

    char *message = "PACMAN 3D VUE ISOMETRIQUE ";
    glColor3f(1, 1, 1);
    glRasterPos2f(200, 250);
    while (*message)
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *message++);

    message = "**COMMENT JOUER ?**";
    glRasterPos2f(310, 400);
    while (*message)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *message++);

    message = "UTILISER LES FLECHES POUR BOUGER LE PACMAN";
    glRasterPos2f(100, 450);
    while (*message)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *message++);

    message = "POUR (RE)COMMENCER LE JEU APPUIE SUR LA TOUCHE ESPACE";
    glRasterPos2f(170, 500);
    while (*message)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *message++);
}
//Dessine l'écran en fonction de l'angle de vison
void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (mode == 1)
    {
        glFrustum(lft,  rght,  bottom, top,  near,  1000);
    }
    else if (mode == 2)
    {
        glOrtho(0, 750, 750, 0, -750, 750);
    }
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    ww = w;
    hh = h;
}
// Mets a jours la fenêtre 3D
void mapScreen3D()
{
    usleep(100);
    mode = 1;
    reshape(ww, hh);
    viewer[0] =  -42;
    viewer[1] = -141;
    viewer[2] = 38;
    xx = -42;
    yy = 65 ;
    zz = -42;
    gluLookAt(viewer[0], viewer[1], viewer[2], xx, yy, zz, 0, 1, 0);
    dotEaten(Pacman.y,Pacman.x);

    if (startgame)
    {
        if (!over && !win && !gameover)
        {
            glPushMatrix();
            glRotated(90, 1, 0, 0);
            glScalef(0.1, 0.1, 0.1);//echelle
            glTranslated(-375, -375, -5);// centre  jeu au milieu de la fentre
            drawFloor();
            drawLabyrinth();

            pacman_setPacman(Pacman.x, Pacman.y);
            pacman_drawPacman3D();
            drawGhost3D();
            glPopMatrix();
        }
        else
        {
            mode = 2;
            reshape(ww, hh);
            resultScreen();
        }
    }
    else
    {
        mode = 2;
        reshape(ww, hh);
        glLoadIdentity();
        welcomeScreen();
    }
}
// Mets a jours la fenêtre 2D
void mapScreen2D()
{
    //Map & Information
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    mode = 2;
    reshape(ww, hh);
    if (startgame && !gameover && !win && !over )
    {
        //Print score
        glPushMatrix();
        glColor3f(1, 1, 1);
        char *message = "score : ";
        glRasterPos2f(20, 60);
        while (*message)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *message++);
        char str[10] = "";
        sprintf(str, "%d", Pacman.points);
        message = str;
        glRasterPos2f(80, 60);
        while (*message)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *message++);
        glPopMatrix();

        glPushMatrix();
        glColor3f(1.0, 1.0, 0.0);
        if (Pacman.life >= 1)
        {
            glTranslatef(squareSize - 20, squareSize - 25, 60);// placer les petites vies du pacman
            pacman_drawLife();

            if (Pacman.life >= 2)
            {
                glTranslatef(30, 0, 0);
                pacman_drawLife();

                if (Pacman.life >= 3)
                {
                    glTranslatef(30, 0, 0);
                    pacman_drawLife();
                }
            }
        }
        glPopMatrix();

        glPushMatrix();
        glScalef(0.3, 0.3, 0.3);
        glTranslated(1600, 50, 50);// position de la fenetre 2D
        drawFloor();
        drawLabyrinth();
        pacman_drawPacman2D();
        drawGhost2D(0);
        drawGhost2D(1);
        glPopMatrix();
    }
}

//Affichage des fenetres et mets a jours les données du jeu tout le temps
void display()
{
    uint32_t t1,t2;
    t1=getTick();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 1.0); //Black
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    mapScreen3D();
    mapScreen2D();

    collision();

    if( endoflevel())
    {
        win=true;
    }
    keyOperations();
    if ( ticks%8 == 0 )
    {
        pacman_timepower();
        moveGhost();
    }
    glutSwapBuffers();
    t2=getTick();
    useconds_t u= t2-t1;
    if( t2-t1 < 2000)
    {
        u=2000-u;
        usleep(u);

    }
    ticks=ticks+1;
}

int main(int argc, char** argv)
{
    //Initialise et creer la fenetre
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("3D PACMAN");

    //Definis toutes les fonctions de control
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(display);
    glutKeyboardFunc(keyPressed);
    glutKeyboardUpFunc(keyUp);
    glutSpecialFunc (keyPressed);
    glutSpecialUpFunc (keyUp);

    //Joue le jeu
    init();
    glutMainLoop();

    return 0;
}


