#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cube.h"
#include "secure.h"

#define STR(s)  #s

/* Variable globale au fichier qui contient toutes les faces du cube */
static int g_cube[NB_FACES][NB_CUB_FACE_MAX];


static void initFaceCouleur(int * tab, int couleur);
static void inverserFaceArr(int * face, int sens, int nbCubLig);
static void rotationFace(int * face, int sens, int nbCubLig);
static int checkRotateFace(int v, int sens, int * face, int nbCubLig);


static void initFaceCouleur(int * tab, int couleur) {
  int i;
  
  for (i = 0; i < NB_CUB_FACE_MAX; i++)
    tab[i] = couleur;
}

void initCube(Cube * cube, int nbCubLig) {
  int i;
  
  if (nbCubLig < 1 || nbCubLig > NB_CUB_LIG_MAX)
    nbCubLig = NB_CUB_LIG_MAX;
  
  for (i = 0; i < NB_FACES; i++) {
    initFaceCouleur(g_cube[i], i);
    cube->faces[i] = g_cube[i];
  }
  cube->nbCubLig = nbCubLig;
  cube->nbCubFace = nbCubLig * nbCubLig;
  cube->nbCoups = 0;
  cube->tpsTotal = 0;
  cube->majAff = 1;
}

Cube * nouveauCube(void) {
  return emalloc(sizeof(Cube), "Erreur de création du cube.\n");
}

void detruireCube(Cube * (*cube)) {
  free(*cube);
  *cube = NULL;
}

static void inverserFaceArr(int * face, int sens, int nbCubLig) {
  int i, j;
  int tmp[NB_CUB_FACE_MAX];
  
  memcpy(tmp, face, NB_CUB_FACE_MAX * sizeof *tmp);
  
  for (i = 0; i < nbCubLig; i++) {
    for (j = 0; j < nbCubLig; j++) {
      if (sens == R_HAUT || sens == R_BAS)
        face[i * nbCubLig + j] = tmp[(nbCubLig - i - 1) * nbCubLig + j];
      else if (sens == R_DROITE || sens == R_GAUCHE)
        face[i * nbCubLig + j] = tmp[i * nbCubLig + (nbCubLig - j - 1)];
    }
  }
}

static void rotationFace(int * face, int sens, int nbCubLig) {
  int i, j;
  int tmp[NB_CUB_FACE_MAX];
  
  memcpy(tmp, face, NB_CUB_FACE_MAX * sizeof *tmp);
  
  for (i = 0; i < nbCubLig; i++) {
    for (j = 0; j < nbCubLig; j++) {
      if (sens == R_HORRAIRE)
        face[j * nbCubLig + (nbCubLig - i - 1)] = tmp[i * nbCubLig + j];
      else if (sens == R_AHORRAIRE)
        face[(nbCubLig - j - 1) * nbCubLig + i] = tmp[i * nbCubLig + j];
      else {
        aprintf("'%s' en dehors des limites.\n", STR(sens));
        return;
      }
    }
  }
}

/* Le nom des rotations est choisi en fonction de la face qui sera amenée
 * à être 'en face'. Si je prend la rotation R_DROITE, à la fin de la rotation,
 * la face qui se trouvait à droite se trouvera 'en face'. */

void rotationCubeEntier(Cube * cube, int sens) {
  static int faceRot[4][6] = {
    /* R_HAUT */
    { F_BAS, F_HAUT, F_GAUCHE, F_DROITE, R_HORRAIRE, R_AHORRAIRE },
    /* R_BAS */
    { F_HAUT, F_BAS, F_GAUCHE, F_DROITE, R_AHORRAIRE, R_HORRAIRE },
    /* R_GAUCHE */
    { F_DROITE, F_GAUCHE, F_HAUT, F_BAS, R_AHORRAIRE, R_HORRAIRE },
    /* R_DROITE */
    { F_GAUCHE, F_DROITE, F_HAUT, F_BAS, R_HORRAIRE, R_AHORRAIRE }
  };
  int * tmp;
  
  if (sens < 0 || sens > R_DROITE) {
    aprintf("'%s' en dehors des limites.\n", STR(sens));
    return;
  }
  
  inverserFaceArr(cube->faces[F_ARR], sens, cube->nbCubLig);
  
  tmp = cube->faces[faceRot[sens][0]];
  cube->faces[faceRot[sens][0]] = cube->faces[F_FACE];
  cube->faces[F_FACE] = cube->faces[faceRot[sens][1]];
  cube->faces[faceRot[sens][1]] = cube->faces[F_ARR];
  cube->faces[F_ARR] = tmp;
  rotationFace(cube->faces[faceRot[sens][2]], faceRot[sens][4], cube->nbCubLig);
  rotationFace(cube->faces[faceRot[sens][3]], faceRot[sens][5], cube->nbCubLig);
  
  inverserFaceArr(cube->faces[F_ARR], sens, cube->nbCubLig);
  
  cube->majAff = 1;
}

void rotationDeFace(Cube * cube, int sens) {
  rotationCubeEntier(cube, R_HAUT);
  if (sens == R_HORRAIRE)
    rotationCubeEntier(cube, R_GAUCHE);
  else
    rotationCubeEntier(cube, R_DROITE);
  rotationCubeEntier(cube, R_BAS);
  
  cube->majAff = 1;
}

