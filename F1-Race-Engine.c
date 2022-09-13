/*
 *
 * $ gcc F1-Race-Engine.c -o F1-Race -lSDL2
 *
 */

#include <SDL2/SDL.h>

#include <stdio.h>
#include <stdlib.h>

static SDL_bool exit_main_loop = SDL_FALSE;

int main(SDL_UNUSED int argc, SDL_UNUSED char *argv[]) {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	SDL_Window* window = SDL_CreateWindow("F1 Race", 100, 100, 620, 387, SDL_WINDOW_SHOWN); /* TODO: Name and SIZE */
	if (window == NULL) {
		fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	SDL_Renderer* render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (render == NULL) {
		fprintf(stderr, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
		SDL_DestroyWindow(window);
		SDL_Quit();
		return EXIT_FAILURE;
	}

	SDL_Surface* bitmap = SDL_LoadBMP("F1race/race.bmp");
	if (bitmap == NULL) {
		fprintf(stderr, "SDL_LoadBMP Error: %s\n", SDL_GetError());
		SDL_DestroyRenderer(render);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return EXIT_FAILURE;
	}

	SDL_Texture* texture = SDL_CreateTextureFromSurface(render, bitmap);
	if (texture == NULL) {
		fprintf(stderr, "SDL_CreateTextureFromSurface Error: %s\n", SDL_GetError());
		SDL_FreeSurface(bitmap);
		SDL_DestroyRenderer(render);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return EXIT_FAILURE;
	}
	SDL_FreeSurface(bitmap);

	while (!exit_main_loop) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					exit_main_loop = SDL_TRUE;
					break;
			}
		}
		SDL_RenderClear(render);
		SDL_RenderCopy(render, texture, NULL, NULL);
		SDL_RenderPresent(render);
		SDL_Delay(100); // TODO: Delay?
	}

	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(render);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return EXIT_SUCCESS;
}
