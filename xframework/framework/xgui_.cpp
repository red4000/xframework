#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "_vector.h"
#include "drender.h"
#include "xgui.h"
#include "xguirender.h"
#include <commctrl.h>
#include "input.h"
#include "menu.h"

#define MENUKEY VK_INSERT
WindowManager wm;

int menuOpen = 0, mx, my;

void Element::RemoveSubElement(Element *e) {
	auto it = subElements.find(e);

	if (it) {
		subElements.erase(it);
	}
}

TabContainer::TabContainer() {
	selectedTab = NULL;
}

TabContainer *Window::CreateTabContainer(Element *parent, int x, int y, int w, int h) {
	TabContainer *res = new TabContainer();
	res->x            = x;
	res->y            = y;
	res->w            = w;
	res->h            = h;
	res->t            = E_TABCONTAINER;
	res->xo           = x + 3;
	res->hoverTime    = 0;
	res->enabled      = 1;
	guiRender.SetElementSize(E_TABCONTAINER, res);
	parent->subElements.push_back(res);
	return res;
}

Tab *Window::CreateTab(TabContainer *container, char *text) {
	Tab *res  = new Tab();
	res->text = text;
	res->x    = container->xo;
	RECT r;
	render.CalcTextRect(guiRender.menuFont, text, &r);
	res->w         = res->tx = r.right;
	res->w         += 4;
	/*if (res->w <= MINTABW) {
		res->w = MINTABW;
	}*/
	res->h         = r.bottom;
	res->y         = container->y - (res->h + 1);
	res->t         = E_TAB;
	res->enabled   = 1;
	res->hoverTime = 0;
	container->xo += res->w + 5;
	guiRender.SetElementSize(E_TAB, res);
	container->subElements.push_back(res);

	int startX = 0, cX;

	for (auto it = container->subElements.begin(); it != container->subElements.end(); it++) {
		render.CalcTextRect(guiRender.menuFont, (*it)->text, &r);
		if (startX > 0) {
			startX += 5;
		}
		(*it)->x = startX;
		
		startX += r.right + 5;
		((Tab*)*it)->tx = 0;
	}
	
	cX = w - (startX + 7);

	for (auto it = container->subElements.begin(); it != container->subElements.end(); it++) {
		(*it)->x = cX;
		cX += (*it)->w + 5;
	}

	if (container->selectedTab == NULL) {
		container->selectedTab = res;
	}
	// TODO: reposition every other tab to accompany for further left justification
	// (lol)
	return res;
}

CheckBox *Window::CreateCheckBox(Element *parent, char *text, int x, int y, int *state) {
	CheckBox *res = new CheckBox();
	res->text     = text;
	res->x        = x;
	res->y        = y;
	RECT r;
	render.CalcTextRect(guiRender.menuFont, text, &r);
	res->h         = guiRender.menuFont->h;
	res->w         = res->h + 2 + r.right;
	res->t         = E_CHECKBOX;
	res->enabled   = 1;
	res->state     = state;
	res->hoverTime = 0;
	guiRender.SetElementSize(E_CHECKBOX, res);
	parent->subElements.push_back(res);
	return res;
}

Slider *Window::CreateSlider(Element *p, char *text, int x, int y, int w, int h, float *frac, float min, float max, ElementChangedCallback onChange) {
	Slider *res    = new Slider();
	res->text      = text;
	res->x         = x;
	res->y         = y;
	res->w         = w;
	res->h         = guiRender.menuFont->h + 8;
	res->valueType = V_FLOAT;
	res->fFrac     = frac;
	res->fMin      = min;
	res->fMax      = max;
	res->scale     = (max - min);
	res->t         = E_SLIDER;
	res->enabled   = 1;
	res->hoverTime = 0;
	res->onChange  = onChange;
	guiRender.SetElementSize(E_SLIDER, res);
	p->subElements.push_back(res);
	return res;
}

Slider *Window::CreateSlider(Element *p, char *text, int x, int y, int w, int h, int *frac, int min, int max, ElementChangedCallback onChange) {
	Slider *res    = new Slider();
	res->text      = text;
	res->x         = x;
	res->y         = y;
	res->w         = w;
	res->h         = guiRender.menuFont->h + 8;
	res->valueType = V_INT;
	res->iFrac     = frac;
	res->iMin      = min;
	res->iMax      = max;
	res->scale     = (max - min);
	res->t         = E_SLIDER;
	res->enabled   = 1;
	res->hoverTime = 0;
	res->onChange  = onChange;
	guiRender.SetElementSize(E_SLIDER, res);
	p->subElements.push_back(res);
	return res;
}

