#ifndef XGUIRENDER_H
#define XGUIRENDER_H

#define OUTLINETHICKNESS  1
#define CTRLLINETHICKNESS 1
#define SHADOWSIZE        10
#define CURSORFLASHTIME   300
#define CURSORTHICKNESS   1
#define MAXINPUTSIZE      256
#define FADETIME          350
#define MINTABW           40
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define CONTAINSMOUSE(x, y, w, h) (mx >= (x) && mx <= ((x) + (w)) && my >= (y) && my <= ((y) + (h)))

class GUIRenderer {
public:
	GUIRenderer();
	~GUIRenderer();

	void Initialize();
	void Destroy();
	void SetElementSize(int type, Element *e);
	void RenderCheckBox(int sx, int sy, CheckBox *cb);
	void RenderSlider(int sx, int sy, Slider *cb);
	void RenderComboBox(int sx, int sy, ComboBox *cb);
	void RenderInputBox(int sx, int sy, InputBox *cb);
	void RenderPreciseSlider(int sx, int sy, PreciseSlider *cb);
	void RenderButton(int sx, int sy, Button *cb);
	void RenderTextBox(int sx, int sy, TextBox *cb);
	void RenderListView(int sx, int sy, ListView *lv);
	void RenderTreeView(int sx, int sy, TreeView *tv);
	void RenderTab(int sx, int sy, Tab *t, TabContainer *tc);
	void RenderTabContainer(int sx, int sy, TabContainer *tc);
	void RenderLabel(int sx, int sy, Label *l);
	void RenderGroupBox(int sx, int sy, GroupBox *gb);
	void RenderExpandBox(int sx, int sy, ExpandBox *eb);
	void RenderWindow(Window *w);
	void RenderCursor(int cx, int cy);
	int  GetTitlebarHeight();

	_Color lineColor, textColor;
	Font  *menuFont, *bigMenuFont;
};

extern GUIRenderer guiRender;
extern int cdx1, cdy1, cdx2, cdy2, cdx3, cdy3;

#endif
