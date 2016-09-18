#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "defines.h"
#include "ui/window.h"
#include "ui/graphics.h"

static int col_r, col_g, col_b, col_a;
static TTF_Font *font;

void init_graphics(void)
{
    //init image library
    int flags = IMG_INIT_PNG;
    int initted = IMG_Init(IMG_INIT_PNG);
    
    if ((initted & flags) != flags)
    {
        printf("failed to init SDL_image: %s\n", IMG_GetError());
        SDL_Quit();
        exit(1);
    }

    //init the font library
    if (TTF_Init() != 0)
    {
        printf("failed to init SDL_ttf: %s\n", TTF_GetError());
        SDL_Quit();
        exit(1);
    }

    //then get our font
    font = TTF_OpenFont("fonts/font.ttf", 24);

    if (font == NULL)
    {
        printf("failed to open font: %s\n", TTF_GetError());
        SDL_Quit();
        exit(1);
    }
}

void cleanup_graphics(void)
{
    IMG_Quit();
    TTF_Quit();
}


void set_color3f(int r, int g, int b)
{
    col_r = r;
    col_g = g;
    col_b = b;
}

void set_color4f(int r, int g, int b, int a)
{
    col_r = r;
    col_g = g;
    col_b = b;
    col_a = a;
}

//TODO: these functions seem to draw rectangles 1 pixel too high. Investigate
void fill_rect(float x, float y, float w, float h)
{
    boxRGBA(get_renderer(), x, get_y(y), x + w, get_y(y + h), col_r, col_g, col_b, col_a);
}

void draw_rect(float x, float y, float w, float h)
{
    rectangleRGBA(get_renderer(), x, get_y(y), x + w, get_y(y + h), col_r, col_g, col_b, col_a);
}

void draw_line(float x1, float y1, float x2, float y2)
{
    lineRGBA(get_renderer(), x1, get_y(y1), x2, get_y(y2), col_r, col_g, col_b, col_a);
}

void draw_circle(float cx, float cy, float r)
{
    aacircleRGBA(get_renderer(), cx, get_y(cy), r, col_r, col_g, col_b, col_a);
}

void fill_circle(float cx, float cy, float r)
{
    filledCircleRGBA(get_renderer(), cx, get_y(cy), r, col_r, col_g, col_b, col_a);
}

void draw_image(float x, float y, struct Texture image)
{
    apply_textureReal((int) x, (int) get_y(y), image);
}

void center_print(float y, const char* text, float r, float g, float b, float a)
{
    int size_w;
    int size_h;

    TTF_SizeText(font, text, &size_w, &size_h);

    float x = (WIDTH - size_w) / 2;
    draw_string(x, y, text, r, g, b, a);
}

void draw_string(float x, float y, const char* text, float r, float g, float b, float a)
{
    if (text == NULL || strlen(text) == 0) return;

    SDL_Surface *text_surface;
    SDL_Color text_color = {r * 255, g * 255, b * 255, 0};
    text_surface = TTF_RenderText_Blended(font, text, text_color);
    
    if (text_surface == NULL)
    {
        printf("surface was null for string: %s\n", text);
        exit(1);
    }

    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(get_renderer(), text_surface);

    SDL_FreeSurface(text_surface);

    //TODO: the - 27 is a hack because changing to SDL caused a weird offset.
    apply_texture(x - 5, get_y(y) - 29 + 5, textTexture);
    SDL_DestroyTexture(textTexture);
}

float get_y(float y)
{
    return HEIGHT - y - 1;
}

int str_width(const char* str)
{
    int size_w;
    int size_h;

    TTF_SizeText(font, str, &size_w, &size_h);

    return size_w;
}
