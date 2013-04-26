#include <Windows.h>
#include <sys/time.h>
#include <GL/glut.h>
#include <stdbool.h>
#include "highscore.h"
#include "main.h"
#include "pong.h"
#include "renderer.h"

void initGlutWindow();
void initGL();
void display();
void key(unsigned char key, int x, int y);
void keyUp(unsigned char key, int x, int y);
void idle();
void process();

long long timeT = 0;
struct timeval tv;

int main(int argc, char **argv)
{
    //initialize the random number generator with new value
    srand(time(NULL));
    //load scores
    loadHighscoresFromDisc();
    //init stuff
    glutInit(&argc, argv);
    initGlutWindow();
    initGL();

    //main game happens here
    glutMainLoop();

    return EXIT_SUCCESS;
}

void initGlutWindow()
{
    //center the screen
    RECT rc;
    GetWindowRect(GetDesktopWindow(), &rc);
    glutInitWindowSize(WIDTH,HEIGHT);
    glutInitWindowPosition((rc.right - WIDTH) / 2, (rc.bottom - HEIGHT) / 2);

    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("Troykanoid");

    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutKeyboardUpFunc(keyUp);
    glutIdleFunc(idle);
}

void initGL()
{
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH, GL_NICEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, WIDTH, 0.0, HEIGHT, -1.0, 1.0);
}

void key(unsigned char key, int x, int y)
{
    if (key == 27) exit(0);


    if (game.mode == POST_GAME)
    {
        switch (key)
        {
        //enter
        case 13:
            if (verifyHighscoreName())
            {
                enterScore(game.player.score);
                saveHighscoresToDisc();
                game.mode = MAIN_MENU;
            }
        default:
            enterChar(key);
        }

        process();
        return;
    }

    if (key > 'z') key = key - ('A' - 'a');
    switch (key)
    {
    case 32:
        game.Keymanager.space = true;
        break;
    case 'a':
    case 'A':
        game.Keymanager.left = true;
        break;
    case 'd':
    case 'D':
        game.Keymanager.right = true;
        break;
    case 'p':
    case 'P':
        game.Keymanager.pause = true;
    }

    process();
}

void keyUp(unsigned char key, int x, int y)
{
    if (key > 'z') key = key - ('A' - 'a');
    switch (key)
    {
    case 32:
        game.Keymanager.space = false;
        break;
    case 'a':
    case 'A':
        game.Keymanager.left = false;
        break;
    case 'd':
    case 'D':
        game.Keymanager.right = false;
        break;
    case 'p':
    case 'P':
        game.Keymanager.pause = false;
    }

    process();
}

void specialFunc()
{

}

void idle()
{
    process();
}

void display()
{
    render();
}

void process()
{
    gettimeofday(&tv, NULL);

    unsigned long long milli = (unsigned long long)(tv.tv_sec) * 1000 + (unsigned long long)(tv.tv_usec) / 1000;

    if (milli - timeT >= TICK_TIME)
    {
        tick();
        timeT = milli;

        //only need to redraw if we have changed state
        glutPostRedisplay();
    }
}
