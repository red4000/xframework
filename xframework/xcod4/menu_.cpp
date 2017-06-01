#include <string.h>
#include <stdlib.h>
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
#include "xmenu.h"
#include "chams.h"

XMenu   menu;
Window *menuWindow;

FVector3 scaleMin(0,0,0), scaleMax(100.f, 100.f, 100.f);

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

void SetAimKey(char *text) {
}

void SetGlobalEnabled(XMenuEntry *e, void *arg) {

}

void InitializeMenu() {
	if (menu.window != NULL) {
		return;
	}

	menu.Initialize(10, 10, 300, 300, "DopeHack");
	menuWindow = menu.window;
	XMenuEntry *aimEntry = menu.CreateHeader("Aim");
	menu.CreateCheckbox("Enabled", &aimbot, aimEntry);
	menu.CreateCheckbox("Silent Aim", &silentAim, aimEntry);
	menu.CreateCheckbox("Target Lock", &targetLock, aimEntry);
	menu.CreateCheckbox("Aim at first target", &aimAtFirstTarget, aimEntry);
	XMenuEntry *afEntry = menu.CreateHeader("Auto Fire", aimEntry);
	menu.CreateCheckbox("Enabled", &autoFire, afEntry);
	menu.CreateSlider("Delay", &fireDelay, 0, 1000, afEntry);
	XMenuEntry *keyEntry = menu.CreateHeader("Key", aimEntry);
	menu.CreateCheckbox("Enabled", &aimKeyEnabled, keyEntry);
	menu.CreateKeyInput("Key", &aimKey, keyEntry);
	menu.CreateSlider("FOV X", &aimFOVX, 0.f, 360.f, afEntry);
	menu.CreateSlider("FOV Y", &aimFOVY, 0.f, 360.f, afEntry);
	menu.CreateSlider("Local Prediction", &localPrediction, 0.f, 2.f, afEntry);
	menu.CreateSlider("Sort", &aimSort, 0, NUM_SORTS, aimEntry);

	XMenuEntry *bonesEntry = menu.CreateHeader("Bones", aimEntry);

	for (int i = 0; i < MAX_TAG; i++) {
		char tmpName[32], *tmpStr, tmpStr2[16];

		if (tagName[i][0] == 'j') {
			strcpy_s(tmpName, sizeof(tmpName), &tagName[i][2]);
		} else if (tagName[i][0] == 't' && tagName[i][1] == 'a' && tagName[i][2] == 'g') {
			strcpy_s(tmpName, sizeof(tmpName), &tagName[i][4]);
		} else {
			strcpy_s(tmpName, sizeof(tmpName), tagName[i]);
		}
		tmpStr2[0] = tmpName[0];
		tmpStr2[1] = 0;
		tmpName[0] = strupr(tmpStr2)[0];

		tmpStr = (char*)calloc(1, strlen(tmpName) + 1);
		strcpy(tmpStr, tmpName);

		XMenuEntry *boneEntry = menu.CreateHeader(tmpStr, bonesEntry);
		BoneCfg *cfg = &boneCfg[i];

		menu.CreateCheckbox("Enabled", &cfg->enabled, boneEntry);
		menu.CreateCheckbox("Auto Wall", &cfg->autoWall, boneEntry);
		menu.CreateSlider("Penetration", &cfg->penetration, 0001.f, 1.f, boneEntry);
		menu.CreateSlider("Prediction", &cfg->prediction, 0.f, 2.f, boneEntry);
		menu.CreateSlider("Z Adjust", &cfg->zAdjust, -20.f, 20.f, boneEntry);
		menu.CreateSlider("Priority", &cfg->priority, 0, 20, boneEntry);
		//menu.CreateButton("Set Global Enabled", SetGlobalEnabled, bonesEntry)->userData = (void*)cfg->enabled;
		// TODO: buttons to set global values of all settings

		XMenuEntry *roEntry = menu.CreateHeader("Random Offset", boneEntry);
		menu.CreateSlider("Min Length", &cfg->randomOffset.minLength, 0.f, 50.f, roEntry);
		menu.CreateSlider("Max Length", &cfg->randomOffset.maxLength, 0.f, 50.f, roEntry);
		menu.CreateSlider("Velocity Fraction", &cfg->randomOffset.velocityFrac, 0.f, 1.f, roEntry);
		menu.CreateSlider("Direction Fraction", &cfg->randomOffset.directionFrac, 0.f, 1.f, roEntry);
		menu.CreateVector("Scale", &cfg->randomOffset.scale, &scaleMin, &scaleMax, roEntry);

		XMenuEntry *pointsEntry = menu.CreateHeader("Points", boneEntry);
		menu.CreateSlider("Min Length", &cfg->point.minLength, 0.f, 50.f, pointsEntry);
		menu.CreateSlider("Max Length", &cfg->point.maxLength, 0.f, 50.f, pointsEntry);
		menu.CreateSlider("Velocity Fraction", &cfg->point.velocityFrac, 0.f, 1.f, pointsEntry);
		menu.CreateSlider("Direction Fraction", &cfg->point.directionFrac, 0.f, 1.f, pointsEntry);
		menu.CreateVector("Scale", &cfg->point.scale, &scaleMin, &scaleMax, pointsEntry);
	}

	XMenuEntry *saEntry = menu.CreateHeader("Smooth Aim", aimEntry);
	menu.CreateSlider("Min Delay", &saMinDelay, 0, 500, saEntry);
	menu.CreateSlider("Max Delay", &saMaxDelay, 0, 500, saEntry);
	menu.CreateSlider("Min Initial Speed", &saMinInitialSpeed, 0.01f, 0.5f, saEntry);
	menu.CreateSlider("Max Initial Speed", &saMaxInitialSpeed, 0.02f, 0.5f, saEntry);
	menu.CreateSlider("Min Acceleration", &saMinAcceleration, 0.01f, 0.5f, saEntry);
	menu.CreateSlider("Max Acceleration", &saMaxAcceleration, 0.02f, 0.5f, saEntry);

	XMenuEntry *rcsEntry = menu.CreateHeader("RCS");
	menu.CreateCheckbox("Enabled", &rcsEnabled, rcsEntry);
	menu.CreateSlider("Delay", &rcsDelay, 0, 500, rcsEntry);
	menu.CreateSlider("Min Length", &rcsMinLength, 0.01f, 10.f, rcsEntry);
	menu.CreateSlider("Start Speed", &rcsStartSpeed, 0.01f, 1.f, rcsEntry);
	menu.CreateSlider("Acceleration", &rcsAccel, 0.01f, 1.f, rcsEntry);
	menu.CreateSlider("Max Speed", &rcsMaxSpeed, 0.01f, 2.5f, rcsEntry);

	XMenuEntry *espEntry = menu.CreateHeader("ESP");
	XMenuEntry *playerEspEntry = menu.CreateHeader("Players", espEntry);
	menu.CreateCheckbox("Box", &boxEsp, playerEspEntry);
	menu.CreateCheckbox("Boxhairs", &boxHairs, playerEspEntry);
	menu.CreateCheckbox("Box Edges", &boxEdges, playerEspEntry);
	menu.CreateSlider("Box Edge Frac", &boxEdgeFrac, 0.05f, 1.0f, playerEspEntry);
	//menu.CreateCheckbox("Barrel", &barrelEsp, playerEspEntry);
	XMenuEntry *ppixelEspEntry = menu.CreateHeader("Pixel", playerEspEntry);
	menu.CreateCheckbox("Enabled", &pixelEsp, ppixelEspEntry);
	menu.CreateSlider("Size", &pixelSize, 1, 100, ppixelEspEntry);
	menu.CreateCheckbox("Distance", &distanceEsp, playerEspEntry);
	menu.CreateCheckbox("Name", &nameEsp, playerEspEntry);
	menu.CreateCheckbox("Warning", &playerWarning, playerEspEntry);
	menu.CreateCheckbox("Weapon", &weaponEsp, playerEspEntry);
	menu.CreateCheckbox("Threat", &threatEsp, playerEspEntry);
	XMenuEntry *entityEspEntry = menu.CreateHeader("Entities", espEntry);

	XMenuEntry *chamsEntry = menu.CreateHeader("Chams");
	XMenuEntry *playersEntry = menu.CreateHeader("Players", chamsEntry);
	menu.CreateCheckbox("Enabled", &playerChams, playersEntry);
	menu.CreateCheckbox("Enemy Only", &onlyEnemyChams, playersEntry);
	menu.CreateSlider("Vis style", &visChamStyle, 0, 5, playersEntry);
	menu.CreateSlider("Invis style", &invisChamStyle, 0, 5, playersEntry);
	menu.CreateCheckbox("Wallhack", &wallHack, playersEntry);
	XMenuEntry *weaponsEntry = menu.CreateHeader("Weapons", chamsEntry);
	menu.CreateCheckbox("Enabled", &weaponChams, weaponsEntry);
	menu.CreateSlider("Vis style", &visWeaponChamStyle, 0, 5, weaponsEntry);
	menu.CreateSlider("Invis style", &invisWeaponChamStyle, 0, 5, weaponsEntry);

	XMenuEntry *radarEntry = menu.CreateHeader("Radar");
	menu.CreateCheckbox("Enabled", &radarEnabled, radarEntry);
	//menu.CreateCheckbox("Barrels", &radarBarrels, radarEntry);
	menu.CreateCheckbox("Players", &radarPlayers, radarEntry);
	menu.CreateCheckbox("Explosives", &radarExplosives, radarEntry);
	menu.CreateCheckbox("Items", &radarItems, radarEntry);
	menu.CreateSlider("Scale", &radarScalar, 1.0f, 10.f, radarEntry);

	XMenuEntry *removalsEntry = menu.CreateHeader("Removals");
	menu.CreateCheckbox("No Spread", &noSpread, removalsEntry);
	menu.CreateCheckbox("No Recoil", &noRecoil, removalsEntry);

	XMenuEntry *miscEntry = menu.CreateHeader("Misc");
	menu.CreateCheckbox("Crosshair", &crosshairEnabled, miscEntry);
	menu.CreateCheckbox("Clock", &clockEnabled, miscEntry);

	XMenuEntry *colorsEntry = menu.CreateHeader("Colors");

	XMenuEntry *espColorsEntry = menu.CreateHeader("ESP", colorsEntry);
	menu.CreateColor("Visible Enemy", &espEnemyVisCol, espColorsEntry);
	menu.CreateColor("Invisible Enemy", &espEnemyInvisCol, espColorsEntry);
	menu.CreateColor("Visible Friend", &espFriendlyVisCol, espColorsEntry);
	menu.CreateColor("Invisible Friend", &espFriendlyInvisCol, espColorsEntry);

	XMenuEntry *chamsColorsEntry = menu.CreateHeader("Chams", colorsEntry);
	menu.CreateColor("Visible Enemy", &enemyVisCol, chamsColorsEntry);
	menu.CreateColor("Invisible Enemy", &enemyInvisCol, chamsColorsEntry);
	menu.CreateColor("Visible Friend", &friendlyVisCol, chamsColorsEntry);
	menu.CreateColor("Invisible Friend", &friendlyInvisCol, chamsColorsEntry);

	XMenuEntry *guiColorsEntry = menu.CreateHeader("GUI", colorsEntry);
	menu.CreateColor("Outline", &outlineCol, guiColorsEntry);
	menu.CreateColor("Background", &backgroundCol, guiColorsEntry);
	menu.CreateColor("Foreground", &foregroundCol, guiColorsEntry);
	menu.CreateColor("Highlight", &highlightCol, guiColorsEntry);
	menu.CreateColor("Foretext", &foreTextCol, guiColorsEntry);
	menu.CreateColor("Backtext", &backTextCol, guiColorsEntry);
	menu.CreateColor("Ctrl Line", &ctrlLineCol, guiColorsEntry);
	menu.CreateColor("Highlight Ctrl Line", &hlctrlLineCol, guiColorsEntry);
	menu.CreateColor("Shadow", &shadowCol, guiColorsEntry);
	menu.CreateColor("Crosshair", &crosshairCol, guiColorsEntry);
	menu.CreateColor("Crosshair2", &crosshairCol2, guiColorsEntry);
}

void DestroyMenu() {
}

void MenuFrame() {
}
