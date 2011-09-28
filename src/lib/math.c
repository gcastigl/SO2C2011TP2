#include "../../include/lib/math.h"

long seed = 213131;

int pow(int base, int power) {
	if (0 == power) {
		return 1;
	}
	return base * pow(base, power - 1);
}

int random() {
	seed = (7717*seed + 6007)%101;
	return abs(seed);
}

void setSeed(long s) {
	seed = s;
}

int abs(int n) {
	return n >= 0 ? n : -n;
}

