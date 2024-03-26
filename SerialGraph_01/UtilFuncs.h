#pragma once
#include <stdint.h>
#include <stdlib.h>
//map one domain to another
class UtilFuncs {
public:
	inline float mapFloat(float x, float in_min, float in_max, float out_min, float out_max);
	//float constrain function
	float constrainFloat(float x, float min, float max);

	int32_t map(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max);


	int32_t constrain(int32_t x, int32_t min, int32_t max);//uint32_t constrain function

	float absFloat(float x);

	int32_t Rand(int32_t low, int32_t high);
};
