//#include "graphics.h"
#include "parser.h"
#include <SDL_image.h>
#include <SDL.h>



#pragma warning(disable : 4996)

#define GRAPHIC_ACC false
#define PIXEL_W 5
#define SDL_SCREEN_WIDTH 640
#define SDL_SCREEN_HEIGHT 320

SDL_Window* window = NULL;
SDL_Surface* surface_img = NULL;
SDL_Renderer* gRenderer = NULL;
SDL_Texture* gTexture = NULL;
SDL_Rect sprite;
SDL_Surface* gScreen = NULL;
SDL_Surface* loadedSurface = NULL;
SDL_Texture* fonts[16];
uint16_t addr = -1;
uint8_t	(*arr)[5] ;
bool quit = false;
SDL_Event e;
bool paint;
SDL_Color colors[SDL_SCREEN_WIDTH * SDL_SCREEN_HEIGHT];
char* pixels;

void paint_pixel(int x, int y);


int program_add[10] = { 0x6355 , 0x6811, 0x8384 };
int program_paint[10] = { 0x6166, 0x129, 0xd355 };
int* program = program_paint;/*{ 0x6355, 0xD144, 0xD672, 0xD114};*/

struct color {
	Uint8 r;
	Uint8 g;
	Uint8 b;

};

typedef struct color color;

Uint32 white;
Uint32 black;
Uint32 cur_color;
//color cur_color;

void close() {

	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(window);
	SDL_DestroyTexture(gTexture);

	for (int i = 0; i < FONTS; ++i) {

		SDL_DestroyTexture(fonts[i]);

	}

	gScreen = NULL;
	surface_img = NULL;
	gRenderer = NULL;
	window = NULL;
	IMG_Quit();
	SDL_Quit();

}

void paint_surf(int x, int y, int n) {

	SDL_Rect rect;
	rect.w = 40;
	rect.h = 50;
	rect.x = x * 10;
	rect.y = y * 10;
	SDL_RenderCopy(gRenderer, fonts[n]/*(SDL_Texture*)registers[I]*/, NULL, &rect);
	SDL_RenderPresent(gRenderer);
}


Uint32* getpixel(SDL_Surface* surface, int x, int y)
{
	int bpp = surface->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to retrieve */
	Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

	switch (bpp)
	{
	case 1:
		return p;
		break;

	case 2:
		return (Uint16*)p;
		break;

	case 3:
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
			return p[0] << 16 | p[1] << 8 | p[2];
		else
			return p[0] | p[1] << 8 | p[2] << 16;
		break;

	case 4:
		return (Uint32*)p;
		break;

	default:
		return 0;       /* shouldn't happen, but avoids warnings */
	}
}

//void iter_screen(SDL_Surface * img) {
//
//	int y = img->h;
//	int x = img->w;
//	
//	for (int i = 0; i < y; ++i) {
//
//		for (int j = 0; j < x; ++j) {
//
//			Uint32 data = getpixel(img, j, i);
//			SDL_Color Color;
//			SDL_GetRGB(data, img->format, &Color.r, &Color.g, &Color.b);
//			colors[i + j] = Color;
//			if ((Color.r, Color.g, Color.b) == (WHITE)) {
//
//				printf(" colors[ y = %d - x = %d] = WHITE \n", i , j);
//			}
//
//			//else if ((Color.r, Color.g, Color.b) != (0,0,0))
//				//printf(" colors[ y=%d - x=%d ] = (%d, %d, %d)\n", i, j, Color.r, Color.g, Color.b);
//
//		}
//	}
//
//}

void get_color(int x, int y) {

	SDL_Color Color;
	Uint32 data = getpixel(gScreen, x, y);
	SDL_GetRGB(data, gScreen->format, &Color.r, &Color.g, &Color.b);

	return;
}


