#include <string.h>

#include "SDL.h"
#include "SDL_image.h"

#include "afficher.h"
#include "bouton.h"
#include "cube.h"
#include "secure.h"

#include "font.h"

#define FICH_PETIT_CUBE   "pcube_coul.png"
#define FICH_FOND_PCUBE   "pcube_fond.png"
#define FICH_CUBE3D_FOND  "cube3D_fond.png"
#define FICH_CUBE3D_COUL  "cube3D_coul.png"
#define FICH_RUBIK_FOND   "rubik_fond.png"
#define FICH_RUBIK_TITRE  "rubik_titre.png"
#define NB_FOND           2

enum {
  NB_ALLOC = 10,
  OFFSET_FLCH = 14,
  NB_FLCH = 6
};

static int g_fps = FPS;
static int g_tick = TICK;
static Uint32 g_nxtTime = TICK;
static SDL_Surface * g_bckgrnd[NB_FOND];

static int g_nbSprites;
static SDL_Surface ** g_sprites;
static SDL_Surface * g_cadre;

static int g_oldBckgrnd = -1;

unsigned int g_couleurs[NB_COUL] = {
  0xFFD8D800, 0xFF50D250, 0xFFD28030, 0xFF5C74D3, 0xFFE0E0E0, 0xFFD33C44
};


void setFPS(int fps) {
  if (fps <= 0)
    fps = FPS;
  g_fps = fps;
  g_tick = 1000 / FPS;
}

void initFPS(void) {
  g_nxtTime = SDL_GetTicks() + TICK;
}

static Uint32 timeLeft(void) {
  Uint32 now;
  
  now = SDL_GetTicks();
  if(g_nxtTime <= now)
    return 0;
  else
    return g_nxtTime - now;
}

SDL_Surface * chargerImage32(char * s) {
  SDL_Surface * sfc = IMG_Load(s);
  SDL_Surface * tmp;
  
  if (!sfc)
    eprintf("Ouverture de l'image '%s' impossible.\n", s);
  
  tmp = SDL_DisplayFormatAlpha(sfc);
  SDL_FreeSurface(sfc);
  
  return tmp;
}

void setAlphaParCouleur32(SDL_Surface * sfc, Uint32 coul) {
  int i, j;
  Uint8 * p;
  
  if (!sfc)
    return;
  
  for (i = 0; i < sfc->h; i++) {
    for (j = 0; j < sfc->w; j++) {
      p = (Uint8*) sfc->pixels + i * sfc->pitch + j * 4;
      if ((*(Uint32*)p & 0x00FFFFFF) == (coul & 0x00FFFFFF))
        *(Uint32*)p = 0x00FFFFFF;
    }
  }
}

void affecterImgBtn(void) {
  int i, j;
  
  for (i = 0, j = OFFSET_FLCH; j < OFFSET_FLCH + NB_FLCH; i += 3, j++) {
    g_imgBouton[i] = g_sprites[j];
    g_imgBouton[i+1] = g_sprites[j + NB_FLCH];
    g_imgBouton[i+2] = g_sprites[j + 2 * NB_FLCH];
  }
}

