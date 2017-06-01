#ifndef XGUI_H
#define XGUI_H

#include "_vector.h"

enum {
	E_CHECKBOX,
	E_SLIDER,
	E_COMBOBOX,
	E_INPUTBOX,
	E_PRECISESLIDER,
	E_BUTTON,
	E_TEXTBOX,
	E_TREEVIEW,
	E_LISTVIEW,
	E_TAB,
	E_TABCONTAINER,
	E_LABEL,
	E_GROUPBOX,
	E_EXPANDBOX,
	E_WINDOW
};

enum {
	V_FLOAT,
	V_INT
};

class Element {
public:
	int               x, y, w, h, t, enabled;
	char             *text;
	_Vector<Element*> subElements;
	DWORD             hoverTime;

	void RemoveSubElement(Element *e);

	inline void Hide() { enabled = 0; }
	inline void Show() { enabled = 1; }
};

class CheckBox : public Element {
public:
	int *state;
};

typedef void (*ElementChangedCallback)();

class Slider : public Element {
public:
	int                    valueType;
	union {
		float             *fFrac;
		int               *iFrac;
	};
	union {
		float              fMin;
		int                iMin;
	};
	union {
		float              fMax;
		int                iMax;
	};
	double                 scale;
	ElementChangedCallback onChange;
};

class ComboBox : public Element {
public:
	char **entries;
	int    state, numEntries, *selected, xo;
	ElementChangedCallback onChange;
};

typedef void (*InputBoxCallback)(char *text);

class InputBox : public Element {
public:
	InputBoxCallback onEnter;
	DWORD            cursorTime, right, flash;
	char            *input;
};

class PreciseSlider : public Element {
public:
	float                 *frac, min, max;
	ElementChangedCallback onChange;
	InputBox              *ib;
};

typedef void (*ButtonCallback)();

class Button : public Element {
public:
	ButtonCallback onClick;
};

class TextBox : public Element {
};

class TreeView : public Element {
};

class ListView : public Element {
public:
	_Vector<char*> items;
	int sh, sy, si, selectedItem;
};

class Tab : public Element {
public:
	int tx;
};

class TabContainer : public Element {
public:
	TabContainer();

	void Render();
	int  xo;
	Tab *selectedTab;
};

class Label : public Element {
};

class GroupBox : public Element {
public:
	int tw;
};

class ExpandBox : public Element { // collapsible groupbox
public:
	int th, tw, state;
};

union PElement {
	Element      *e;
	CheckBox     *cb;
	Slider       *s;
	ComboBox     *comboBox;
	InputBox     *inputBox;
	Button       *button;
	TextBox      *textBox;
	TreeView     *treeView;
	ListView     *listView;
	Tab          *tab;
	TabContainer *tc;
	Label        *label;
	GroupBox     *gb;
	ExpandBox    *eb;
};

class Window : public Element {
public:
	CheckBox      *CreateCheckBox(Element *parent, char *text, int x, int y, int *state);
	Slider        *CreateSlider(Element *p, char *text, int x, int y, int w, int h, float *frac, float min, float max, ElementChangedCallback onChange = NULL);
	Slider        *CreateSlider(Element *p, char *text, int x, int y, int w, int h, int *frac, int min, int max, ElementChangedCallback onChange = NULL);
	PreciseSlider *CreatePreciseSlider(Element *p, char *text, int x, int y, int w, int h, float *frac, float min, float max, ElementChangedCallback onChange = NULL);
	ComboBox      *CreateComboBox(Element *p, char *text, char **entries, int numEntries, int x, int y, int w, int h, int *selectedIndex, ElementChangedCallback onChange = NULL);
	InputBox      *CreateInputBox(Element *p, char *text, int x, int y, int w, int h, InputBoxCallback onEnter);
	Button        *CreateButton(Element *p, char *text, int x, int y, int w, int h, ButtonCallback onClick);
	TextBox       *CreateTextBox(Element *p, char *text, int x, int y, int w, int h);
	//CreateTreeView
	ListView      *CreateListView(Element *p, char *text, int x, int y, int w, int h, char **items, int numItems);
	Tab           *CreateTab(TabContainer *container, char *text);
	TabContainer  *CreateTabContainer(Element *parent, int x, int y, int w, int h);
	Label         *CreateLabel(Element *p, char *text, int x, int y);
	GroupBox      *CreateGroupBox(Element *p, char *text, int x, int y, int w, int h);
	ExpandBox     *CreateExpandBox(Element *p, char *text, int x, int y, int w, int h);

	void Render();

	int state, th, renderWindow;
private:
	void RenderElement(Element *e, TabContainer *tc, DWORD currentTime);
};

class WindowManager {
public:
	void Initialize();
	void Release();

	Window *CreateNewWindow(char *title, int w, int h);
	void    Render();

	_Vector<Window*> windows;
	Window          *selectedWindow;
	Element         *heldElement, *selectedElement; // TODO: move to Window class
	int              dx, dy;
};

LRESULT CALLBACK HandleGUIInput(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

extern WindowManager wm;
extern _Color        outlineCol, backgroundCol, foregroundCol, highlightCol, foreTextCol, backTextCol, ctrlLineCol, hlctrlLineCol, shadowCol;
extern int           menuOpen, mx, my;
extern Font         *menuFont;

#endif
