#ifndef MENU_H
#define MENU_H

#include "xmenu.h"

void InitializeMenu();
void DestroyMenu();
void MenuFrame();

extern XMenu   menu;
extern Window *menuWindow;
extern char   *bones[];

#endif
