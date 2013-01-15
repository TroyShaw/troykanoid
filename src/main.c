#include <windows.h>
#include <sys/time.h>
#include <GL/glut.h>
#include <stdbool.h>
#include "main.h"
#include "pong.h"
#include "renderer.h"

long long timeT = 0;
struct timeval tv;

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    initGlutWindow();
    initGL();

    glutMainLoop();

    return EXIT_SUCCESS;
}

void initGlutWindow()
{
    glutInitWindowSize(WIDTH,HEIGHT);
    glutInitWindowPosition(10,10);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("Pong");

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
    if (key > 'z') key = key - ('A' - 'a');
    switch (key)
    {
    case 27:
        exit(0);
        break;
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
        tickGame();
        timeT = milli;

        //only need to redraw if we have changed state
        glutPostRedisplay();
    }
}