PreciseSlider *Window::CreatePreciseSlider(Element *p, char *text, int x, int y, int w, int h, float *frac, float min, float max, ElementChangedCallback onChange) {
	PreciseSlider *res = new PreciseSlider();
	res->text          = text;
	res->x             = x;
	res->y             = y;
	res->w             = w;
	res->h             = guiRender.menuFont->h + 8;
	res->frac          = frac;
	res->min           = min;
	res->max           = max;
	res->t             = E_PRECISESLIDER;
	res->enabled       = 1;
	res->hoverTime     = 0;
	res->onChange      = onChange;
	//res->ib = CreateInputBox();
	guiRender.SetElementSize(E_PRECISESLIDER, res);
	p->subElements.push_back(res);
	return res;
}

ComboBox *Window::CreateComboBox(Element *p, char *text, char **entries, int numEntries, int x, int y, int w, int h, int *selectedIndex, ElementChangedCallback onChange) {
	ComboBox *res   = new ComboBox();
	res->x          = x;
	res->y          = y;
	RECT r;
	render.CalcTextRect(guiRender.menuFont, text, &r);
	res->xo         = r.right + 2;
	res->w          = res->xo + w;
	res->h          = guiRender.menuFont->h + 4;
	res->text       = text;
	res->entries    = entries;
	res->numEntries = numEntries;
	res->selected   = selectedIndex;
	res->state      = 0;
	res->t          = E_COMBOBOX;
	res->enabled    = 1;
	res->hoverTime  = 0;
	res->onChange   = onChange;
	guiRender.SetElementSize(E_COMBOBOX, res);
	p->subElements.push_back(res);
	return res;
}

InputBox *Window::CreateInputBox(Element *p, char *text, int x, int y, int w, int h, InputBoxCallback onEnter) {
	InputBox *res   = new InputBox();
	res->x          = x;
	res->y          = y;
	res->w          = w;
	res->h          = 2 * guiRender.menuFont->h + 4;
	res->text       = text;
	res->t          = E_INPUTBOX;
	res->enabled    = 1;
	res->onEnter    = onEnter;
	res->hoverTime  = 0;
	res->cursorTime = 0;
	res->input      = (char*)calloc(1, MAXINPUTSIZE);
	res->right      = 2;
	guiRender.SetElementSize(E_INPUTBOX, res);
	p->subElements.push_back(res);
	return res;
}

Button *Window::CreateButton(Element *p, char *text, int x, int y, int w, int h, ButtonCallback onClick) {
	Button *res    = new Button();
	res->x         = x;
	res->y         = y;
	res->w         = w;
	res->h         = h;
	res->text      = text;
	res->onClick   = onClick;
	res->t         = E_BUTTON;
	res->enabled   = 1;
	res->hoverTime = 0;
	guiRender.SetElementSize(E_BUTTON, res);
	p->subElements.push_back(res);
	return res;
}

TextBox *Window::CreateTextBox(Element *p, char *text, int x, int y, int w, int h) {
	TextBox *res   = new TextBox();
	res->x         = x;
	res->y         = y;
	res->w         = w;
	res->h         = h;
	res->text      = text;
	res->t         = E_TEXTBOX;
	res->enabled   = 1;
	res->hoverTime = 0;
	guiRender.SetElementSize(E_TEXTBOX, res);
	p->subElements.push_back(res);
	return res;
}

ListView *Window::CreateListView(Element *p, char *text, int x, int y, int w, int h, char **items, int numItems) {
	ListView *res  = new ListView();
	res->x         = x;
	res->y         = y;
	res->w         = w;
	res->h         = h;
	res->text      = text;
	res->t         = E_LISTVIEW;
	res->enabled   = 1;
	res->hoverTime = 0;
	guiRender.SetElementSize(E_LISTVIEW, res);
	for (size_t i = 0; i < numItems; i++) {
		res->items.push_back(items[i]);
	}
	res->sh = (int)(res->h * 0.25f);
	res->sy = res->si = res->selectedItem = 0;

	p->subElements.push_back(res);
	return res;
}