int chargerTileset(char * s) {
  SDL_Surface * sfc;
  int ret;
  int i, j;
  int ix, iy;
  int h, w;
  int nbSprite;
  int nAlloc;
  Uint8 * p8;
  
  sfc = chargerImage32(s);
  nAlloc = NB_ALLOC;
  g_sprites = emalloc(nAlloc * sizeof *g_sprites,
                      "Erreur d'allocation de g_sprites.\n");
  
  nbSprite = 0;
  ret = 0;
  
  for (iy = 0; iy < sfc->h; iy++) {
    for (ix = 0; ix < sfc->w; ix++) {
      p8 = (Uint8 *) sfc->pixels + iy * sfc->pitch + ix * 4;
      
      if (((*(Uint32*)p8) & 0xFFFFFF) != 0xFF) {
        SDL_Surface * tmp;
        Uint32 * pixTmp;
        
        /* Récupération de la taille du tile */
        p8 = (Uint8 *) sfc->pixels + iy * sfc->pitch + ix * 4;
        w = 0;
        while (((*(Uint32*)p8) & 0xFFFFFF) != 0xFF) {
          w++;
          p8 += 4;
        }
        
        p8 = (Uint8 *) sfc->pixels + iy * sfc->pitch + ix * 4;
        h = 0;
        while (((*(Uint32*)p8) & 0xFFFFFF) != 0xFF) {
          h++;
          p8 += sfc->pitch;
        }
        tmp = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 32, 0, 0, 0, 0xFF000000);
        if (!tmp) {
          aprintf("Erreur de création de la surface pour le tilset.\n");
          ret = -1;
          goto end;
        }
        
        pixTmp = tmp->pixels;
        /* On copie la surface dans une nouvelle et on supprime l'ancienne */
        for (i = 0; i < h; i++) {
          for (j = 0; j < w; j++) {
            p8 = (Uint8 *) sfc->pixels + (i + iy) * sfc->pitch + (j + ix) * 4;
            *pixTmp = * (Uint32*) p8;
            * (Uint32*) p8 = 0xFF0000FF;
            pixTmp++;
          }
        }
        
        setAlphaParCouleur32(tmp, 0x00FF00FF);
        
        if (nbSprite >= nAlloc) {
          nAlloc *= 2;
          g_sprites = erealloc(g_sprites, nAlloc * sizeof *g_sprites,
                               "Erreur de réallocation mémoire pour g_sprites.\n");
        }
        g_sprites[nbSprite] = tmp;
        
        nbSprite++;
      }
    }
  }
  g_nbSprites = nbSprite;
  SDL_FreeSurface(sfc);
  
  sfc = chargerImage32("boutons.png");
  
  nbSprite = 0;
  ret = 0;
  
  for (iy = 0; iy < sfc->h; iy++) {
    for (ix = 0; ix < sfc->w; ix++) {
      p8 = (Uint8 *) sfc->pixels + iy * sfc->pitch + ix * 4;
      
      if (((*(Uint32*)p8) & 0xFFFFFF) != 0xFF) {
        SDL_Surface * tmp;
        Uint32 * pixTmp;
        
        /* Récupération de la taille du tile */
        p8 = (Uint8 *) sfc->pixels + iy * sfc->pitch + ix * 4;
        w = 0;
        while (((*(Uint32*)p8) & 0xFFFFFF) != 0xFF) {
          w++;
          p8 += 4;
        }
        
        p8 = (Uint8 *) sfc->pixels + iy * sfc->pitch + ix * 4;
        h = 0;
        while (((*(Uint32*)p8) & 0xFFFFFF) != 0xFF) {
          h++;
          p8 += sfc->pitch;
        }
        tmp = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 32, 0, 0, 0, 0xFF000000);
        if (!tmp) {
          aprintf("Erreur de création de la surface pour le tilset.\n");
          ret = -1;
          goto end;
        }
        
        pixTmp = tmp->pixels;
        /* On copie la surface dans une nouvelle et on supprime l'ancienne */
        for (i = 0; i < h; i++) {
          for (j = 0; j < w; j++) {
            p8 = (Uint8 *) sfc->pixels + (i + iy) * sfc->pitch + (j + ix) * 4;
            *pixTmp = * (Uint32*) p8;
            * (Uint32*) p8 = 0xFF0000FF;
            pixTmp++;
          }
        }
        
        setAlphaParCouleur32(tmp, 0x00FF00FF);
        
        g_boutons[nbSprite] = tmp;
        
        nbSprite++;
      }
    }
  }
  g_nbSprites = nbSprite;
  SDL_FreeSurface(sfc);
  
  sfc = chargerImage32("cadre.png");
  nbSprite = 0;
  ret = 0;
  
  for (iy = 0; iy < sfc->h; iy++) {
    for (ix = 0; ix < sfc->w; ix++) {
      p8 = (Uint8 *) sfc->pixels + iy * sfc->pitch + ix * 4;
      
      if (((*(Uint32*)p8) & 0xFFFFFF) != 0xFF) {
        SDL_Surface * tmp;
        Uint32 * pixTmp;
        
        /* Récupération de la taille du tile */
        p8 = (Uint8 *) sfc->pixels + iy * sfc->pitch + ix * 4;
        w = 0;
        while (((*(Uint32*)p8) & 0xFFFFFF) != 0xFF) {
          w++;
          p8 += 4;
        }
        
        p8 = (Uint8 *) sfc->pixels + iy * sfc->pitch + ix * 4;
        h = 0;
        while (((*(Uint32*)p8) & 0xFFFFFF) != 0xFF) {
          h++;
          p8 += sfc->pitch;
        }
        tmp = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 32, 0, 0, 0, 0xFF000000);
        if (!tmp) {
          aprintf("Erreur de création de la surface pour le tilset.\n");
          ret = -1;
          goto end;
        }
        
        pixTmp = tmp->pixels;
        /* On copie la surface dans une nouvelle et on supprime l'ancienne */
        for (i = 0; i < h; i++) {
          for (j = 0; j < w; j++) {
            p8 = (Uint8 *) sfc->pixels + (i + iy) * sfc->pitch + (j + ix) * 4;
            *pixTmp = * (Uint32*) p8;
            * (Uint32*) p8 = 0xFF0000FF;
            pixTmp++;
          }
        }
        
        setAlphaParCouleur32(tmp, 0x00FF00FF);
        setAlphaParCouleur32(tmp, 0);
        
        g_cadre = tmp;
        
        nbSprite++;
      }
    }
  }
  g_nbSprites = nbSprite;
  SDL_FreeSurface(sfc);
  
