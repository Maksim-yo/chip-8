#include "utils.h"


uint16_t random_gen() {

	time_t result = time(NULL);

	if (result == (time_t)(-1))
		putc("error with time ", stdout);

	int16_t rand = (result * 841834) / (result % 1414);
	return rand;

}


uint16_t little_to_big(uint16_t val) {

	return val >> 8 | val << 8;

}

uint16_t xor (uint16_t val1, uint16_t val2) {

	return (~val1 & val2) || (val1 & ~val2);

}






