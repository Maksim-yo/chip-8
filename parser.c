/* INSTRUCTIONS
	0NNN	Execute machine language subroutine at address NNN
	00E0	Clear the screen
	00EE	Return from a subroutine
	1NNN	Jump to address NNN
	2NNN	Execute subroutine starting at address NNN
	3XNN	Skip the following instruction if the value of register VX equals NN
	4XNN	Skip the following instruction if the value of register VX is not equal to NN
	5XY0	Skip the following instruction if the value of register VX is equal to the value of register VY
	6XNN	Store number NN in register VX
	7XNN	Add the value NN to register VX
	8XY0	Store the value of register VY in register VX
	8XY1	Set VX to VX OR VY
	8XY2	Set VX to VX AND VY
	8XY3	Set VX to VX XOR VY

	8XY4	Add the value of register VY to register VX
			Set VF to 01 if a carry occurs
			Set VF to 00 if a carry does not

	8XY5	Subtract the value of register VY from register VX
			Set VF to 00 if a borrow occurs
			Set VF to 01 if a borrow does not occur

	8XY6	Store the value of register VY shifted right one bit in register VX?
			Set register VF to the least significant bit prior to the shift
			VY is unchanged

	8XY7	Set register VX to the value of VY minus VX
			Set VF to 00 if a borrow occurs
			Set VF to 01 if a borrow does not occur

	8XYE	Store the value of register VY shifted left one bit in register VX?
			Set register VF to the most significant bit prior to the shift
			VY is unchanged

	9XY0	Skip the following instruction if the value of register VX is not equal to the value of register VY
	ANNN	Store memory address NNN in register I
	BNNN	Jump to address NNN + V0
	CXNN	Set VX to a random number with a mask of NN

	DXYN	Draw a sprite at position VX, VY with N bytes of sprite data starting at the address stored in I
			Set VF to 01 if any set pixels are changed to unset, and 00 otherwise

	EX9E	Skip the following instruction if the key corresponding to the hex value currently stored in register VX is pressed
	EXA1	Skip the following instruction if the key corresponding to the hex value currently stored in register VX is not pressed
	FX07	Store the current value of the delay timer in register VX
	FX0A	Wait for a keypress and store the result in register VX
	FX15	Set the delay timer to the value of register VX
	FX18	Set the sound timer to the value of register VX
	FX1E	Add the value stored in register VX to register I
	FX29	Set I to the memory address of the sprite data corresponding to the hexadecimal digit stored in register VX
	FX33	Store the binary-coded decimal equivalent of the value stored in register VX at addresses I, I + 1, and I + 2

	FX55	Store the values of registers V0 to VX inclusive in memory starting at address I
			I is set to I + X + 1 after operation?

	FX65	Fill registers V0 to VX inclusive with the values stored in memory starting at address I
			I is set to I + X + 1 after operation?

*/


#include "parser.h"
#include <SDL_thread.h>
#include "graphics.h"
/*
TODO :
	GRAPHICS
	KEYPAD INPUT
	TIMERS :
		SOUND TIMER
		DELAY TIMER

*/

// REGISTERS
	


timer* del_timer = NULL;
timer* snd_timer = NULL;
HANDLE hStdin = INVALID_HANDLE_VALUE;
uint16_t registers[R_COUNT];
DWORD fdwMode, fdwOldMode;
uint16_t keys[16] = { '0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f' };
uint16_t memory[SIZE_MEM];
uint16_t data[SIZE_MEM];
stack_frame* head = NULL;
stack_frame* cur_stack = NULL;
SDL_Texture* fonts[16];
char cur_instr[7];

void stack_init(stack_frame* frame) {

	for (int i = 0; i < PARAMS; ++i) {

		frame->params[i] = NULL;

	}

	for (int i = 0; i < LOCALS; ++i) {

		frame->locals[i] = NULL;

	}

	frame->next = NULL;
	frame->prev = NULL;
	frame->ret_addr = NULL;

}



