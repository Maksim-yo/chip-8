#ifndef _PARSER
#define _PARSER

#include "utils.h"

#define START_ADDR 200
#define STACK_DEPTH 19
#define SIZE_MEM 65556
#define ERR 111
#define FONTS 16
#define PARAMS 7
#define LOCALS 7
//BIG INDIAN
// Each reg is a 8 bit but I reg is a 12 bit
//uint16_t I = 0;		// ADDRESS REGISTER

typedef struct {
	uint16_t frequency;
	uint16_t value;
} timer;



struct stack_frame{
	uint16_t params[PARAMS];
	uint16_t locals[LOCALS];
	uint16_t ret_addr;
	struct stack_frame* next;
	struct stack_frame* prev;
};

typedef struct stack_frame stack_frame;

enum KEYBOARDS_KEYS {

	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,
	KEY_10,
	KEY_11,
	KEY_12,
	KEY_13,
	KEY_14,
	KEY_15,
	KEY_16

};

typedef enum {

	V0 = 0,
	V1,
	V2,
	V3,
	V4,
	V5,
	V6,
	V7,
	V8,
	V9,
	VA,
	VB,
	VC,
	VD,
	VE,
	VF,				// FLAG
	I,
	PC,
	BP,
	R_COUNT

} REGISTERS;

const static struct {

	char val;
	const char data[10];

} fonts_data[] = {

	{'0', "F0909090F0"},
	{'1', "2060202070"},
	{'2', "F010F080F0"},
	{'3', "F010F010F0"},
	{'4', "9090F01010"},
	{'5', "F080F010F0"},
	{'6', "F080F090F0"},
	{'7', "F010204040"},
	{'8', "F090F090F0"},
	{'9', "F090F010F0"},
	{'a', "F090F09090"},
	{'b', "E090E090E0"},
	{'c', "F0808080F0"},
	{'d', "E0909090E0"},
	{'e', "F080F080F0"},
	{'f', "F080F08080"}

};
const static struct {
	REGISTERS   val;
	const char* str;
} conversion[] = {
	{V0,	"V0"},
	{V1,	"V1"},
	{V2,	"V2"},
	{V3,	"V3"},
	{V4,	"V4"},
	{V5,	"V5"},
	{V6,	"V6"},
	{V7,	"V7"},
	{V8,	"V8"},
	{V9,	"V9"},
	{VA,	"VA"},
	{VB,	"VB"},
	{VC,	"VC"},
	{VD,	"VD"},
	{VE,	"VE"},
	{VF,	"VF"}
};

void read_data(const char* path, uint8_t argc);
void parse(uint16_t instr);
void hz_timer();
void init_timers();
void reg_overflow(REGISTERS reg);
REGISTERS str_to_enum(const char* str);
void load_data(const char* path);
uint8_t check_path(uint8_t argc, const char argv[]);
void pretiffy(uint16_t val);
uint16_t check_key();
void init_mem();
void testing_paint();

//stack
void testing_stack();
stack_frame* stack_malloc(const char* text);
void stack_push();
uint16_t stack_return();

extern uint16_t data[SIZE_MEM];
extern uint16_t registers[R_COUNT];
#endif