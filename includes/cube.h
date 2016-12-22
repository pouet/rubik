#ifndef CUBE_H_INCLUDED
#define CUBE_H_INCLUDED

enum {
  NB_CUB_LIG_MAX = 10,
  NB_CUB_FACE_MAX = NB_CUB_LIG_MAX * NB_CUB_LIG_MAX
};

enum rotation_e {
  R_HAUT,
  R_BAS,
  R_GAUCHE,
  R_DROITE,
  R_HORRAIRE,
  R_AHORRAIRE,
  NO_ROTATE,
  NB_ROTATE
};

enum faces_e {
  F_HAUT,
  F_GAUCHE,
  F_FACE,
  F_DROITE,
  F_BAS,
  F_ARR,
  NB_FACES
};

/* Les couleurs sont classées par rapport aux faces définies juste au dessus */
/* Chaque couleur de face opposée correspond à ajouter/supprimer du jaune
 * à la face 'en cours'.
 * Par exemple, si la face en cours est jaune, la face opposée sera blanche
 * (jaune-jaune=blanc) et inversement, si la face est blanche, la face opposée
 * sera jaune (blanc+jaune=jaune). */

enum couleurs_e {
  C_JAUNE,
  C_VERT,
  C_ORANGE,
  C_BLEU,
  C_BLANC,
  C_ROUGE,
  NB_COUL
};

typedef struct Cube Cube;
struct Cube {
  int * faces [NB_FACES];
  int nbCubLig;
  int nbCubFace;
  int nbCoups;
  int tpsTotal;
  int majAff;
};

Cube * nouveauCube(void);
void detruireCube(Cube * (*cube));
void initCube(Cube * cube, int nbCubLig);
void rotationDeFace(Cube * cube, int sens);
void rotationCubeEntier(Cube * cube, int sens);
void mouvement(Cube * cube, int v, int sens);
void melangerCube(Cube * cube);

#endif