Label *Window::CreateLabel(Element *p, char *text, int x, int y) {
	Label *res     = new Label();
	res->x         = x;
	res->y         = y;
	res->w         = 0;
	res->h         = 0;
	res->text      = text;
	res->t         = E_LABEL;
	res->enabled   = 1;
	res->hoverTime = 0;
	guiRender.SetElementSize(E_LABEL, res);
	p->subElements.push_back(res);
	return res;
}

GroupBox *Window::CreateGroupBox(Element *p, char *text, int x, int y, int w, int h) {
	GroupBox *res  = new GroupBox();
	res->x         = x;
	res->y         = y;
	res->w         = w;
	res->h         = h;
	res->text      = text;
	RECT r;
	render.CalcTextRect(guiRender.menuFont, text, &r);
	res->tw        = r.right + 2;
	res->t         = E_GROUPBOX;
	res->enabled   = 1;
	res->hoverTime = 0;
	guiRender.SetElementSize(E_GROUPBOX, res);
	p->subElements.push_back(res);
	return res;
}

ExpandBox *Window::CreateExpandBox(Element *p, char *text, int x, int y, int w, int h) {
	ExpandBox *res  = new ExpandBox();
	res->x          = x;
	res->y          = y;
	res->w          = w;
	res->h          = res->th = h;
	res->text       = text;
	RECT r;
	render.CalcTextRect(guiRender.menuFont, text, &r);
	res->tw         = r.right + 2;
	res->t          = E_EXPANDBOX;
	res->state      = 0;
	res->h          = guiRender.menuFont->h + 4;
	res->enabled    = 1;
	res->hoverTime  = 0;
	guiRender.SetElementSize(E_EXPANDBOX, res);
	p->subElements.push_back(res);
	return res;
}

void Window::RenderElement(Element *e, TabContainer *tc, DWORD currentTime) {
	int sx = x + e->x, sy = y + e->y;
	//int fade;
	if (!CONTAINSMOUSE(sx, sy, e->w, e->h)) {
		guiRender.lineColor = ctrlLineCol;
		guiRender.textColor = backTextCol;

		if (currentTime) {
			DWORD fadeDelta = currentTime - e->hoverTime;
			if (fadeDelta != 0) {
				if (fadeDelta < FADETIME) {
					float fadeFrac = ((((float)FADETIME - fadeDelta) / ((float)FADETIME)));
					// todo: sexy color fading...
					//float startCol, endCol, col;
					//startCol = hl
					/*
					guiRender.lineColor  = hlctrlLineCol;
					guiRender.lineColor &= 0x00FFFFFF;
					guiRender.lineColor |= ((BYTE)((255.0f * fadeFrac) /*- ((float)((hlctrlLineCol & 0xFF000000) >> 24))*)) << 24;
					*/
				}
			}
		}
	} else {
		e->hoverTime        = currentTime;
		guiRender.lineColor = hlctrlLineCol;
		guiRender.textColor = highlightCol;
	}
	switch (e->t) {
	case E_CHECKBOX:
		guiRender.RenderCheckBox(sx, sy, (CheckBox*)e);
		break;

	case E_SLIDER:
		guiRender.RenderSlider(sx, sy, (Slider*)e);
		break;

	case E_COMBOBOX:
		guiRender.RenderComboBox(sx, sy, (ComboBox*)e);
		break;

	case E_INPUTBOX:
		guiRender.RenderInputBox(sx, sy, (InputBox*)e);
		break;

	case E_PRECISESLIDER:
		guiRender.RenderPreciseSlider(sx, sy, (PreciseSlider*)e);
		break;

	case E_BUTTON:
		guiRender.RenderButton(sx, sy, (Button*)e);
		break;

	case E_TEXTBOX:
		guiRender.RenderTextBox(sx, sy, (TextBox*)e);
		break;

	case E_LISTVIEW:
		guiRender.RenderListView(sx, sy, (ListView*)e);
		break;

	case E_TAB:
		guiRender.RenderTab(sx, sy, (Tab*)e, tc);
		break;
		                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               
	case E_TABCONTAINER:
		guiRender.RenderTabContainer(sx, sy, (TabContainer*)e);
		break;

	case E_LABEL:
		guiRender.RenderLabel(sx, sy, (Label*)e);
		break;

	case E_GROUPBOX:
		guiRender.RenderGroupBox(sx, sy, (GroupBox*)e);
		break;

	case E_EXPANDBOX:
		guiRender.RenderExpandBox(sx, sy, (ExpandBox*)e);
		break;
	}
}

