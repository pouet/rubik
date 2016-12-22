#include <stdio.h>

#include "SDL.h"

#include "afficher.h"
#include "bouton.h"
#include "font.h"
#include "secure.h"

enum {
  BTN_TYPE_TXT,
  BTN_TYPE_IMG
};

Bouton * nouveauBouton(void) {
  return emalloc(sizeof(Bouton), "Erreur de création de bouton");
}

void detruireBouton(Bouton * (*b)) {
  int i;
  
  for (i = 0; i < NB_ETAT_BTN; i++) {
    if ((*b)->type == BTN_TYPE_TXT)
      SDL_FreeSurface((*b)->s[i]);
  }
  free(*b);
  *b = NULL;
}

void detruireBoutonTexte(Bouton * (*b)) {
  detruireBouton(b);
}

void detruireBoutonImage(Bouton * (*b)) {
  detruireBouton(b);
}

void fillAlpha32(SDL_Surface * sfc) {
  int i, j;
  Uint8 * p;
  
  if (!sfc)
    return;
  
  for (i = 0; i < sfc->h; i++) {
    for (j = 0; j < sfc->w; j++) {
      p = (Uint8*) sfc->pixels + i * sfc->pitch + j * 4;
      *(Uint32*)p = 0x00FFFFFF;
    }
  }
}

Bouton * creerBoutonTexte(char * nom, int x, int y, int actif) {
  Bouton * b;
  SDL_Rect r;
  int i;
  
  b = nouveauBouton();
  r.x = 12;
  
  for (i = 0; i < NB_ETAT_BTN; i++) {
    SDL_Surface * tmp = SDL_CreateRGBSurface(SDL_SWSURFACE,
                                             g_boutons[i]->w, g_boutons[i]->h,
                                             32, 0, 0, 0, 0xFF000000);
    SDL_Surface * sfc;
    
    if (!tmp)
      eprintf("Erreur de création de la surface pour les boutons.\n");
    b->s[i] = SDL_DisplayFormatAlpha(tmp);
    SDL_FreeSurface(tmp);
    if (!b->s[i])
      eprintf("Erreur de création de la surface pour les boutons.\n");
    
    blit32(g_boutons[i], NULL, b->s[i], NULL);
    sfc = creerTxtSurface(nom);
    r.y = (b->s[i]->h - sfc->h) / 2;
    r.h = sfc->h;
    r.w = sfc->w;
    if (i != BTN_ETAT_INACTIF)
      blit32(sfc, NULL, b->s[i], &r);
    else
      fillNBlitPerColor32(sfc, NULL, b->s[i], &r, 0xFFFFFF, 0xF0F0F0F0);
    
    setAlphaParCouleur32(b->s[i], 0);
  }
  b->r.x = x;
  b->r.y = y;
  b->r.h = b->s[0]->h;
  b->r.w = b->s[0]->w;
  if (!actif)
    b->etat = BTN_ETAT_INACTIF;
  else
    b->etat = BTN_ETAT_NORMAL;
  b->type = BTN_TYPE_TXT;
  b->actif = actif;
  b->majAff = 1;
  
  return b;
}

Bouton * creerBoutonImage(int image, int x, int y, int actif) {
  Bouton * b;
  int i;
  
  b = nouveauBouton();
  
  for (i = 0; i < NB_ETAT_BTN - 1; i++) {
    b->s[i] = g_imgBouton[image + i];
    b->r.x = x;
    b->r.y = y;
    b->r.h = b->s[i]->h;
    b->r.w = b->s[i]->w;
    b->etat = BTN_ETAT_NORMAL;
    b->type = BTN_TYPE_IMG;
    b->actif = actif;
    b->majAff = 1;
  }
  
  return b;
}
