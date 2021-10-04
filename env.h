#ifndef H_ENV
#define H_ENV

#include <stdint.h>

#define RES_SCALEDOWN 4

#define WIDTH (640/RES_SCALEDOWN)
#define HEIGHT (480/RES_SCALEDOWN)

#define WIN_WIDTH (WIDTH * RES_SCALEDOWN * 2)
#define WIN_HEIGHT (HEIGHT * RES_SCALEDOWN * 2)

#define NUM_SAMPLES 1

extern uint8_t img[WIDTH*HEIGHT*3];

void init();

void draw();

void update();

typedef enum
{
	mcUp,
	mcDown,
	mcLeft,
	mcRight,
	mcForward,
	mcBackward
} MoveDir;

void moveCamera(MoveDir d);

void moveObject(MoveDir d);

#endif