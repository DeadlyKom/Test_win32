#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <stdint.h>
#include <windows.h>

#include <cmath>
#include <vector>

#define	COLOR_WHITE		RGB(0xFF, 0xFF, 0xFF)
#define COLOR_RED		RGB(0xFF, 0x00, 0x00)

#define CLASS_NAME		L"System"
#define	WINDOW_NAME		L"Test ball"

#define MIN_SIZE_BALL	5
#define MAX_SIZE_BALL	10

#define MIN_SIZE_BLOCK	5
#define MAX_SIZE_BLOCK	100

#define MIN_SPEED_BALL	1
#define MAX_SPEED_BALL	5

static int Rand(int min_value, int max_value)	{ return rand() % (max_value - min_value) + min_value; }