void Window::Render() {
	guiRender.RenderWindow(this);
	if (state) {
		DWORD currentTime = GetTickCount();

		for (auto it = subElements.begin(); it != subElements.end(); it++) {
			Element *e       = *it;
			RenderElement(e, NULL, 0);
		
			switch (e->t) {
			case E_TABCONTAINER:
				for (auto it2 = e->subElements.begin(); it2 != e->subElements.end(); it2++) {
					Element *e2 = *it2;
					RenderElement(e2, (TabContainer*)e, 0);

					if (e2 == ((TabContainer*)e)->selectedTab) {
						for (auto it3 = e2->subElements.begin(); it3 != e2->subElements.end(); it3++) {
							if ((*it3)->enabled == 1) {
								RenderElement(*it3, (TabContainer*)e, currentTime);
								if ((*it3)->t == E_EXPANDBOX && ((ExpandBox*)(*it3))->state == 1) {
									for (auto it4 = (*it3)->subElements.begin(); it4 != (*it3)->subElements.end(); it4++) {
										if ((*it4)->enabled == 1) {
											RenderElement(*it4, (TabContainer*)e, currentTime);
										}
									}
								}
							}
						}
					}
				}
				break;
			}
		}
	}
}

void WindowManager::Initialize() {
	guiRender.Initialize();
	selectedWindow = NULL;
	heldElement    = selectedElement = NULL;
	RegisterInputListener(HandleGUIInput);
}

void WindowManager::Release() {
}

Window *WindowManager::CreateNewWindow(char *title, int w, int h) {
	Window *res  = new Window();
	res->x       = (render.sW / 2) - (w / 2);
	res->y       = (render.sH / 2) - (h / 2);
	res->w       = w;
	res->h       = h;
	res->text    = title;
	res->t       = E_WINDOW;
	res->enabled = res->state = 1;
	if (wm.selectedWindow == NULL) {
		wm.selectedWindow = res;
	}
	res->renderWindow = 1;
	windows.push_back(res);
	return res;
}

void WindowManager::Render() {
	for (auto it = wm.windows.begin(); it != wm.windows.end(); it++) {
		Window *w = *it;
		if (w->enabled == 1) {
			if (w->renderWindow) {
				w->Render();
			}
		}
	}
	
	if (menuOpen == 0) {
		return;
	}
	guiRender.RenderCursor(mx, my);
}

