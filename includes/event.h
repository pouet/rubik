#ifndef EVENT_H_INCLUDED
#define EVENT_H_INCLUDED

#include "SDL.h"

typedef struct Input Input;
struct Input {
	char key[SDLK_LAST];
	SDL_Rect mouse;
	SDL_Rect mouserel;
	char mousebuttons[8];
	int quit;
};

Input * newEvent(void);
void deleteEvent(Input * (*in));
void updateEvents(Input * in);

#endif
