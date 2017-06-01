#include "framework.h"
#include "game.h"
#include "aim.h"
#include "esp.h"
#include "clock.h"
#include "crosshair.h"
#include "dodge.h"
#include "radar.h"
#include "rcs.h"
#include "sesp.h"
#include "warning.h"

CVar aimHeader("Aim"),
	 c1("Smooth Aim", &aimHeader),
	 c2("Enabled", 0, 0, 1, &saEnabled, &c1),
	 c3("Min Delay", 0, 0, 500, &saMinDelay, &c1),
	 c4("Max Delay", 0, 0, 500, &saMaxDelay, &c1),
	 c5("Min Initial Speed", 0, 0.001f, 10.0f, &saMinInitialSpeed, &c1),
	 c6("Max Initial Speed", 0, 0.002f, 10.0f, &saMaxInitialSpeed, &c1),
	 c7("Max Speed", 0, 0.002f, 10.0f, &saMaxSpeed, &c1),
	 c8("Min Initial Acceleration", 0, 0.001f, 10.0f, &saMinAcceleration, &c1),
	 c9("Max Initial Acceleration", 0, 0.002f, 10.0f, &saMaxAcceleration, &c1),

	 rcsHeader("RCS"),
	 c52("Enabled", 0, 0, 1, &rcsEnabled, &rcsHeader),
	 c53("Delay", 0, 0, 500, &rcsDelay, &rcsHeader),
	 c48("Min Length", 0, 0.001f, 10.0f, &rcsMinLength, &rcsHeader),
	 c49("Start Speed", 0, 0.001f, 10.0f, &rcsStartSpeed, &rcsHeader),
	 c50("Acceleration", 0, 0.001f, 10.0f, &rcsAccel, &rcsHeader),
	 c51("Max Speed", 0, 0.001f, 10.0f, &rcsMaxSpeed, &rcsHeader),

	 espHeader("ESP"),
	 c10("Players", &espHeader),
	 //c11("Barrel", 0, 0, 1, &barrelEsp, &c10),
	 c12("Box", 0, 0, 1, &boxEsp, &c10),
	 c13("Distance", 0, 0, 1, &distanceEsp, &c10),
	 c14("Enemies Only", 0, 0, 1, &enemiesOnly, &c10),
	 c15("Name", 0, 0, 1, &nameEsp, &c10),
	 c46("Warning", 0, 0, 1, &playerWarning, &c10),
	 c47("Warning Threshold", 0, 0.1f, 300.f, &warningThreshold, &c10),
	 c16("Weapon", 0, 0, 1, &weaponEsp, &c10),
	 c17("Threat", 0, 0, 1, &threatEsp, &c10),

	 c54("Sound ESP", &c10),
	 c55("Enabled", 0, 0, 1, &soundEsp, &c54),
	 c56("Delay", 0, 0, 500, &soundEspDelay, &c54),
	 c57("FOV", 0, 0.01f, 360.f, &soundEspFOV, &c54),

	 /*
	 c18("Style", 0, 0, 1, &espStyle, &c10),
	 c18("Entities", &espHeader),
	 c19("Explosives", 0, 0, 1, &explosiveEsp, &c18),
	 c20("Weapons", 0, 0, 1, &weaponEsp, &c18),
	 */

	 radarHeader("Radar"),
	 c21("Enabled", 0, 0, 1, &radarEnabled, &radarHeader),
	 //c22("Barrel", 0, 0, 1, &radarBarrels, &radarHeader),
	 //c22("Barrel Length", 0, 3.0f, 100.f, &radarBarrelLength, &radarHeader),
	 c22("Players", 0, 0, 1, &radarPlayers, &radarHeader),
	 //c22("Explosives", 0, 0, 1, &radarExplosives, &radarHeader),
	 //c23("Items", 0, 0, 1, &radarItems, &radarHeader),
	 c23("X", 0, 0, 5000, &radarX, &radarHeader),
	 c24("Y", 0, 0, 5000, &radarY, &radarHeader),
	 c25("W", 0, 0, 500, &radarW, &radarHeader),
	 c26("H", 0, 0, 500, &radarH, &radarHeader),

	 miscHeader("Misc"),
	 c29("Clock", 0, 0, 1, &clockEnabled, &miscHeader),
	 c30("Crosshair", 0, 0, 1, &crosshairEnabled, &miscHeader),

	 colorHeader("Color"),
	 c31("GUI Outline", 0, (unsigned int*)&outlineCol, &colorHeader),
	 c32("GUI Background", 0, (unsigned int*)&backgroundCol, &colorHeader),
	 c33("GUI Foreground", 0, (unsigned int*)&foregroundCol, &colorHeader),
	 c34("GUI Highlight", 0, (unsigned int*)&highlightCol, &colorHeader),
	 c35("GUI Foretext", 0, (unsigned int*)&foreTextCol, &colorHeader),
	 c36("GUI Backtext", 0, (unsigned int*)&backTextCol, &colorHeader),
	 c37("GUI Ctrl Line", 0, (unsigned int*)&ctrlLineCol, &colorHeader),
	 c38("GUI Highlight Ctrl Line", 0, (unsigned int*)&hlctrlLineCol, &colorHeader),
	 c39("GUI Shadow", 0, (unsigned int*)&shadowCol, &colorHeader),
	 c40("Visible Enemy", 0, (unsigned int*)&espEnemyVisCol, &colorHeader),
	 c41("Invisible Enemy", 0, (unsigned int*)&espEnemyInvisCol, &colorHeader),
	 c42("Visible Friend", 0, (unsigned int*)&espFriendlyVisCol, &colorHeader),
	 c43("Invisible Friend", 0, (unsigned int*)&espFriendlyInvisCol, &colorHeader),
	 c44("Crosshair", 0, (unsigned int*)&crosshairCol, &colorHeader),
	 c45("Crosshair2", 0, (unsigned int*)&crosshairCol2, &colorHeader);
	// c31("")