end:
  return ret;
}

int initAffiche(void) {
  int ret = -1;
  
  g_bckgrnd[0] = chargerImage32(FICH_RUBIK_FOND);
  g_bckgrnd[1] = chargerImage32(FICH_RUBIK_TITRE);
  if (chargerTileset("tileset.png") < 0)
    goto end;
  affecterImgBtn();
  
  ret = 0;
end:
  return ret;
}

void releaseAffiche(void) {
  int i;
  
  for (i = 0; i < NB_FOND; i++)
    SDL_FreeSurface(g_bckgrnd[i]);
  
  /* TODO: Rajouter les free des surfaces */
}

/* flag = 0 -> On blitte la surface src dans dst
 * flag = 1 -> On blitte la surface src dans dst en coloriant avec la couleur
 *              'srcCoul'.
 * flag = 2 -> On blitte la surface src dans dst en coloriant la couleur
 *              'srcCoul' de la surface 'src' avec la couleur 'dstCoul'.
 */
void fillNBlitWithFlag32(SDL_Surface * src, SDL_Rect const * srcRec,
                         SDL_Surface * dst, SDL_Rect const * dstRec,
                         Uint32 const srcCoul, Uint32 const dstCoul,
                         int flag) {
  SDL_Rect recSrc, recDst;
  int ix, iy;
  int jx, jy;
  int ixmax, iymax;
  int jxmax, jymax;
  
  if (src == NULL || dst == NULL) {
    return;
  }
  if (src->format->BytesPerPixel != 4 || dst->format->BytesPerPixel != 4) {
    return;
  }
  
  if (srcRec == NULL) {
    recSrc.x = 0;
    recSrc.y = 0;
    recSrc.h = src->h;
    recSrc.w = src->w;
  }
  else {
    recSrc = *srcRec;
  }
  if (dstRec == NULL) {
    recDst.x = 0;
    recDst.y = 0;
    recDst.h = dst->h;
    recDst.w = dst->w;
  }
  else {
    recDst = *dstRec;
  }
  
  ixmax = recSrc.x + recSrc.w;
  iymax = recSrc.y + recSrc.h;
  jxmax = recDst.x + recDst.w;
  jymax = recDst.y + recDst.h;
  
#define ALIGN(n, min, max) ((n) < (min) ? (min) : (n) >= (max) ? (max) : (n))
  ixmax = ALIGN(ixmax, 0, SCR_W-1);
  iymax = ALIGN(iymax, 0, SCR_H-1);
  jxmax = ALIGN(jxmax, 0, SCR_W-1);
  jymax = ALIGN(jymax, 0, SCR_H-1);
#undef ALIGN
  
  for (iy = recSrc.y, jy = recDst.y;
       iy >= 0 && iy < SCR_H && iy < iymax &&
       jy >= 0 && jy < SCR_H && jy < jymax; iy++, jy++) {
    Uint8 * p1 = (Uint8 *) dst->pixels + jy * dst->pitch + recDst.x * 4;
    Uint8 * p2 = (Uint8 *) src->pixels + iy * src->pitch + recSrc.x * 4;
    
    for (ix = recSrc.x, jx = recDst.x;
         ix >= 0 && ix < SCR_W && ix < ixmax &&
         jx >= 0 && jx < SCR_W && jx < jxmax; ix++, jx++) {
      Uint8 a = p2[3];
      
      if (flag == 0) {
        /* Surface complètement opaque */
        if (a == 0xFF) {
          p1[0] = p2[0];
          p1[1] = p2[1];
          p1[2] = p2[2];
        }
        /* Surface avec canal alpha > 0.
         * Alpha = 0 -> transparent, on ne calcule pas */
        else if (a != 0) {
          p1[0] = (p2[0] * a + p1[0] * (255 - a)) / 255;
          p1[1] = (p2[1] * a + p1[1] * (255 - a)) / 255;
          p1[2] = (p2[2] * a + p1[2] * (255 - a)) / 255;
        }
      }
      else if (flag == 1) {
        if (a == 0xFF)
          * (Uint32*) p1 = srcCoul;
        else if (a != 0) {
          p1[0] = (srcCoul * a + p1[0] * (255 - a)) / 255;
          p1[1] = (srcCoul * a + p1[1] * (255 - a)) / 255;
          p1[2] = (srcCoul * a + p1[2] * (255 - a)) / 255;
        }
      }
      else if (flag == 2) {
        Uint32 c;
        
        if ((srcCoul << 8) == ((* (Uint32*) p2) << 8))
          c = dstCoul;
        else
          c = * (Uint32*) p2;
        
        if (a == 0xFF)
          * (Uint32*) p1 = c;
        else if (a != 0) {
          p1[0] = (c * a + p1[0] * (255 - a)) / 255;
          p1[1] = (c * a + p1[1] * (255 - a)) / 255;
          p1[2] = (c * a + p1[2] * (255 - a)) / 255;
        }
      }
      
      p1 += 4;
      p2 += 4;
    }
  }
}

