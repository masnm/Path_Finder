#ifndef AB_SDL_H
#define AB_SDL_H

#include <SDL2/SDL.h>

SDL_Window *window;
SDL_Renderer *renderer;

void ab_sdl_start ( int width, int height )
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		fprintf(stderr, "Could not initialize sdl2: %s\n", SDL_GetError());
		exit ( 1 );
		// return EXIT_FAILURE;
	}

	window = SDL_CreateWindow("Path Finder in C",
										  100, 100,
										  width, height,
										  SDL_WINDOW_SHOWN);

	if (window == NULL) {
		fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
		exit ( 1 );
		// return EXIT_FAILURE;
	}

	renderer = SDL_CreateRenderer(window, -1,
												SDL_RENDERER_ACCELERATED |
												SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL) {
		SDL_DestroyWindow(window);
		fprintf(stderr, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
		exit ( 1 );
		// return EXIT_FAILURE;
	}
}

void ab_sdl_quit ()
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

#endif /* AB_SDL_H */
