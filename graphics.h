#pragma once

#ifndef GRAPH
#include <SDL.h>
#include <stdbool.h>

#define GRAPH
#define FONTS 16


SDL_Texture* load_sprite(const char* path);
void init_sprites();
void paint_surf(int x, int y, int n);
void paint_sprite(uint8_t n, uint16_t x, uint16_t y);
void clear_screen();
void close();

#endif