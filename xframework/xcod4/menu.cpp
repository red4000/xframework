#include "framework.h"
#include "game.h"
#include "aimbot.h"
#include "aim.h"
#include "prediction.h"
#include "autowall.h"
#include "esp.h"
#include "radar.h"
#include "nospread.h"
#include "crosshair.h"
#include "clock.h"
#include "warning.h"
#include "rcs.h"
#include "chams.h"

Window       *menuWindow;
TabContainer *menuTc;
Tab          *aimTab, *espTab, *rcsTab, *radarTab, *removalTab, *miscTab, *colorsTab;

float fireDelayFrac;

char *bones[] = {
	"Head",
	"Neck",
	"Helmet",
	"Eye",
	"Upper spine",
	"Lower spine",
	"Left shoulder",
	"Right shoulder",
	"Left hip",
	"Right hip",
	"Left elbow",
	"Right elbow",
	"Left knee",
	"Right knee",
	"Left ankle",
	"Right ankle",
	"Left wrist",
	"Right wrist",
	"Pelvis"
};
#define NUM_BONES 20

char *aimSorts[] {
	"Threat",
	"Distance",
	"FOV"
};
#define NUM_SORTS 3

char *colors[] = {
	"GUI outline",
	"GUI Background",
	"GUI Foreground",
	"GUI Highlight",
	"GUI Foretext",
	"GUI Backtext",
	"GUI Ctrl outline",
	"GUI Ctrl outline HL",
	"Visible Enemy",
	"Invisible Enemy",
	"Visible Friend",
	"Invisible Friend",
	"Crosshair",
	"Crosshair2"
};
#define NUM_COLORS 14

Tab *colorTab;
int selectedColor;
Slider *redSlider, *blueSlider, *greenSlider, *alphaSlider;
float redValue, blueValue, greenValue, alphaValue;

_Color *GetColorByIndex(int i) {
	switch (i) {
	default:
	case 0:
		return &outlineCol;
		break;

	case 1:
		return &backgroundCol;
		break;

	case 2:
		return &foregroundCol;
		break;

	case 3:
		return &highlightCol;
		break;

	case 4:
		return &foreTextCol;
		break;

	case 5:
		return &backTextCol;
		break;

	case 6:
		return &ctrlLineCol;
		break;

	case 7:
		return &hlctrlLineCol;
		break;

	case 8:
		return &shadowCol;
		break;

	case 9:
		return &enemyVisCol;
		break;

	case 10:
		return &enemyInvisCol;
		break;

	case 11:
		return &friendlyVisCol;
		break;

	case 12:
		return &friendlyInvisCol;
		break;

	case 13:
		return &crosshairCol;
		break;

	case 14:
		return &crosshairCol2;
		break;
	}
}

void UpdateColor() {
	_Color *c = GetColorByIndex(selectedColor);
	(*c)      = D3DCOLOR_RGBA((int)(redValue), (int)(greenValue), (int)(blueValue), (int)(alphaValue));
}

void UpdateSliders() {
	_Color *c  = GetColorByIndex(selectedColor);
	blueValue  = ((float)((*c) & 0xFF));
	greenValue = ((float)((((*c) & 0xFF00)) >> 8));
	redValue   = ((float)((((*c) & 0xFF0000) >> 16)));
	alphaValue = ((float)((((*c) & 0xFF000000)) >> 24));
}

void InitColorTab() {
	colorTab    = menuWindow->CreateTab(menuTc, "Color");
	redSlider   = menuWindow->CreateSlider(colorTab, "Red", 15, 70, 320, 0, &redValue, 0, 255, UpdateColor);
	greenSlider = menuWindow->CreateSlider(colorTab, "Green", 15, 100, 320, 0, &greenValue, 0, 255, UpdateColor);
	blueSlider  = menuWindow->CreateSlider(colorTab, "Blue", 15, 130, 320, 0, &blueValue, 0, 255, UpdateColor);
	alphaSlider = menuWindow->CreateSlider(colorTab, "Alpha", 15, 160, 320, 0, &alphaValue, 0, 255, UpdateColor);
	menuWindow->CreateComboBox(colorTab, "Color:", colors, NUM_COLORS, 15, 50, 150, 0, &selectedColor, UpdateSliders);
	UpdateSliders();
}

void SetAimKey(char *text) {
}