bool check_white() {


}
void paint_pixel(int x, int y) {


	if (!((x >= 0 && x < SDL_SCREEN_WIDTH) && (y < SDL_SCREEN_HEIGHT && y >= 0)))
		return;

	bool success = false;

	Uint32* p = getpixel(gScreen, x, y);
	if (*p == white) {
		cur_color = black;
		success = true;
	}
	if (cur_color == black && !success)
		cur_color = white;

	*p = cur_color;

}

#ifdef GRAPHIC_ACC 

	//for (int i = 0; i < 5; ++i) {
	//	SDL_Color Color;
	//	SDL_GetRenderDrawColor(gRenderer, &Color.r, &Color.g, &Color.b, &Color.a);
	//	int* a;
	//	p = getpixel(gScreen, x + i + n * 5, y);
	//	a = &p;
	//	(*(&a)) = SDL_MapRGB(gScreen->format, (Uint8)(Color.r), (Uint8)(Color.g), (Uint8)(Color.b));
	//	SDL_RenderDrawPoint(gRenderer, x + i + n * 5, y);
	//}

#endif

void paint_byte(uint8_t data, uint16_t x, uint16_t y) {
	
	uint16_t col = y * SDL_SCREEN_WIDTH;
	uint16_t row = col + x * 10;
	for (int i = 0; i < 8; ++i) {
		if ((data >> 8 - 1 - i) & 1)			// if data is 1 paint else do nothing
			for (int j = 0; j < PIXEL_W; ++j)	
				paint_pixel(x + j + i * PIXEL_W, y);	
	}
}

void paint_sprite(uint8_t n,  uint16_t x, uint16_t y){


	for (int i = 0; i < n; ++i) {
		
		for (int j = 0; j < PIXEL_W; ++j) {
			paint_byte(memory[registers[I] + i], x, y + i * PIXEL_W + j);
		}
	}
}

void sprite_parse(const char* src) {
	const char* arr = src;
	for (int i = 0; i < 10; i = i + 2) {
		uint8_t num = hex_byte(arr + i);
		memory[registers[I] + i / 2] = num;
	}
}

void init_sprite() {

	for (int i = 0; i < FONTS; ++i) {

		registers[I] = i * BYTES_PER_CHAR;
		sprite_parse(fonts_data[i].data);
	}	

}

void clear_screen() {

	SDL_RenderClear(gRenderer);

}

void init_screen() {

	cur_color = white;
	for (int i = 0; i < gScreen->w; i++) {

		for (int j = 0; j < gScreen->h; ++j) {

			paint_pixel(i, j, 0);
		}
	}
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
		//SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		//gTexture = SDL_CreateTextureFromSurface(gRenderer, gScreen);
		//SDL_SetRenderTarget(gRenderer, gTexture);

		gScreen = SDL_GetWindowSurface(window);
		int imgFlags = IMG_INIT_PNG;
		//Initialize PNG loading
		if (!(IMG_Init(imgFlags) & imgFlags))
		{
			fprintf(stderr,"ERROR: SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
			return 1;
		}
	}

	return 0;
}

uint16_t instr() {

	return memory[addr++];

}

int main(int argc, char* argv[]) {

	if (init() != 0)
		exit(-1);

	init_sprite();
	
	bool success = false;
	registers[I] = 0x0;
	
	white = SDL_MapRGB(gScreen->format, 255, 255, 255);
	black = SDL_MapRGB(gScreen->format, 0, 0, 0);

	cur_color = white;

	//for (int i = 0; i < sizeof(program) / sizeof(*program); i++) {

	//	memory[registers[PC]+i] = program + i;

	//}
	//init_screen();
	while (!quit) {

		if (!success) {

			while (parse() != 0);
			success = true;
		}
			
		while ((SDL_PollEvent(&e)) != 0) {

			if (e.type == SDL_QUIT) {

				quit = true;
			}

		}

		SDL_UpdateWindowSurface(window);
		//SDL_RenderPresent(gRenderer);

	}

	close();
	return 0;
}

