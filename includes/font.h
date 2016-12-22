#ifndef FONT_H_INCLUDED
#define FONT_H_INCLUDED

int initFont(void);
void releaseFont(void);
SDL_Surface * creerTxtSurface(char * s);

#endif