stack_frame* stack_malloc(const char* text) {

	stack_frame* temp = malloc(sizeof(stack_frame));
	stack_init(temp);

	if (temp == 0) {

		fprintf(stderr, "ERROR: %s couldn't do push op", text);
		exit(-1);
	}
	return temp;

}

void stack_push(){
	
	stack_frame* temp ;
	stack_frame* tmp;

	if (head == NULL) {

		head = stack_malloc("head");
		
		if (head == 0) {

			fprintf(stderr, "ERROR: stack_push couldn't do push op");
			exit(-1);
		}
		
		temp = head;
		cur_stack = temp;
	}

	else {

		temp = head;

		while (temp->next != NULL){
			
			//tmp = temp;
			temp = temp->next;
			//temp->prev = tmp;

		}

		// NOW temp->next eq to null, so allocate memory for new stack frame
		temp->next = stack_malloc("next");
		temp->next->prev = stack_malloc("prev"); // maybe redundant mem
		temp->next->prev = temp;
		cur_stack = temp->next;
		printf("frame was created successfuly");

	}

}

uint16_t stack_return() {

	uint16_t addr = cur_stack->ret_addr;

	if (cur_stack->prev == NULL) {

		return addr;

	}

	stack_frame* frame = cur_stack->prev;
	free(cur_stack);
	cur_stack = frame;
	return addr;

}

void testing_stack() {

	stack_push();
	stack_push();
	stack_push();
	stack_return();
	stack_return();
	stack_return();
	
}

void init_mem() {

	for (int i = 0;i < SIZE_MEM; ++i) 

		memory[i] = 0000;

}

uint16_t check_key() {

	return WaitForSingleObject(hStdin, 1000) == WAIT_OBJECT_0 && _kbhit();

}

void pretiffy(uint16_t val) {

	HANDLE buff = GetStdHandle(STD_OUTPUT_HANDLE);

}

uint8_t check_path(uint8_t argc, const char* path) {

	FILE* file;	
	if (argc >= 2) {

		if (file = fopen(path, "r")) {

			fclose(file);
			return 1;

		}

	}
		return -1;

}

void load_data(const char* path) {

	FILE* fp = fopen(path, "rb");
	uint16_t* p = memory;
	fread(p, sizeof(uint16_t), SIZE_MEM, fp);

	if (feof(fp))

		printf("File was loaded successufly.\n");

	else {

		fprintf(stderr, "ERRPR: File read error.\n");
		exit(-1);
	}
	fclose(fp);

}

void read_data(const char* path, uint8_t argc) {

	// NEEDED CHANGE

	for (int i = 0; i < SIZE_MEM; ++i) {

		memory[i] = 0000;

	}
	
	if (check_path(argc, path) == -1) {

		fprintf(stderr, "ERROR: File couldn'n read\n\t::: main.c [filepath]");
		exit(-1);
	
	}
	
	load_data(path);

}


void fetch_byte(uint16_t x) {

	const char* arr = fonts_data[x].data;
	for (int i = 0; i < 10; i = i + 2) {

		uint8_t num = hex_byte(arr + i);
		memory[registers[I] + i / 2] = num;
	}

}

REGISTERS str_to_enum(uint16_t num) {

	char str[4];
	unsigned int len = sprintf(str, "V%x", num);
	str[1] = toupper(str[1]);

	if (str == NULL) {

		fprintf(stderr, "ERROR: str_to_enum func get incorret args");
		exit(-1);
	}

	for (int8_t i = 0; i < sizeof(conversion) / sizeof(conversion[0]);++i) {

		if (strcmp(str, conversion[i].str) == 0) 

			return conversion[i].val;

	}

	return ERR;
}

void reg_overflow(REGISTERS reg) {

	registers[reg] = registers[reg] % 256;

}


