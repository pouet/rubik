#ifndef BOUTON_H_INCLUDED
#define BOUTON_H_INCLUDED

#include "SDL.h"

typedef enum bouton_e_ {
  BTN_ETAT_NORMAL,
  BTN_ETAT_SURVOLE,
  BTN_ETAT_CLIC,
  BTN_ETAT_INACTIF,
  NB_ETAT_BTN
} bouton_e;

typedef struct Bouton Bouton;
struct Bouton {
  SDL_Surface * s[NB_ETAT_BTN];   /* Une surface par état               */
  SDL_Rect r;                     /* Position du bouton                 */
  bouton_e etat;                  /* Différents états du bouton         */
  int type;                       /* Bouton texte, image ?              */
  int actif;                      /* Est-ce que le bouton est actif ?   */
  int majAff;                     /* Mise à jour de l'affichage ?       */
};

Bouton * nouveauBouton(void);
void detruireBouton(Bouton * (*b));
Bouton * creerBoutonTexte(char * nom, int x, int y, int actif);
Bouton * creerBoutonImage(int image, int x, int y, int actif);
void detruireBoutonTexte(Bouton * (*b));
void detruireBoutonImage(Bouton * (*b));

#endif