void blit32(SDL_Surface * src, SDL_Rect const * srcRec,
            SDL_Surface * dst, SDL_Rect const * dstRec) {
  fillNBlitWithFlag32(src, srcRec, dst, dstRec, 0, 0, 0);
}

/* Fait un blit en coloriant la surface 'src' avec la couleur 'coul' en
 * laissant le canal alpha tel quel */
void fillNBlit32(SDL_Surface * src, SDL_Rect const * srcRec,
                 SDL_Surface * dst, SDL_Rect const * dstRec,
                 Uint32 const coul) {
  fillNBlitWithFlag32(src, srcRec, dst, dstRec, coul, 0, 1);
}

/* Fait un blit en coloriant la couleur de la surface 'src' avec la
 * couleur 'coul' en laissant le canal alpha tel quel */
void fillNBlitPerColor32(SDL_Surface * src, SDL_Rect const * srcRec,
                         SDL_Surface * dst, SDL_Rect const * dstRec,
                         Uint32 const srcCoul, Uint32 const dstCoul) {
  fillNBlitWithFlag32(src, srcRec, dst, dstRec, srcCoul, dstCoul, 2);
}

void afficheCoinsCubeIso(Cube * cube) {
  static SDL_Surface * scr;
  SDL_Rect r, s;
  int i, j;
  int nbCubLig;
  
  if (!scr)
    scr = SDL_GetVideoSurface();
  nbCubLig = cube->nbCubLig;
  
  /* Place les 3 coins (gauche, droite et bas) */
  r.w = g_sprites[3]->w;
  r.h = g_sprites[3]->h;
  r.x = POS_X_CUBISO - 3;
  r.y = POS_Y_CUBISO + nbCubLig * OFFSET_Y_H_CUBISO;
  blit32(g_sprites[3], NULL, scr, &r);
  
  r.w = g_sprites[4]->w;
  r.h = g_sprites[4]->h;
  r.x = POS_X_CUBISO + nbCubLig * g_sprites[2]->w + 4 * (nbCubLig - 1) - 2;
  r.y = POS_Y_CUBISO + nbCubLig * OFFSET_Y_H_CUBISO;
  blit32(g_sprites[4], NULL, scr, &r);
  
  r.w = g_sprites[5]->w;
  r.h = g_sprites[5]->h;
  r.x = POS_X_CUBISO + nbCubLig * OFFSET_X_H_CUBISO - 6;
  r.y = (POS_Y_CUBISO + nbCubLig * OFFSET_Y_H_CUBISO +
         OFFSET_Y_H_CUBISO * nbCubLig +
         nbCubLig * (g_sprites[0]->h - OFFSET_Y_COTE_CUBISO)
         - 5);
  blit32(g_sprites[5], NULL, scr, &r);
  
  
  /* Combler les faces du haut */
  r.w = g_sprites[6]->w;
  r.h = g_sprites[6]->h;
  
  s.w = r.w;
  s.h = r.h;
  
  r.x = POS_X_CUBISO + 35;
  r.y = POS_Y_CUBISO + 39;
  
  for (i = 0; i < nbCubLig; i++) {
    for (j = 0; j < nbCubLig; j++) {
      s.x = r.x + OFFSET_X_H_CUBISO * j;
      s.y = r.y + OFFSET_Y_H_CUBISO * (nbCubLig - j - 1);
      blit32(g_sprites[6], NULL, scr, &s);
    }
    r.x += OFFSET_X_H_CUBISO;
    r.y += OFFSET_Y_H_CUBISO;
  }
  
  /* Combler les faces du bas gauche */
  r.x = POS_X_CUBISO - 3 + 35;
  r.y = POS_Y_CUBISO + nbCubLig * OFFSET_Y_H_CUBISO + 3 + 65;
  
  for (i = 0; i < nbCubLig - 1; i++) {
    for (j = 0; j < nbCubLig; j++) {
      s.x = r.x + 43 * j;
      s.y = r.y + OFFSET_Y_H_CUBISO * j;
      blit32(g_sprites[6], NULL, scr, &s);
    }
    r.y += r.h - OFFSET_Y_COTE_CUBISO + 54;
  }
  
  /* Combler les faces du bas droite */
  r.x = POS_X_CUBISO + nbCubLig * OFFSET_X_H_CUBISO - 2 + 35;
  r.y = POS_Y_CUBISO + nbCubLig * OFFSET_Y_H_CUBISO + (nbCubLig - 1) * OFFSET_Y_H_CUBISO + 3 + 42;
  
  for (i = 0; i < nbCubLig - 1; i++) {
    for (j = 0; j < nbCubLig - 1; j++) {
      s.x = r.x + (g_sprites[1]->w - 1) * j;
      s.y = r.y - OFFSET_Y_H_CUBISO * j;
      blit32(g_sprites[6], NULL, scr, &s);
    }
    r.y += r.h - OFFSET_Y_COTE_CUBISO + 54;
  }
}

