#include <SDL/SDL_gfxPrimitives.h>
#include <SDL/SDL_ttf.h>

#include "defines.h"
#include "ui/window.h"
#include "ui/graphics.h"

static void SetSurfaceAlpha (SDL_Surface *surface, Uint8 alpha);

static int col_r, col_g, col_b, col_a;
static TTF_Font *font;

void init_graphics(void)
{
    //first init the font library
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
    boxRGBA(get_screen(), x, get_y(y), x + w, get_y(y + h), col_r, col_g, col_b, col_a);
}

void draw_rect(float x, float y, float w, float h)
{
    rectangleRGBA(get_screen(), x, get_y(y), x + w, get_y(y + h), col_r, col_g, col_b, col_a);
}

void draw_line(float x1, float y1, float x2, float y2)
{
    lineRGBA(get_screen(), x1, get_y(y1), x2, get_y(y2), col_r, col_g, col_b, col_a);
}

void draw_circle(float cx, float cy, float r)
{
    aacircleRGBA(get_screen(), cx, get_y(cy), r, col_r, col_g, col_b, col_a);
}

void fill_circle(float cx, float cy, float r)
{
    filledCircleRGBA(get_screen(), cx, get_y(cy), r, col_r, col_g, col_b, col_a);
}

void draw_image(float x, float y, SDL_Surface *image)
{
    apply_surface((int) x, (int) get_y(y), image);
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
    SDL_Surface *text_surface;
    SDL_Color text_color = {r * 255, g * 255, b * 255, 0};
    text_surface = TTF_RenderText_Blended(font, text, text_color);
    SetSurfaceAlpha(text_surface, a * 255);

    //TODO: the - 27 is a hack because changing to SDL caused a weird offset.
    apply_surface(x - 5, get_y(y) - 29 + 5, text_surface);
}

float get_y(float y)
{
    return HEIGHT - y - 1;
}

int str_width(char* str)
{
    int size_w;
    int size_h;

    TTF_SizeText(font, str, &size_w, &size_h);

    return size_w;
}


// Changes a surface's alpha value, by altering per-pixel alpha if necessary.
// from http://stackoverflow.com/questions/3229391/sdl-sdl-ttf-transparent-blended-text
static void SetSurfaceAlpha (SDL_Surface *surface, Uint8 alpha)
{
    SDL_PixelFormat* fmt = surface->format;

    // If surface has no alpha channel, just set the surface alpha.
    if( fmt->Amask == 0 ) {
        SDL_SetAlpha( surface, SDL_SRCALPHA, alpha );
    }
    // Else change the alpha of each pixel.
    else {
        unsigned bpp = fmt->BytesPerPixel;
        // Scaling factor to clamp alpha to [0, alpha].
        float scale = alpha / 255.0f;

        SDL_LockSurface(surface);

        for (int y = 0; y < surface->h; ++y) 
        for (int x = 0; x < surface->w; ++x) {
            // Get a pointer to the current pixel.
            Uint32* pixel_ptr = (Uint32 *)( 
                    (Uint8 *)surface->pixels
                    + y * surface->pitch
                    + x * bpp
                    );

            // Get the old pixel components.
            Uint8 r, g, b, a;
            SDL_GetRGBA( *pixel_ptr, fmt, &r, &g, &b, &a );

            // Set the pixel with the new alpha.
            *pixel_ptr = SDL_MapRGBA( fmt, r, g, b, scale * a );
        }   

        SDL_UnlockSurface(surface);
    }       
}
