#include <stdio.h>

#include "cube.h"
#include "fichiers.h"
#include "secure.h"

int chargerCube(Cube * cube) {
  FILE * f;
  int i, n;
  int ret;
  
  ret = 0;
  f = fopen(FICH_SAV_CUBE, "rb");
  if (f == NULL) {
    aprintf("Erreur lors de l'ouverture du fichier %s.\n", FICH_SAV_CUBE);
    return -1;
  }
  
  n = fread(&cube->tpsTotal, sizeof(cube->tpsTotal), 1, f);
  if (n != 1) {
    ret = -1;
    goto end;
  }
  n = fread(&cube->nbCoups, sizeof(cube->nbCoups), 1, f);
  if (n != 1) {
    ret = -1;
    goto end;
  }
  n = fread(&cube->nbCubLig, sizeof(cube->nbCubLig), 1, f);
  if (n != 1) {
    ret = -1;
    goto end;
  }
  n = fread(&cube->nbCubFace, sizeof(cube->nbCubFace), 1, f);
  if (n != 1) {
    ret = -1;
    goto end;
  }
  
  for (i = 0; i < NB_FACES; i++) {
    n = fread(cube->faces[i], sizeof(cube->faces[i]), NB_CUB_FACE_MAX, f);
    if (n != NB_CUB_FACE_MAX) {
      ret = -1;
      goto end;
    }
  }
  
  cube->majAff = 1;
  
end:
  fclose(f);
  if (ret == -1)
    remove(FICH_SAV_CUBE);
  return ret;
}

int sauverCube(Cube * cube) {
  FILE * f;
  int i, n;
  int ret;
  
  ret = 0;
  f = fopen(FICH_SAV_CUBE, "wb");
  if (f == NULL) {
    aprintf("Erreur lors de l'ouverture/création du fichier %s.\n",
            FICH_SAV_CUBE);
    return -1;
  }
  
  n = fwrite(&cube->tpsTotal, sizeof(cube->tpsTotal), 1, f);
  if (n != 1) {
    ret = -1;
    goto end;
  }
  n = fwrite(&cube->nbCoups, sizeof(cube->nbCoups), 1, f);
  if (n != 1) {
    ret = -1;
    goto end;
  }
  n = fwrite(&cube->nbCubLig, sizeof(cube->nbCubLig), 1, f);
  if (n != 1) {
    ret = -1;
    goto end;
  }
  n = fwrite(&cube->nbCubFace, sizeof(cube->nbCubFace), 1, f);
  if (n != 1) {
    ret = -1;
    goto end;
  }
  
  for (i = 0; i < NB_FACES; i++) {
    n = fwrite(cube->faces[i], sizeof(cube->faces[i]), NB_CUB_FACE_MAX, f);
    if (n != NB_CUB_FACE_MAX) {
      ret = -1;
      goto end;
    }
  }
  
end:
  fclose(f);
  if (ret == -1)
    remove(FICH_SAV_CUBE);
  return 0;
}

int fichierExistant(char * s) {
  FILE * f;
  int ret;
  
  f = fopen(s, "r");
  if (f != NULL) {
    ret = 1;
    fclose(f);
  }
  else
    ret = 0;
  
  return ret;
}
