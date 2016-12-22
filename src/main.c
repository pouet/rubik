#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "SDL.h"
#include "SDL_endian.h"

#include "afficher.h"
#include "cube.h"
#include "fichiers.h"
#include "font.h"
#include "jeu.h"
#include "menu.h"
#include "secure.h"

enum {
  INIT_FLAGS_SDL = SDL_INIT_VIDEO | SDL_INIT_TIMER,
  VIDEO_FLAGS_SDL = SDL_SWSURFACE
};

#if SDL_BYTEORDER != SDL_LIL_ENDIAN
#error On verra plus tard pour le big endian
#endif

int init(void) {
  srand(time(NULL));
  if (SDL_Init(INIT_FLAGS_SDL) < 0) {
    aprintf("SDL_Init: %s.\n", SDL_GetError());
    return -1;
  }
  atexit(SDL_Quit);
  if (SDL_SetVideoMode(SCR_W, SCR_H, 32, VIDEO_FLAGS_SDL) == NULL) {
    aprintf("SDL_SetVideoMode: %s.\n", SDL_GetError());
    return -1;
  }
  if (initFont() < 0) {
    aprintf("initFont: Chargement impossible.\n");
    return -1;
  }
  atexit(releaseFont);
  if (initAffiche() < 0) {
    aprintf("initAffiche: Chargement impossible.\n");
    return -1;
  }
  atexit(releaseAffiche);
  initFPS();
  
  return 0;
}

void release(void) {
  
}

int loop(void) {
  Cube * cube;
  int nbCubFace;
  int ret;
  int rep;
  
  struct SItem pMenu1[] = {
    { "Nouveau", M_NOUVEAU , NULL, 0 },
    { "Charger", M_CHARGE  , NULL, 0 },
    { "Reprise", M_REPRISE , NULL, 0 },
    { "Quitter", M_QUIT    , NULL, 0 }
  };
  int nbItemMenu1 = sizeof(pMenu1) / sizeof(pMenu1[0]);
  
  struct SItem pMenu2[] = {
    { "2x2"   , M_INIT_JEU  , &nbCubFace, 2 },
    { "3x3"   , M_INIT_JEU  , &nbCubFace, 3 },
    { "4x4"   , M_INIT_JEU  , &nbCubFace, 4 },
    { "Retour", M_MENU      , NULL      , 0 }
  };
  int nbItemMenu2 = sizeof(pMenu2) / sizeof(pMenu2[0]);
  
  struct SItem pMenuJeu[] = {
    { "Menu"          , M_MENU      , &rep, 0 },
    { "Melanger"      , M_MELANGE   , NULL, 0 },
    { "Reinitialiser" , M_REINIT    , NULL, 0 },
    { "Sauvegarder"   , M_SAUVE     , NULL, 0 },
    { "Quitter"       , M_QUIT      , NULL, 0 }
  };
  int nbItemMenuJeu = sizeof(pMenuJeu) / sizeof(pMenuJeu[0]);
  
  cube = nouveauCube();
  
  rep = 0;
  ret = M_MENU;
  while (ret != M_QUIT && ret != M_ERR) {
    switch (ret) {
      case M_INIT_JEU:
        initCube(cube, nbCubFace);
        ret = M_JEU;
        break;
      case M_CHARGE:
        if (chargerCube(cube) < 0)
          ret = M_INIT_JEU;
        else
          ret = M_JEU;
        break;
      case M_REPRISE:
        cube->majAff = 1;
        ret = M_JEU;
        break;
      case M_JEU:
        ret = jeu(cube, pMenuJeu, nbItemMenuJeu);
        rep = 1;
        break;
      case M_MENU:
        ret = menu(pMenu1, nbItemMenu1, rep);
        break;
      case M_NOUVEAU:
        ret = menu(pMenu2, nbItemMenu2, rep);
        break;
      case M_OPT:
        /* Non fait */
        break;
      case M_QUIT:
        /* Rien à faire, on quitte */
        break;
      case M_ERR:
        aprintf("Des erreurs ont fait que le programme ne peut plus"
                "s'exécuter correctement.\n");
        break;
      default:
        aprintf("Option invalide, retour au menu.\n");
        ret = M_MENU;
        break;
    }
  }
  
  detruireCube(&cube);
  
  return 0;
}

int main(int argc, char ** argv) {
  if (init() < 0)
    return EXIT_FAILURE;
  
  loop();
  
  release();
  (void) argc;
  (void) argv;
  return EXIT_SUCCESS;
}