void afficheCubeIso(Cube * cube) {
  static SDL_Surface * scr;
  SDL_Rect r, s;
  int i, j;
  int nbCubLig;
  
  if (!scr)
    scr = SDL_GetVideoSurface();
  nbCubLig = cube->nbCubLig;
  
  /* Face du haut */
  r.w = g_sprites[2]->w;
  r.h = g_sprites[2]->h;
  
  s.w = r.w;
  s.h = r.h;
  
  r.x = POS_X_CUBISO;
  r.y = POS_Y_CUBISO;
  
  for (i = 0; i < nbCubLig; i++) {
    for (j = 0; j < nbCubLig; j++) {
      s.x = r.x + OFFSET_X_H_CUBISO * j;
      s.y = r.y + OFFSET_Y_H_CUBISO * (nbCubLig - j - 1);
      
      fillNBlitPerColor32(g_sprites[2], NULL, scr, &s, 0xFFFFFF,
                          g_couleurs[cube->faces[F_HAUT][i * nbCubLig + j]]);
      
    }
    r.x += OFFSET_X_H_CUBISO;
    r.y += OFFSET_Y_H_CUBISO;
  }
  
  /* Face de gauche */
  r.w = g_sprites[0]->w;
  r.h = g_sprites[0]->h;
  
  s.w = r.w;
  s.h = r.h;
  
  r.x = POS_X_CUBISO - 3;
  r.y = POS_Y_CUBISO + nbCubLig * OFFSET_Y_H_CUBISO + 3;
  
  for (i = 0; i < nbCubLig; i++) {
    for (j = 0; j < nbCubLig; j++) {
      s.x = r.x + 43 * j;
      s.y = r.y + OFFSET_Y_H_CUBISO * j;
      
      fillNBlitPerColor32(g_sprites[0], NULL, scr, &s, 0xFFFFFF,
                          g_couleurs[cube->faces[F_GAUCHE][i * nbCubLig + j]]);
      
    }
    r.y += r.h - OFFSET_Y_COTE_CUBISO;
  }
  
  /* Face de droite (face F_FACE) */
  r.w = g_sprites[1]->w;
  r.h = g_sprites[1]->h;
  
  s.w = r.w;
  s.h = r.h;
  
  r.x = POS_X_CUBISO + nbCubLig * OFFSET_X_H_CUBISO - 2;
  r.y = POS_Y_CUBISO + nbCubLig * OFFSET_Y_H_CUBISO + (nbCubLig - 1) * OFFSET_Y_H_CUBISO + 3;
  
  for (i = 0; i < nbCubLig; i++) {
    for (j = 0; j < nbCubLig; j++) {
      s.x = r.x + (g_sprites[1]->w - 1) * j;
      s.y = r.y - OFFSET_Y_H_CUBISO * j;
      
      fillNBlitPerColor32(g_sprites[1], NULL, scr, &s, 0xFFFFFF,
                          g_couleurs[cube->faces[F_FACE][i * nbCubLig + j]]);
      
    }
    r.y += r.h - OFFSET_Y_COTE_CUBISO;
  }
}


