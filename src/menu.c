#include <stdio.h>

#include "SDL.h"
#include "SDL_image.h"

#include "afficher.h"
#include "bouton.h"
#include "collision.h"
#include "cube.h"
#include "event.h"
#include "fichiers.h"
#include "jeu.h"
#include "menu.h"
#include "secure.h"

#include "font.h"

int menu(SItem * it, int nbItem, int reprise) {
  Bouton ** b = NULL;
  Input * in;
  int ret = M_QUIT;
  int done;
  int i;
  int clic;
  
  refreshScreen();
  in = newEvent();
  b = ecalloc(nbItem + 1, sizeof *b, NULL);
  
  for (i = 0; i < nbItem; i++) {
    b[i] = creerBoutonTexte(it[i].pItemName, SCR_W - g_boutons[0]->w - 12,
                            SCR_H - 6 - (nbItem - i) * (g_boutons[0]->h + 6), 1);
    
    if (it[i].nItemVal == M_CHARGE && fichierExistant(FICH_SAV_CUBE) == 0) {
      b[i]->etat = BTN_ETAT_INACTIF;
      b[i]->actif = 0;
    }
    if (it[i].nItemVal == M_REPRISE && reprise == 0) {
      b[i]->etat = BTN_ETAT_INACTIF;
      b[i]->actif = 0;
    }
  }
  
  clic = 0;
  done = 0;
  while (!done) {
    updateEvents(in);
    
    if (in->quit) {
      done = 1;
      ret = M_QUIT;
    }
    
    for (i = 0; i < nbItem; i++) {
      if (b[i]->actif) {
        if (collisionPoint(&b[i]->r, &in->mouse)) {
          /* Si on clique */
          if (!clic && (in->mousebuttons[1] || in->mousebuttons[2])) {
            clic = 1;
            b[i]->etat = BTN_ETAT_CLIC;
            b[i]->majAff = 1;
          }
          /* Si on relâche le clic */
          else if (clic && b[i]->etat == BTN_ETAT_CLIC &&
                   !in->mousebuttons[1] && !in->mousebuttons[2]) {
            clic = 0;
            b[i]->etat = BTN_ETAT_NORMAL;
            b[i]->majAff = 1;
            
            ret = it[i].nItemVal;
            if (it[i].pVal != NULL)
              *it[i].pVal = it[i].nVal;
            done = 1;
          }
          else if (b[i]->etat != BTN_ETAT_CLIC) {
            if (b[i]->etat != BTN_ETAT_SURVOLE)
              b[i]->majAff = 1;
            b[i]->etat = BTN_ETAT_SURVOLE;
          }
        }
        else if (b[i]->etat != BTN_ETAT_CLIC) {
          if (b[i]->etat != BTN_ETAT_NORMAL)
            b[i]->majAff = 1;
          b[i]->etat = BTN_ETAT_NORMAL;
        }
      }
    }
    
    if (clic && !in->mousebuttons[1] && !in->mousebuttons[2]) {
      clic = 0;
      for (i = 0; i < nbItem; i++)
        if (b[i]->etat == BTN_ETAT_CLIC) {
          b[i]->etat = BTN_ETAT_NORMAL;
          b[i]->majAff = 1;
        }
    }
    
    affiche(BCK_MENU, NULL, b);
  }
  
  for (i = 0; i < nbItem; i++)
    detruireBoutonTexte(&b[i]);
  free(b);
  deleteEvent(&in);
  
  return ret;
}