LRESULT CALLBACK HandleGUIInput(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (menuOpen == 0) {
		switch (uMsg) {
		case WM_KEYDOWN:
			if (wParam == MENUKEY) {
				menuOpen ^= 1;
				menuWindow->enabled ^= 1;
			}
			break;
		}
	} else {
		size_t len;
		switch (uMsg) {
		case WM_MOUSEMOVE:
			mx = LOWORD(lParam);
			my = HIWORD(lParam);
			if (!menuOpen) {
				goto hoverHandled;
			}
			if (wm.heldElement) {
				switch (wm.heldElement->t) {
				case E_WINDOW:
					wm.heldElement->x = mx - wm.dx;
					wm.heldElement->y = my - wm.dy;
					break;

				case E_SLIDER:
					if (((Slider*)wm.heldElement)->valueType == V_FLOAT) {
						(*((Slider*)wm.heldElement)->fFrac) = ((Slider*)wm.heldElement)->fMin + ((Slider*)wm.heldElement)->scale * (((float)(mx - (wm.selectedWindow->x + wm.heldElement->x))) / (float)wm.heldElement->w);
						if ( (*((Slider*)wm.heldElement)->fFrac) < ((Slider*)wm.heldElement)->fMin) {
							(*((Slider*)wm.heldElement)->fFrac) = ((Slider*)wm.heldElement)->fMin;
						} else {
							if ((*((Slider*)wm.heldElement)->fFrac) > ((Slider*)wm.heldElement)->fMax) {
								(*((Slider*)wm.heldElement)->fFrac) = ((Slider*)wm.heldElement)->fMax;
							}
						}
					} else {
						(*((Slider*)wm.heldElement)->iFrac) = ((Slider*)wm.heldElement)->iMin + ((Slider*)wm.heldElement)->scale * (((float)(mx - (wm.selectedWindow->x + wm.heldElement->x))) / (float)wm.heldElement->w);
						if ( (*((Slider*)wm.heldElement)->iFrac) < ((Slider*)wm.heldElement)->iMin) {
							(*((Slider*)wm.heldElement)->iFrac) = ((Slider*)wm.heldElement)->iMin;
						} else {
							if ((*((Slider*)wm.heldElement)->iFrac) > ((Slider*)wm.heldElement)->iMax) {
								(*((Slider*)wm.heldElement)->iFrac) = ((Slider*)wm.heldElement)->iMax;
							}
						}
					}
					if (((Slider*)wm.heldElement)->onChange != NULL) {
						((Slider*)wm.heldElement)->onChange();
					}
					break;

				case E_COMBOBOX:
					int sx = wm.heldElement->x + wm.selectedWindow->x, sy = wm.heldElement->y + wm.selectedWindow->y;
					if (CONTAINSMOUSE(sx, sy, wm.heldElement->w, wm.heldElement->h)) {
						for (size_t i = 0; i < ((ComboBox*)wm.heldElement)->numEntries; i++) {
							if (CONTAINSMOUSE(sx, sy + (i * guiRender.menuFont->h), wm.heldElement->w, guiRender.menuFont->h)) {
								//(*((ComboBox*)wm.heldElement)->selected) = i;
							}
						}
					}
					break;
				}
			} else {
				for (auto it = wm.windows.begin(); it != wm.windows.end(); it++) {
					Window *w = *it;

					if (CONTAINSMOUSE(w->x, w->y, w->w, w->h)) {
						for (auto it2 = w->subElements.begin(); it2 != w->subElements.end(); it2++) {
							TabContainer *e = (TabContainer*)*it2;
							for (auto it3 = e->subElements.begin(); it3 != e->subElements.end(); it3++) {
								Tab *t = (Tab*)*it3;
								if (CONTAINSMOUSE(w->x + t->x, w->y + t->y, t->w, t->h)) {
									e->hoverTime   = GetTickCount();
									goto hoverHandled;
								} else {
									if (e->selectedTab == t) {
										for (auto it4 = t->subElements.begin(); it4 != t->subElements.end(); it4++) {
											PElement pe;
											pe.e = *it4;
											if (pe.e->enabled == 1) {
												int sx = w->x + pe.e->x, sy = w->y + pe.e->y;
												if (CONTAINSMOUSE(sx, sy, pe.e->w, pe.e->h)) {
													pe.e->hoverTime = GetTickCount();
													goto hoverHandled;
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
			hoverHandled:
			break;

		case WM_KEYDOWN:
		//case WM_SYSKEYDOWN:
			if (wParam == MENUKEY) {
				menuOpen ^= 1;
				menuWindow->enabled ^= 1;
			}
			break;

		case WM_CHAR:
			if (menuOpen == 1 && wm.selectedElement && wm.selectedElement->t == E_INPUTBOX) {
				char c = tolower(wParam);
				switch (wParam) {
				case VK_BACK:
					len = strlen(((InputBox*)wm.selectedElement)->input);
					if (len) {
						((InputBox*)wm.selectedElement)->input[len - 1] = 0;
						RECT r;
						render.CalcTextRect(guiRender.menuFont, ((InputBox*)wm.selectedElement)->input, &r);
						((InputBox*)wm.selectedElement)->right = r.right + 1;
					}
					break;

				case VK_RETURN:
					((InputBox*)wm.selectedElement)->onEnter(((InputBox*)wm.selectedElement)->input);
					((InputBox*)wm.selectedElement)->input[0] = 0;
					((InputBox*)wm.selectedElement)->right    = 0;
					break;

				case VK_LEFT:
					break;

				case VK_RIGHT:
					break;
				}
				if ((wParam >= 'a' && wParam <= 'z') ||
					(wParam >= 'A' && wParam <= 'Z')) {
					if (wm.selectedElement) {
						switch (wm.selectedElement->t) {
						case E_INPUTBOX:
							char inputChar[2];
							inputChar[0] = wParam;
							inputChar[1] = 0;
							strcat(((InputBox*)wm.selectedElement)->input, inputChar);
							RECT r;
							render.CalcTextRect(guiRender.menuFont, ((InputBox*)wm.selectedElement)->input, &r);
							((InputBox*)wm.selectedElement)->right = r.right + 1;
						}
					}
				}
			}
			break;
			
		case WM_KEYUP:
		case WM_SYSKEYUP:
			break;

		case WM_LBUTTONDOWN:
			if (!menuOpen) {
				goto inputHandled;
			}
			if (wm.selectedWindow != NULL && wm.heldElement != NULL) {
				PElement pe;
				pe.e = wm.heldElement;
				if (pe.e->t == E_COMBOBOX) {
					Window *w = wm.selectedWindow;
					int sx = w->x + pe.e->x, sy = w->y + pe.e->y;
					if (CONTAINSMOUSE(sx, sy, pe.e->w, pe.e->h)) {
						if (pe.comboBox->state == 0) {
							pe.comboBox->state = 1;
							wm.heldElement     = pe.e;
							pe.e->h            = 4 + guiRender.menuFont->h * pe.comboBox->numEntries;
						} else {
							pe.comboBox->state = 0;
							pe.e->h            = 4 + guiRender.menuFont->h;
							for (size_t i = 0; i < ((ComboBox*)pe.e)->numEntries; i++) {
								if (CONTAINSMOUSE(sx, sy + i * guiRender.menuFont->h, pe.e->w, guiRender.menuFont->h)) {
									(*pe.comboBox->selected) = i;
									if (((ComboBox*)wm.heldElement)->onChange != NULL) {
										((ComboBox*)wm.heldElement)->onChange();
									}
								}
							}
							wm.heldElement = NULL;
						}
						goto inputHandled;
					} else {
						pe.comboBox->state = 0;
						pe.e->h            = 4 + guiRender.menuFont->h;
						wm.heldElement     = NULL;
					}
				}
			}

			for (auto it = wm.windows.begin(); it != wm.windows.end(); it++) {
				Window *w = *it;

				if (CONTAINSMOUSE(w->x, w->y, w->w, w->h)) {
					wm.selectedWindow = w;
					bool elementSelected = false;

					int sx = w->x + (w->w - 25), sy = w->y + 2;
					if (CONTAINSMOUSE(sx, sy, 20, 5)) {
						menuOpen = 0;
						goto inputHandled;
					}

					sx = w->x + (w->w - 50);
					if (CONTAINSMOUSE(sx, sy, 20, 5)) {
						if (w->state) {
							w->th    = w->h;
							w->h     = 25;
							w->state = 0;
						} else {
							w->h     = w->th;
							w->state = 1;
						}
						goto inputHandled;
					}

					for (auto it2 = w->subElements.begin(); it2 != w->subElements.end(); it2++) {
						TabContainer *e = (TabContainer*)*it2;
						for (auto it3 = e->subElements.begin(); it3 != e->subElements.end(); it3++) {
							Tab *t = (Tab*)*it3;
							if (CONTAINSMOUSE(w->x + t->x, w->y + t->y, t->w, t->h)) {
								e->selectedTab = t;
								goto inputHandled;
							} else {
								if (e->selectedTab == t) {
									for (auto it4 = t->subElements.begin(); it4 != t->subElements.end(); it4++) {
										PElement pe;
										pe.e = *it4;
										if (pe.e->enabled == 1) {
											int sx = w->x + pe.e->x, sy = w->y + pe.e->y;
											if (CONTAINSMOUSE(sx, sy, pe.e->w, pe.e->h)) {
												wm.selectedElement = pe.e;
												elementSelected    = true;
												typeSwitch:
												switch (pe.e->t) {
												case E_CHECKBOX:
													(*pe.cb->state) = (*pe.cb->state) ^ 1;
													break;

												case E_SLIDER:
													wm.heldElement    = pe.e;
													if (((Slider*)wm.heldElement)->valueType == V_FLOAT) {
														(*((Slider*)wm.heldElement)->fFrac) = ((Slider*)wm.heldElement)->fMin + ((Slider*)wm.heldElement)->scale * (((float)(mx - (wm.selectedWindow->x + wm.heldElement->x))) / (float)wm.heldElement->w);
														if ( (*((Slider*)wm.heldElement)->fFrac) < ((Slider*)wm.heldElement)->fMin) {
															(*((Slider*)wm.heldElement)->fFrac) = ((Slider*)wm.heldElement)->fMin;
														} else {
															if ((*((Slider*)wm.heldElement)->fFrac) > ((Slider*)wm.heldElement)->fMax) {
																(*((Slider*)wm.heldElement)->fFrac) = ((Slider*)wm.heldElement)->fMax;
															}
														}
													} else {
														(*((Slider*)wm.heldElement)->iFrac) = ((Slider*)wm.heldElement)->iMin + ((Slider*)wm.heldElement)->scale * (((float)(mx - (wm.selectedWindow->x + wm.heldElement->x))) / (float)wm.heldElement->w);
														if ( (*((Slider*)wm.heldElement)->iFrac) < ((Slider*)wm.heldElement)->iMin) {
															(*((Slider*)wm.heldElement)->iFrac) = ((Slider*)wm.heldElement)->iMin;
														} else {
															if ((*((Slider*)wm.heldElement)->iFrac) > ((Slider*)wm.heldElement)->iMax) {
																(*((Slider*)wm.heldElement)->iFrac) = ((Slider*)wm.heldElement)->iMax;
															}
														}
													}
													if (((Slider*)wm.heldElement)->onChange != NULL) {
														((Slider*)wm.heldElement)->onChange();
													}
													break;

												case E_COMBOBOX:
													if (pe.comboBox->state == 0) {
														pe.comboBox->state = 1;
														wm.heldElement     = pe.e;
														pe.e->h            = 4 + guiRender.menuFont->h * pe.comboBox->numEntries;
													} else {
														pe.comboBox->state = 0;
														pe.e->h            = 4 + guiRender.menuFont->h;
														wm.heldElement     = NULL;
														for (size_t i = 0; i < (pe.comboBox)->numEntries; i++) {
															if (CONTAINSMOUSE(sx, sy + i * guiRender.menuFont->h, pe.e->w, guiRender.menuFont->h)) {
																(*pe.comboBox->selected) = i;
																if ((pe.comboBox)->onChange != NULL) {
																	(pe.comboBox)->onChange();
																}
															}
														}
													}
													break;

												case E_INPUTBOX:
													break;

												case E_GROUPBOX:
													continue;
													break;

												case E_BUTTON:
													pe.button->onClick();
													break;

												case E_EXPANDBOX:
													if (CONTAINSMOUSE(sx, sy, 10 + pe.eb->tw, guiRender.menuFont->h + 2)) {
														if (pe.eb->state == 0) {
															pe.eb->state = 1;
															pe.eb->h     = pe.eb->th;
														} else {
															pe.eb->state = 0;
															pe.eb->h     = guiRender.menuFont->h + 4;
														}
													} else {
														if (pe.eb->state == 1 && pe.eb->subElements.size()) {
															for (auto it5 = pe.eb->subElements.begin(); it5 != pe.eb->subElements.end(); it5++) {
																ExpandBox *e5 = (ExpandBox*)*it5;
																sx = w->x + pe.eb->x + e5->x;
																sy = w->y + pe.eb->y + e5->y;
																if (CONTAINSMOUSE(sx, sy, e5->w, e5->h)) {
																	pe.e = (Element*)e5;
																	goto typeSwitch;
																}
															}
														}
													}
													break;
												}
												goto inputHandled;
											}
										}
									}
								}
							}
						}
					}
					if (CONTAINSMOUSE(w->x, w->y, w->w, 10 + guiRender.menuFont->h)) {
						wm.heldElement = w;
						wm.dx          = mx - w->x;
						wm.dy          = my - w->y;
						goto inputHandled;
					}
					if (!elementSelected) {
						wm.selectedElement = NULL;
					}
				}
			}
			inputHandled:
			break;

		case WM_LBUTTONUP:
			if (wm.heldElement != NULL) {
				if (wm.heldElement->t != E_COMBOBOX) {
					wm.heldElement = NULL;
				}
			}
			break;
		}

		return 1;
	}

	return 0;
}