void InitializeMenu() {
	if (menuWindow != NULL) {
		return;
	}
	
	Window *w = wm.CreateNewWindow("XCoD4", 500, 320);
	w->enabled = 0;
	menuWindow = w;
	TabContainer *tc = w->CreateTabContainer(w, 10, 40, 480, 270);

	menuTc = tc;
	aimTab = w->CreateTab(tc, "Aim");
	//w->CreateGroupBox(aimTab, "TestBoxz", 15, 65, 150, 150);
	Element *e = w->CreateCheckBox(aimTab, "Auto Aim", 15, 45, &aimbot);
	int cy = e->y + e->h + 3;
	e = w->CreateCheckBox(aimTab, "Target Lock", 15, cy, &targetLock);
	cy = e->y + e->h + 3;
	e = w->CreateCheckBox(aimTab, "Silent Aim", 15, cy, &silentAim);
	cy = e->y + (e->h * 2) + 3;
	e = w->CreateCheckBox(aimTab, "Use Aim Key", 15, cy, &aimKey);
	cy = e->y + e->h + 3;
	e = w->CreateInputBox(aimTab, "Aim Key (0-255)", 15, cy, 50, 0, SetAimKey);
	e = w->CreateCheckBox(aimTab, "Auto Wall", 15, 168, &autoWall);
	//w->CreateGroupBox(aimTab, "Auto Fire", 302, 43, 126, 68);
	e = w->CreateCheckBox(aimTab, "Auto Fire", 134, 45, &autoFire);
	cy = e->y + e->h + 3;
	e = w->CreateSlider(aimTab, "AF Delay", 149, cy, 100, 0, &fireDelay, 0, 300);
	e = w->CreateCheckBox(aimTab, "Aim at first target", 245, 45, &aimAtFirstTarget);
	e = w->CreateSlider(aimTab, "Prediction", 134, 85, 300, 0, &predictionScalar, 0.f, 2.f);
	cy = e->y + e->h + 2;
	e = w->CreateSlider(aimTab, "Local Prediction", 134, cy, 300, 0, &localPrediction, 0.f, 2.f);
	cy = e->y + e->h + 2;
	e = w->CreateSlider(aimTab, "Z Adjust", 134, cy, 300, 0, &aimZAdjust, -3.f, 3.f);
	cy = e->y + e->h + 2;
	e = w->CreateSlider(aimTab, "FOV X", 134, cy, 300, 0, &aimFOVX, 0.1f, 360.f);
	cy = e->y + e->h + 2;
	e = w->CreateSlider(aimTab, "FOV Y", 134, cy, 300, 0, &aimFOVX, 0.1f, 360.f);
	cy = e->y + e->h + 2;
	e = w->CreateSlider(aimTab, "Auto Wall Power", 134, cy, 300, 0, &autoWallThreshold, 0.01f, 0.99f);
	//w->CreateGroupBox(aimTab, "Targetting", 127, 43, 176, 137);
	//w->CreateListView(aimTab, "Bone", 136, 61, 140, 100, bones, NUM_BONES);

	e = w->CreateCheckBox(aimTab, "Smooth Aim", 26, 186, &saEnabled);
	e = w->CreateSlider(aimTab, "SA Min Delay", 15, 201, 195, 0, &saMinDelay, 0, 500);
	e = w->CreateSlider(aimTab, "SA Max Delay", 255, 201, 195, 0, &saMaxDelay, 0, 500);
	cy = e->y + e->h + 2;
	e = w->CreateSlider(aimTab, "SA Min Initial Speed", 15, cy, 195, 0, &saMinInitialSpeed, 0.01f, 0.5f);
	e = w->CreateSlider(aimTab, "SA Max Initial Speed", 255, cy, 195, 0, &saMaxInitialSpeed, 0.02f, 0.5f);
	cy = e->y + e->h + 2;
	e = w->CreateSlider(aimTab, "SA Min Acceleration", 15, cy, 195, 0, &saMinAcceleration, 0.01f, 0.5f);
	e = w->CreateSlider(aimTab, "SA Max Acceleration", 255, cy, 195, 0, &saMaxAcceleration, 0.02f, 0.5f);
	cy = e->y + e->h + 5;
	e = w->CreateComboBox(aimTab, "Bone", bones, NUM_BONES, 15, cy, 125, 0, &aimBone);
	e = w->CreateComboBox(aimTab, "Sort", aimSorts, NUM_SORTS, 184, cy, 125, 0, &aimSort);

	rcsTab = w->CreateTab(tc, "RCS");
	e = w->CreateCheckBox(rcsTab, "Enabled", 35, 51, &rcsEnabled);
	cy = e->y + e->h + 3;
	e = w->CreateSlider(rcsTab, "Delay", 35, cy, 205, 0, &rcsDelay, 0, 400);
	cy = e->y + e->h + 3;
	e = w->CreateSlider(rcsTab, "Min Length", 35, cy, 205, 0, &rcsMinLength, 0.01f, 2.5f);
	cy = e->y + e->h + 3;
	e = w->CreateSlider(rcsTab, "Start Speed", 35, cy, 205, 0, &rcsStartSpeed, 0.01f, 1.0f);
	cy = e->y + e->h + 3;
	e = w->CreateSlider(rcsTab, "Acceleration", 35, cy, 205, 0, &rcsAccel, 0.01f, 1.0f);
	cy = e->y + e->h + 3;
	e = w->CreateSlider(rcsTab, "Max Speed", 35, cy, 205, 0, &rcsMaxSpeed, 0.01f, 2.5f);

	espTab = w->CreateTab(tc, "ESP");
	w->CreateGroupBox(espTab, "Player", 30, 51, 165, 150);
	e = w->CreateCheckBox(espTab, "Box", 35, 65, &boxEsp);
	cy = e->y + e->h + 3;
	e = w->CreateCheckBox(espTab, "Barrel", 35, cy, &barrelEsp);
	cy = e->y + e->h + 3;
	e = w->CreateCheckBox(espTab, "Distance", 35, cy, &distanceEsp);
	cy = e->y + e->h + 3;
	e = w->CreateCheckBox(espTab, "Enemies Only", 35, cy, &enemiesOnly);
	cy = e->y + e->h + 3;
	e = w->CreateCheckBox(espTab, "Name", 35, cy, &nameEsp);
	cy = e->y + e->h + 3;
	e = w->CreateCheckBox(espTab, "Warning", 35, cy, &playerWarning);
	cy = e->y + e->h + 3;
	e = w->CreateCheckBox(espTab, "Weapon", 35, cy, &weaponEsp);
	cy = e->y + e->h + 3;
	e = w->CreateCheckBox(espTab, "Threat", 35, cy, &threatEsp);
	cy = e->y + e->h + 3;

	w->CreateGroupBox(espTab, "Entities", 220, 51, 170, 120);
	e = w->CreateCheckBox(espTab, "Explosives", 225, 65, &explosiveEsp);
	cy = e->y + e->h + 3;
	e = w->CreateCheckBox(espTab, "Items", 225, cy, &itemEsp);

	radarTab = w->CreateTab(tc, "Radar");
	e = w->CreateCheckBox(radarTab, "Enabled", 35, 65, &radarEnabled);
	cy = e->y + e->h + 3;
	e = w->CreateCheckBox(radarTab, "Barrels", 35, cy, &radarBarrels);
	cy = e->y + e->h + 3;
	e = w->CreateCheckBox(radarTab, "Players", 35, cy, &radarPlayers);
	cy = e->y + e->h + 3;
	e = w->CreateCheckBox(radarTab, "Explosives", 35, cy, &radarExplosives);
	cy = e->y + e->h + 3;
	e = w->CreateCheckBox(radarTab, "Items", 35, cy, &radarItems);
	w->CreateSlider(radarTab, "Scale", 220, 65, 100, 0, &radarScalar, 1.0f, 10.f);

	removalTab = w->CreateTab(tc, "Removals");
	e = w->CreateCheckBox(removalTab, "No Spread", 35, 51, &noSpread);
	cy = e->y + e->h + 3;
	e = w->CreateCheckBox(removalTab, "No Recoil", 35, cy, &noRecoil);

	miscTab = w->CreateTab(tc, "Misc.");
	e = w->CreateCheckBox(miscTab, "Crosshair", 35, 51, &crosshairEnabled);
	cy = e->y + e->h + 3;
	e = w->CreateCheckBox(miscTab, "Clock", 35, cy, &clockEnabled);

	InitColorTab();
}

void DestroyMenu() {
}

void MenuFrame() {
 }