void afficheBoutons(Bouton ** btn, int force) {
  static SDL_Surface * scr;
  int i;
  
  if (!scr)
    scr = SDL_GetVideoSurface();
  if (!btn)
    return;
  
  for (i = 0; btn[i]; i++) {
    if (/*btn[i]->actif && */(btn[i]->majAff || force)) {
      btn[i]->majAff = 0;
      blit32(g_bckgrnd[0], &btn[i]->r, scr, &btn[i]->r);
      blit32(btn[i]->s[btn[i]->etat], NULL, scr, &btn[i]->r);
    }
  }
}

void refreshScreen(void) {
  g_oldBckgrnd = -1;
}

/* Le pointeur de pointeur 'btn' doit se terminer par un pointeur NULL */
void affiche(int NBckgrnd, Cube * cube, Bouton ** btn) {
  static SDL_Surface * scr;
  int force;
  
  if (!scr)
    scr = SDL_GetVideoSurface();
  
  if (SDL_MUSTLOCK(scr))
    SDL_LockSurface(scr);
  
  if (g_oldBckgrnd != NBckgrnd) {
    g_oldBckgrnd = NBckgrnd;
    blit32(g_bckgrnd[0], NULL, scr, NULL);
    if (NBckgrnd == BCK_MENU)
      blit32(g_bckgrnd[1], NULL, scr, NULL);
    else if (NBckgrnd == BCK_JEU) {
      afficheCoinsCubeIso(cube);
      
      SDL_Rect r;
      SDL_Surface * sfc;
      char str[50] = "";
      
      r.x = SCR_W - g_cadre->w - 12;
      r.y = 12;
      r.w = g_cadre->w;
      r.h = g_cadre->h;
      
      blit32(g_cadre, NULL, scr, &r);
      
      sprintf(str, "Cube : %dx%d", cube->nbCubLig, cube->nbCubLig);
      sfc = creerTxtSurface(str);
      if (sfc) {
        r.x = SCR_W - g_cadre->w;
        r.y = 12 + (g_cadre->h / 4) - sfc->h / 2;
        r.w = sfc->w;
        r.h = sfc->h;
        blit32(g_bckgrnd[0], &r, scr, &r);
        blit32(sfc, NULL, scr, &r);
        SDL_FreeSurface(sfc);
      }
      else
        aprintf("Erreur de création de la surface de cube: %s.\n", SDL_GetError());
    }
  }
  
  force = 0;
  if (cube && cube->majAff) {
    cube->majAff = 0;
    force = 1;
    
    afficheCubeIso(cube);
  }
  
  if (cube) {
    char str[50] = "";
    SDL_Surface * sfc;
    SDL_Rect r;
    
    sprintf(str, "Temps: %02d:%02d",
            ((cube->tpsTotal / 1000) / 60) % 60, (cube->tpsTotal / 1000) % 60);
    sfc = creerTxtSurface(str);
    if (sfc) {
      r.x = SCR_W - g_cadre->w;
      r.y = 12 + 2 * (g_cadre->h / 4) - sfc->h / 2;
      r.w = sfc->w;
      r.h = sfc->h;
      blit32(g_bckgrnd[0], &r, scr, &r);
      blit32(sfc, NULL, scr, &r);
      SDL_FreeSurface(sfc);
    }
    else
      aprintf("Erreur de création de la surface de temps: %s.\n", SDL_GetError());
    
    
    sprintf(str, "Coups: %d", cube->nbCoups);
    sfc = creerTxtSurface(str);
    if (sfc) {
      r.x = SCR_W - g_cadre->w;
      r.y = 12 + 3 * (g_cadre->h / 4) - sfc->h / 2;
      r.w = sfc->w;
      r.h = sfc->h;
      blit32(g_bckgrnd[0], &r, scr, &r);
      blit32(sfc, NULL, scr, &r);
      SDL_FreeSurface(sfc);
    }
    else
      aprintf("Erreur de création de la surface de temps: %s.\n", SDL_GetError());
  }
  
  afficheBoutons(btn, force);
  
  if (SDL_MUSTLOCK(scr))
    SDL_UnlockSurface(scr);
  
  SDL_Flip(scr);
  
  SDL_Delay(timeLeft());
  g_nxtTime = SDL_GetTicks() + TICK;
}

