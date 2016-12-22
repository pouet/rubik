#include "SDL.h"

#include "collision.h"

/* Ne teste la collision que d'un point (boite/point) */
/* Dans l'argument 'point' seul 'x' et 'y' seront utilisés */
int collisionPoint(SDL_Rect * box, SDL_Rect * point) {
  int ret;
  
  if (!box || !point)
    return 0;
  
  ret = 0;
  if (point->x >= box->x && point->x < box->x + box->w &&
      point->y >= box->y && point->y < box->y + box->h)
    ret = 1;
  
  return ret;
}