static int checkRotateFace(int v, int sens, int * face, int nbCubLig) {
  static int faceRes[2][4][2] = {
    /* v = 0 */
    {
      /* R_HAUT */
      { F_GAUCHE, R_AHORRAIRE },
      /* R_BAS */
      { F_GAUCHE, R_HORRAIRE },
      /* R_GAUCHE */
      { F_HAUT, R_HORRAIRE },
      /* R_DROITE */
      { F_HAUT, R_AHORRAIRE }
    },
    /* v = nbCubLig-1 */
    {
      /* R_HAUT */
      { F_DROITE, R_HORRAIRE },
      /* R_BAS */
      { F_DROITE, R_AHORRAIRE },
      /* R_GAUCHE */
      { F_BAS, R_AHORRAIRE },
      /* R_DROITE */
      { F_BAS, R_HORRAIRE }
    }
  };
  
  if (v < 0 || v >= nbCubLig)
    return NO_ROTATE;
  else if (v == nbCubLig - 1)
    v = 1;
  else if (v == 0)
    v = 0; /* Funny isn't it ? */
  else
    return NO_ROTATE;
  
  *face = faceRes[v][sens][0];
  return faceRes[v][sens][1];;
}

/* v : Numéro de la ligne/colonne à bouger */
void mouvement(Cube * cube, int v, int sens) {
  /* Sens de rotation des faces */
  static int faceRot[4][4] = {
    /* R_HAUT */
    { F_FACE, F_BAS, F_ARR, F_HAUT },
    /* R_BAS */
    { F_FACE, F_HAUT, F_ARR, F_BAS },
    /* R_GAUCHE */
    { F_FACE, F_DROITE, F_ARR, F_GAUCHE },
    /* R_DROITE */
    { F_FACE, F_GAUCHE, F_ARR, F_DROITE }
  };
  
  int tmp[NB_CUB_LIG_MAX] = { 0 };
  int face;
  int i, j, k, l, s;
  int x, y;
  int incX, incY;
  int nbCubLig;
  
  if (sens < 0 || sens > R_DROITE) {
    aprintf("'%s' en dehors des limites.\n", STR(sens));
    return;
  }
  if (v < 0 || v >= cube->nbCubLig) {
    aprintf("'%s' en dehors des limites.\n", STR(sens));
    return;
  }
  
  nbCubLig = cube->nbCubLig;
  
  /* Si on fait un mouvement sur un côté, on tourne la retourne la face.
   * Si on est au milieu, on ne fait rien */
  s = checkRotateFace(v, sens, &face, cube->nbCubLig);
  if (s != NO_ROTATE)
    rotationFace(cube->faces[face], s, cube->nbCubLig);
  
  x = y = incX = incY = 0;
  if (sens == R_HAUT || sens == R_BAS) {
    x = v;
    incX = 0;
    y = 0;
    incY = 1;
  }
  else if (sens == R_GAUCHE || sens == R_DROITE) {
    x = 0;
    incX = 1;
    y = v;
    incY = 0;
  }
  
  /* On sauvegarde la face */
  for (k = 0, i = x, j = y;
       i < nbCubLig && j < nbCubLig;
       i += incX, j += incY, k++) {
    tmp[k] = cube->faces[faceRot[sens][0]][j * nbCubLig + i];
  }
  
  for (l = 1; l < 4; l++) {
    for (i = x, j = y;
         i < nbCubLig && j < nbCubLig;
         i += incX, j += incY) {
      
      /* Là, c'est un peu compliqué.
       * En fonction des rotations il faut qu'on tourne la face arrière
       * d'une certaine manière pour rester 'en phase' avec les autres
       * côtés. Ca vient du fait que la face arrière est différente des autres.
       * 
       * J'essaierai de l'améliorer si l'envie se faire sentir...
       */
      if ((sens == R_HAUT || sens == R_BAS) &&
          faceRot[sens][l] == F_ARR) {
        cube->faces[faceRot[sens][l-1]][j * nbCubLig + i] =
        /* */cube->faces[faceRot[sens][l]][(nbCubLig - j - 1) * nbCubLig + i];
      }
      else if ((sens == R_GAUCHE || sens == R_DROITE) &&
               faceRot[sens][l] == F_ARR) {
        cube->faces[faceRot[sens][l-1]][j * nbCubLig + i] =
        /* */cube->faces[faceRot[sens][l]][j * nbCubLig + (nbCubLig - i - 1)];
      }
      else if ((sens == R_HAUT || sens == R_BAS) &&
               faceRot[sens][l-1] == F_ARR) {
        cube->faces[faceRot[sens][l-1]][(nbCubLig - j - 1) * nbCubLig + i] =
        /* */cube->faces[faceRot[sens][l]][j * nbCubLig + i];
      }
      else if ((sens == R_GAUCHE || sens == R_DROITE) &&
               faceRot[sens][l-1] == F_ARR) {
        cube->faces[faceRot[sens][l-1]][j * nbCubLig + (nbCubLig - i - 1)] =
        /* */cube->faces[faceRot[sens][l]][j * nbCubLig + i];
      }
      else {
        cube->faces[faceRot[sens][l-1]][j * nbCubLig + i] =
        /*                */cube->faces[faceRot[sens][l]][j * nbCubLig + i];
      }
    }
  }
  
  for (k = 0, i = x, j = y;
       i < nbCubLig && j < nbCubLig;
       i += incX, j += incY, k++) {
    int temp = faceRot[sens][3];
    cube->faces[temp][j * nbCubLig + i] = tmp[k];
  }
  
  cube->majAff = 1;
}

void melangerCube(Cube * cube) {
  int i, tmp;
  
  for (i = 0; i < 256; i++) {
    tmp = rand() % 8;
    
    if (tmp < 4) {
      rotationCubeEntier(cube, tmp);
    }
    else if (tmp < 6) {
      rotationDeFace(cube, tmp);
    }
    else {
      int v = rand() % cube->nbCubLig;
      mouvement(cube, v, tmp % 4);
    }
  }
  
  cube->majAff = 1;
}



