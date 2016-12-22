#ifndef AFFICHER_H_INCLUDED
#define AFFICHER_H_INCLUDED

#include "bouton.h"
#include "cube.h"

#include "SDL.h"

enum {
  SCR_W   = 800,
  SCR_H   = 600
};

enum {
  FPS = 50,
  TICK = 1000 / FPS
};

enum {
  POS_X_CUBISO = 120,
  POS_Y_CUBISO = 100,
  OFFSET_X_H_CUBISO = 43,
  OFFSET_Y_H_CUBISO = 22,
  OFFSET_Y_COTE_CUBISO = 16
};

enum {
  BCK_MENU,
  BCK_JEU
};

enum {
  IMG_BTN_FLCH_B_G = 0,
  IMG_BTN_FLCH_B_D = 3,
  IMG_BTN_FLCH_H_A = 6,
  IMG_BTN_FLCH_H_D = 9,
  IMG_BTN_FLCH_D = 12,
  IMG_BTN_FLCH_G = 15,
  NB_IMG_BTN = 18
};

SDL_Surface * g_imgBouton[NB_IMG_BTN];
SDL_Surface * g_boutons[4];

int initAffiche(void);
void releaseAffiche(void);
void initFPS(void);
void setFPS(int fps);
void affiche(int NBckgrnd, Cube * cube, Bouton ** btn);
SDL_Surface * chargerImage32(char * s);
void setAlphaParCouleur32(SDL_Surface * sfc, Uint32 coul);
void blit32(SDL_Surface * src, SDL_Rect const * srcRec,
            SDL_Surface * dst, SDL_Rect const * dstRec);
void fillNBlitPerColor32(SDL_Surface * src, SDL_Rect const * srcRec,
                         SDL_Surface * dst, SDL_Rect const * dstRec,
                         Uint32 const srcCoul, Uint32 const dstCoul);
void refreshScreen(void);

#endif
