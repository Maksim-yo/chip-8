//#include "graphics.h"
#include "parser.h"
#include <SDL_image.h>
#include <SDL.h>


// def all black
//uint16_t screen_buf[64 * 32];


#pragma warning(disable : 4996)

SDL_Window* window = NULL;
SDL_Surface* surface = NULL;
SDL_Renderer* gRenderer = NULL;
SDL_Texture* gTexture = NULL;
SDL_Rect sprite;
SDL_Surface* gScreen = NULL;
SDL_Surface* loadedSurface = NULL;
int SDL_SCREEN_WIDTH = 640;
int SDL_SCREEN_HEIGHT = 320;
SDL_Texture* fonts[16];
uint16_t addr = 0;

bool quit = false;
SDL_Event e;



SDL_Texture* load_sprite(const char* path) {

	loadedSurface = IMG_Load(path);

	if (loadedSurface == NULL)

	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path, IMG_GetError());
	}

	gTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
	SDL_FreeSurface(loadedSurface);

	if (gTexture == NULL) {

		printf("Unable to load texture from surface %s! SDL_Texture Errpr  %s\n:", path, IMG_GetError());

	}
	return gTexture;

}

void init_sprites() {

	for (int i = 0; i < FONTS; ++i) {

		char path[100];
		sprintf(path, "D:/code/c++/proj/chip8/chip8/assets/font%c.png", fonts_data[i].val);
		fonts[i] = load_sprite(path);
		loadedSurface = NULL;

	}

	printf("Sprites loaded\n");

}

void close() {

	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(window);
	SDL_DestroyTexture(gTexture);

	for (int i = 0; i < FONTS; ++i) {

		SDL_DestroyTexture(fonts[i]);

	}

	gScreen = NULL;
	surface = NULL;
	gRenderer = NULL;
	window = NULL;
	IMG_Quit();
	SDL_Quit();

}

void paint_surf(int x, int y, int n) {

	SDL_Rect rect;
	rect.w = 40;
	rect.h = 50;
	rect.x = x;
	rect.y = y;
	SDL_RenderCopy(gRenderer, (SDL_Texture*)registers[I], NULL, &rect);

}

void paint_surf1(int x) {

	SDL_Rect rect;
	rect.w = 40;
	rect.h = 50;
	rect.x = 200;
	rect.y = 200;
	SDL_RenderCopy(gRenderer, fonts[x], NULL, &rect);

}

void clear_screen() {

	SDL_RenderClear(gRenderer);

}

int init() {

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {

		return 1;

	}

	sprite.w = 40;
	sprite.h = 50;

	window = SDL_CreateWindow("CHIP-8 EMULATOR", SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED, SDL_SCREEN_WIDTH, SDL_SCREEN_HEIGHT,
		SDL_WINDOW_SHOWN);


	if (window == NULL) {

		printf("Window couldn't be created SDL Error: %s\n", SDL_GetError());
		return 1;

	}

	gRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	if (gRenderer == NULL)
	{
		fprintf(stderr,"ERROR: Renderer could not be created! SDL Error: %s\n", SDL_GetError());
		return 1;
	}
	else
	{
		//Initialize renderer color
		SDL_SetRenderDrawColor(gRenderer, 0x0, 0x0, 0x0, 0xFF);
		gScreen = SDL_GetWindowSurface(window);

		//Initialize PNG loading
		int imgFlags = IMG_INIT_PNG;
		if (!(IMG_Init(imgFlags) & imgFlags))
		{
			fprintf(stderr,"ERROR: SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
			return 1;
		}
	}

	init_sprites();
	SDL_RenderPresent(gRenderer);
	return 0;
}

uint16_t instr() {

	return data[addr++];

}

int main(int argc, char* argv[]) {

	if (init() != 0)
		exit(-1);

	//read_data(argv[1], argc);

	testing_stack();
	testing_paint();
	paint_surf1(5);
	
	while (!quit) {

		while (data[addr] != 0)

			parse(instr());

		while ((SDL_PollEvent(&e)) != 0) {

			if (e.type == SDL_QUIT) {

				quit = true;
			}

		}

		SDL_RenderPresent(gRenderer);
		
	}

	close();
	return 0;
}

