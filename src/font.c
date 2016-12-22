#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "SDL.h"

#include "afficher.h"
#include "font.h"
#include "secure.h"

static SDL_Surface * g_font;

int initFont(void) {
  if (g_font == NULL) {
    g_font = chargerImage32("font.png");
    setAlphaParCouleur32(g_font, 0);
  }
  
  return 0;
}

void releaseFont(void) {
  if (g_font) {
    SDL_FreeSurface(g_font);
    g_font = NULL;
  }
}

enum {
  SZ_FONT_H = 6,
  SZ_FONT_W = 16
};

SDL_Surface * creerTxtSurface(char * s) {
  SDL_Surface * tmp;
  SDL_Surface * sfc;
  SDL_Rect src, dst;
  /* Fond de 16 par 6 */
  static char * t = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                    "[\\]^_'abcdefghijklmnopqrstuvwxyz{|}~ ";
  int i;
  char * p;
  
  tmp = SDL_CreateRGBSurface(SDL_SWSURFACE, g_font->w / SZ_FONT_W * strlen(s),
                             g_font->h / SZ_FONT_H,
                             32, 0, 0, 0, 0xFF000000);
  if (!tmp)
    eprintf("Erreur de création de la surface pour la font.\n");
  sfc = SDL_DisplayFormatAlpha(tmp);
  SDL_FreeSurface(tmp);
  if (!sfc)
    eprintf("Erreur de création de la surface pour la font.\n");
  
  memset(&src, 0, sizeof(src));
  memset(&dst, 0, sizeof(dst));
  src.w = g_font->w / SZ_FONT_W;
  src.h = g_font->h / SZ_FONT_H;
  dst.w = src.w;
  dst.h = src.h;
  
  for (i = 0; s[i]; i++) {
    p = strchr(t, s[i]);
    if (!p)
      p = t;
    src.x = (g_font->w / SZ_FONT_W) * ((p - t) % SZ_FONT_W);
    src.y = (g_font->h / SZ_FONT_H)  * ((p - t) / SZ_FONT_W);
    blit32(g_font, &src, sfc, &dst);
    dst.x += src.w;
  }
  setAlphaParCouleur32(sfc, 0);
  
  return sfc;
}







