#include "UtilFuncs.h"

//map one domain to another
 float UtilFuncs::mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

//float constrain function
float UtilFuncs::constrainFloat(float x, float min, float max) {
	if (x > max) return max;
	if (x < min) return min;
	return x;
}

int32_t UtilFuncs::map(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max) { //uint32_t Map function
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


int32_t UtilFuncs::constrain(int32_t x, int32_t min, int32_t max) { //uint32_t constrain function
	if (x > max) return max;
	if (x < min) return min;
	return x;
}


float UtilFuncs::absFloat(float x) {
	if (x >= 0) return x;
	if (x < 0) return -x;
	return x;
}

int32_t UtilFuncs::Rand(int32_t low, int32_t high) {
	return (rand() % (high - low)) + low; //get a random number in the range low to high. i.e (5, 10) is 5 to 9
}
