#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

typedef struct SItem SItem;
struct SItem {
	char * pItemName;
	int	nItemVal;
  
	int	* pVal;
	int	nVal;
};

enum {
  M_ERR = -1,
  M_QUIT,
  M_MENU,
  M_NOUVEAU,
  M_CHARGE,
  M_JEU,
  M_INIT_JEU,
  M_REPRISE,
  M_OPT,
  M_MELANGE,
  M_REINIT,
  M_SAUVE,
  M_PAUSE
};

int menu(SItem * it, int nbItem, int reprise);

#endif