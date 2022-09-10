#ifndef _LIBS
#define _LIBS
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <conio.h>
#include <Windows.h>
#include <time.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_image.h>
#include <stdbool.h>




uint16_t xor (uint16_t val1, uint16_t val2);
uint16_t little_to_big(uint16_t val);
uint16_t random_gen();
uint16_t convert_to_hex(char num);
bool hex(char* str);
uint8_t asci_hex(unsigned char num);
void testing();
uint8_t hex_byte(char* byte);
void sprite_parse(const char* src);
#endif