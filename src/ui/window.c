#include "window.h"
#include "defines.h"

static SDL_Surface *screen;
static SDL_Renderer *renderer;

bool init_window(const char* title, int width, int height)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
	{
		return false;
	}

	//screen = SDL_SetVideoMode(width, height, 32, SDL_HWSURFACE);

	SDL_Window *new_screen = SDL_CreateWindow(title, 
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		width, height,
		SDL_WINDOW_SHOWN);

	renderer = SDL_CreateRenderer(new_screen, -1, SDL_RENDERER_PRESENTVSYNC);

	return true;
}

void dispose_window(void)
{
	SDL_FreeSurface(screen);
}

SDL_Renderer *get_renderer(void)
{
	return renderer;
}

void clear_screen(int r, int g, int b, int a)
{
	// Uint32 col = SDL_MapRGBA(screen->format, r, g, b, a);

	// SDL_FillRect(screen, NULL, col);

	SDL_SetRenderDrawColor(renderer, r, g, b, a);
	SDL_RenderClear(renderer);

}

void apply_surface(int x, int y, SDL_Surface* source)
{
	SDL_Texture *sdlTexture = SDL_CreateTextureFromSurface(renderer, source);

	int w, h;
	SDL_QueryTexture(sdlTexture, NULL, NULL, &w, &h);

	SDL_Rect offset;

	offset.x = x;
	offset.y = y;

	offset.w = w;
	offset.h = h;

	SDL_RenderCopy(renderer, sdlTexture, NULL, &offset);
}

void apply_texture(int x, int y, SDL_Texture *texture)
{
	int w, h;
	SDL_QueryTexture(texture, NULL, NULL, &w, &h);


	SDL_Rect offset;

	offset.x = x;
	offset.y = y;

	offset.w = w;
	offset.h = h;

	SDL_RenderCopy(renderer, texture, NULL, &offset);
}

void apply_textureReal(int x, int y, struct Texture texture)
{
	SDL_Rect offset;

	offset.x = x;
	offset.y = y;

	offset.w = texture.w;
	offset.h = texture.h;

	SDL_RenderCopy(renderer, texture.texture, NULL, &offset);
}

void flip_screen(void)
{
	SDL_RenderPresent(renderer);
}
