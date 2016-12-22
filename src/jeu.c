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

int jeu(Cube * cube, struct SItem * it, int nbItem) {
  Bouton ** b;
  SDL_Rect rimg[6] = {
    { POS_X_CUBISO + 3, POS_Y_CUBISO + cube->nbCubLig * OFFSET_Y_H_CUBISO +
      cube->nbCubLig * 53 + 12, 0, 0 },
    { POS_X_CUBISO + cube->nbCubLig * OFFSET_X_H_CUBISO + 22 - 6,
      POS_Y_CUBISO + cube->nbCubLig * OFFSET_Y_H_CUBISO +
      (cube->nbCubLig - 1) * OFFSET_Y_H_CUBISO + cube->nbCubLig * 53 + 12, 0, 0 },
    { POS_X_CUBISO, POS_Y_CUBISO + (cube->nbCubLig - 1) * OFFSET_Y_H_CUBISO - 15 + 10, 0, 0 },
    { POS_X_CUBISO + cube->nbCubLig * OFFSET_X_H_CUBISO + 18, POS_Y_CUBISO - 15 + 10, 0, 0 },
    { POS_X_CUBISO + cube->nbCubLig * (OFFSET_X_H_CUBISO * 2 ) + 10,
      POS_Y_CUBISO + cube->nbCubLig * OFFSET_Y_H_CUBISO + 15, 0, 0 },
    { POS_X_CUBISO - 27, POS_Y_CUBISO + cube->nbCubLig * OFFSET_Y_H_CUBISO + 15, 0, 0 }
  };
  SDL_Rect roff[6] = {
    { OFFSET_X_H_CUBISO, OFFSET_Y_H_CUBISO, 0, 0 },
    { OFFSET_X_H_CUBISO, -OFFSET_Y_H_CUBISO, 0, 0 },
    { OFFSET_X_H_CUBISO, -OFFSET_Y_H_CUBISO, 0, 0 },
    { OFFSET_X_H_CUBISO, OFFSET_Y_H_CUBISO, 0, 0 },
    { 0, 53, 0, 0 },
    { 0, 53, 0, 0 }
  };
  Input * in;
  int ret = M_MENU;
  int nbBtn;
  int done;
  int clic;
  int timer;
  int i, j, k;
  
  if (cube == NULL)
    return ret;
  
  in = newEvent();
  
  nbBtn = nbItem + cube->nbCubLig * 6;
  b = emalloc((nbBtn + 1) * sizeof **b, "Erreur d'allocation de boutons.\n");
  b[nbBtn] = NULL;
  
  for (i = 0, j = 0; i < nbItem; i++, j++)
    b[i] = creerBoutonTexte(it[i].pItemName, SCR_W - g_boutons[0]->w - 12,
                            SCR_H - 6 - (nbItem - i) * (g_boutons[0]->h + 6), 1);
  
  for (k = 0; k < 6; k++)
    for (i = 0; i < cube->nbCubLig; i++, j++)
      b[j] = creerBoutonImage(k * 3, rimg[k].x + roff[k].x * i,
                              rimg[k].y + roff[k].y * i, 1);
  
  refreshScreen();
  
  timer = 0;
  clic = 0;
  done = 0;
  while (!done) {
    updateEvents(in);
    
    if (in->quit) {
      done = 1;
      ret = M_QUIT;
    }
    
    for (i = 0; i < nbBtn; i++) {
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
            
            if (i < nbItem) {
              if (it[i].nItemVal == M_MENU || it[i].nItemVal == M_QUIT) {
                ret = it[i].nItemVal;
                if (it[i].pVal)
                  *it[i].pVal = it[i].nVal;
                done = 1;
              }
              else if (it[i].nItemVal == M_MELANGE) {
                melangerCube(cube);
                timer = 0;
                cube->tpsTotal = 0;
                cube->nbCoups = 0;
              }
              else if (it[i].nItemVal == M_REINIT) {
                refreshScreen();
                initCube(cube, cube->nbCubLig);
                timer = 0;
              }
              else if (it[i].nItemVal == M_SAUVE) {
                sauverCube(cube);
              }
            }
            else if (i >= nbItem) {
              int tmp = i - nbItem;
              
              cube->nbCoups++;
              if (timer == 0)
                timer = 1;
              
              /* Ca va crescendo on teste la borne la plus basse et ainsi de suite
               * sans retester plus bas, c'est déjà test avant */
              /* Flèche haut gauche */
              if (tmp < cube->nbCubLig) {
                rotationCubeEntier(cube, R_GAUCHE);
                mouvement(cube, tmp, R_BAS);
                rotationCubeEntier(cube, R_DROITE);
              }
              /* Flèches haut droite */
              else if (tmp < cube->nbCubLig * 2) {
                tmp -= cube->nbCubLig;
                mouvement(cube, tmp, R_BAS);
              }
              /* Flèches bas gauche */
              else if (tmp < cube->nbCubLig * 3) {
                tmp -= (cube->nbCubLig * 2);
                mouvement(cube, tmp, R_HAUT);
              }
              /* Flèches bas droite */
              else if (tmp < cube->nbCubLig * 4) {
                tmp -= (cube->nbCubLig * 3);
                rotationCubeEntier(cube, R_GAUCHE);
                mouvement(cube, tmp, R_HAUT);
                rotationCubeEntier(cube, R_DROITE);
              }
              /* Flèches droite */
              else if (tmp < cube->nbCubLig * 5) {
                tmp -= (cube->nbCubLig * 4);
                mouvement(cube, tmp, R_DROITE);
              }
              /* Flèches gauche */
              else if (tmp < cube->nbCubLig * 6) {
                tmp -= (cube->nbCubLig * 5);
                mouvement(cube, tmp, R_GAUCHE);
              }
              else {
                cube->nbCoups--;
                aprintf("Bouton inconnu.\n");
              }
            }
            else {
              aprintf("Bouton inconnu.\n");
            }
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
      for (i = 0; i < nbBtn; i++)
        if (b[i]->etat == BTN_ETAT_CLIC) {
          b[i]->etat = BTN_ETAT_NORMAL;
          b[i]->majAff = 1;
        }
    }
    
    if (timer)
      cube->tpsTotal += TICK;
    
    affiche(BCK_JEU, cube, b);
  }
  
  for (i = 0; i < nbBtn; i++)
    detruireBouton(&b[i]);
  free(b);
  deleteEvent(&in);
  
  return ret;
}
