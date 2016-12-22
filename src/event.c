#include <stdlib.h>

#include "SDL.h"

#include "event.h"
#include "secure.h"

void updateEvents(Input * in) {
	SDL_Event event;
  
  in->mousebuttons[SDL_BUTTON_WHEELUP] = 0;
  in->mousebuttons[SDL_BUTTON_WHEELDOWN] = 0;
  
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
      case SDL_KEYDOWN:
        in->key[event.key.keysym.sym] = 1;
        break;
      case SDL_KEYUP:
        in->key[event.key.keysym.sym] = 0;
        break;
      case SDL_MOUSEMOTION:
        in->mouse.x = event.motion.x;
        in->mouse.y = event.motion.y;
        in->mouserel.x = event.motion.xrel;
        in->mouserel.y = event.motion.yrel;
        break;
      case SDL_MOUSEBUTTONDOWN:
        in->mousebuttons[event.button.button] = 1;
        break;
      case SDL_MOUSEBUTTONUP:
        if (event.button.button != SDL_BUTTON_WHEELUP &&
            event.button.button != SDL_BUTTON_WHEELDOWN)
          in->mousebuttons[event.button.button] = 0;
        break;
      case SDL_QUIT:
        in->quit = 1;
        break;
      default:
        break;
		}
	}
  
  if (in->key[SDLK_ESCAPE])
    in->quit = 1;
}

Input * newEvent(void) {
  return ecalloc(1, sizeof(Input), "Création de 'event' échoué.\n");
}

void deleteEvent(Input * (*in)) {
  if (*in) {
    free(*in);
    *in = NULL;
  }
}
