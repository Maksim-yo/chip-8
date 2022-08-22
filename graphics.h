#pragma once

#ifndef GRAPH
#include <SDL.h>
#include <stdbool.h>

#define GRAPH
#define FONTS 16


SDL_Texture* load_sprite(const char* path);
void init_sprites();
void paint_surf(int x);
void paint_surf1(int x);
void clear_screen();
void close();

#endif