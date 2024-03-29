#pragma once
typedef unsigned int mutex_t;

typedef enum {
	MUTEX_UNLOCKED = 0x00U,
	MUTEX_LOCKED = 0x01U
}mutex_state_t;