void malloc_timers(timer* tmr, const char* str) {

	tmr = malloc(sizeof(timer));

	if (!(tmr)) {

		fprintf(stderr, "ERROR: %s couldn't allocate with malloc", str);
		exit(-1);
	}

}

void init_timers() {

	malloc_timers(del_timer, "delay_timer");
	malloc_timers(snd_timer, "sound_timer");
	(*del_timer).frequency = 60;
	(*snd_timer).frequency = 60;
}

void hz_timer() {

	printf("Counting value in a delay timer:\n");
	uint8_t c = 0;
	while (del_timer->value != 0) {

		time_t start = time(NULL);

		for (int i = 0; i < 60; ++i) {
			

			if (del_timer->value != 0) {
				del_timer->value -= 1;
			}

			else {

				break;
			}

		}

		time_t end = time(NULL);

		while (difftime(end, start) < 1) {
			end = time(NULL);
		}

	}

}


uint16_t parse() {

	 uint16_t instr = memory[registers[PC]];
	 //paint_sprite(5, 100, 100);

	 parser(instr);
	 registers[PC]++;
	 return 0;	

}

void parser(uint16_t instr) {

	sprintf(cur_instr, "%x", instr); // instr in hex

	if (hex(cur_instr)) {

			uint8_t len = strlen(cur_instr);
			memset(cur_instr, '0', 7);
			sprintf(cur_instr, "0x%0*x%x", 4-len, 0, instr); // complete for 4 symbols cause all instr 4 byte
	
	}

	bool flg_z = false;
	uint16_t f_byte = (instr >> 12) & 0xF;
	uint16_t reg = (instr >> 8) & 0xF;
	REGISTERS x = str_to_enum((uint16_t)reg);
	reg = (instr >> 4) & 0xF;
	REGISTERS y = str_to_enum(reg);

	switch (instr) {

	case 0x00e0:				// CLEAR THE SCREEEN 
	{

		clear_screen();
		flg_z = true;
	}
		break;

	case 0x00ee:				// RETURN FROM SUBROUTINE 
	{
		uint16_t addr = cur_stack->ret_addr;

		if (cur_stack->prev)
			cur_stack = cur_stack->prev;

		else {

			cur_stack = NULL;
		}

		registers[PC] = addr;
		flg_z = true;

	}
		break;

	}

	if (!flg_z & cur_instr[2] == '0') {

		uint16_t addr = instr & 0xFFF;
		stack_push();
		cur_stack->ret_addr = registers[PC];
		registers[PC] = addr;

	}

	switch (f_byte) {	

	case 1:						// JUMP TO ADDRESS NNN 
	{

		uint16_t mem = instr & 0xFFF;
		registers[PC] = mem;

	}
		break;

	case 2: {

		uint16_t mem = instr & 0xFFF;
		//registers[BP] = registers[PC];
		stack_push();
		cur_stack->ret_addr = registers[PC];
		registers[PC] = mem;

	}
		break;
	case 3: {

		uint8_t val = instr & 0xFF;

		registers[PC] = (registers[x] == val) ? (registers[PC] + 1 ): (registers[PC]);


	}
		break;
	case 4: {

		uint8_t val = instr & 0xFF;

		registers[PC] = (registers[x] != val) ? (registers[PC] + 1) : (registers[PC]);

	}
		break;
	case 5: {

		registers[PC] = (registers[x] == registers[y]) ? (registers[PC] + 1) : (registers[PC]);

	}
		break;
	case 6: {

		uint8_t val = instr & 0xFF;
		registers[x] = val;

	}
		break;
	case 7: {

		uint8_t val = instr & 0xFF;
		registers[x] += val;

	}
		break;

	case 8: {

		uint8_t l_4b = instr & 0xF;

		// ERR = 111, BUT X == ERR DOESNT WORK IDK;

		if (x == 111 || y == 111) 

			putc("Error with str to enum func ", stdout);


		switch (l_4b) {

		case 0x0: {

			registers[x] = (uint8_t)registers[y];

		}
			break;
		case 0x1:

		{

			registers[x] = registers[y] | registers[x];

		}
			break;
		case 0x2: {

			registers[x] = registers[y] & registers[x];

		}
			break;
		case 0x3: {

			registers[x] = xor (registers[y], registers[x]);

		}

			break;
		case 0x4: {

			uint16_t temp = registers[x];
			registers[x] += registers[y];
			reg_overflow(x);
			// CHECKING FOR A CARRY BIT
			// NEED TO CHANGE 
			registers[VF] = (registers[x] >> 8) & 1;

		}
			break;
		case 0x5: {

			registers[x] -= registers[y];
			registers[VF] = !(registers[y] > registers[x]); // (registers[x] >> 8) & 1

		}
			break;
		case 0x6: {

			registers[VF] = registers[y] & 0x1;
			registers[x] = registers[y] >> 1;

		}
			break;
		case 0x7: {

			registers[x] = registers[y] - registers[x];
			registers[VF] = !(registers[x] > registers[y]);

		}
			break;
		case 0xe: {

			registers[VF] = (registers[y] >> 7) & 1;
			registers[x] = registers[y] << 1;


		}
			break;
		default:
			break;
			}

		}

	case 9: {

		registers[PC] = (registers[x] != registers[y]) ? (registers[PC] + 1) : (registers[PC]);

	}
		break;

	case 0xa: {

		uint16_t mem_val = instr & 0xFFF;
		registers[I] = mem_val;

	}
		break;
	case 0xb: {

		uint16_t mem = instr & 0xFFF;
		registers[PC] = mem + registers[V0];

	}
		break;
	case 0xc: {

		uint8_t mask = instr & 0xFF;

		registers[x] = random_gen() & mask;

	}
		break;
	case 0xd: {

		uint8_t n = instr & 0xF;
		paint_sprite(n, x, y);

	}
		break;
	case 0xe: {

		uint8_t l_byte = instr & 0xFF;

		switch (l_byte) {

		case 0x9e: {


			if (check_key()) {

				registers[PC] += getchar() == registers[x];
			}



		}
			break;
		case 0xa1: {

			if (check_key()) {

				registers[PC] += getchar() != registers[x];

			}

		}
			break;

		default:
			break;
			
			}
		}

	case 0xf: {

		uint8_t l_byte = instr & 0xFF;

		switch (l_byte) {

		case 0x07: {

			registers[x] = (*del_timer).value;
		}
			break;
		case 0x0a: {


			if (check_key()) {

				registers[x] = getchar();

			}

		}
			break;
		case 0x15: {

			(*del_timer).value = registers[x];
		}
			break;
		case 0x18: {

			(*snd_timer).value = registers[x];

		}
			break;
		case 0x1e: {
			// I has 12 bit so no checking
			registers[I] += registers[x];
		}
			break;
		case 0x29: {


			for (int i = 0; i < FONTS; ++i) {

				if (asci_hex(registers[x]) == asci_hex(fonts_data[i].val)) {

					fetch_byte(i);

				}

			}

		}
			break;
		case 0x33: {

			memory[registers[I]] = (registers[x] % 1000) / 100;
			memory[registers[I]+1] = (registers[x] % 100) / 10;
			memory[registers[I] + 2] = (registers[x]) % 10;


		}
			break;
		case 0x55: {

			uint8_t range = x + 1 ;


			for (uint8_t i = 0; i < range; ++i) {

				memory[registers[I]] = registers[i];
				registers[I]++;

			}
				registers[I] += x + 1;

		}
			break;
		case 0x65: {

			uint8_t range = x + 1;

			for (uint8_t i = 0; i < range; i++) {

				registers[i] = memory[registers[I]];

			}
				registers[I] += x + 1;

		}
			break;
		default:

			fprintf(stderr, "No such instruction: 0x%x\n", instr);
			break;

			}
		}
	}

}
