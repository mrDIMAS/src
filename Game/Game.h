#pragma once


#define _CRT_SECURE_NO_WARNINGS
#pragma warning( disable:4244 )
#pragma warning( disable:4305 )

#include <string>
#include "Engine.h"
#include <math.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <stdarg.h>
#include <fstream>
#include <unordered_map>
/*#include <vld.h>*/
#include <windows.h>
#include <assert.h>

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef _DEBUG
#pragma comment( lib, "Ruthenium_d.lib" )
#else
#pragma comment( lib, "Ruthenium.lib" )
#endif 

enum class FootstepsType {
    Rock,
    Metal,
    Dirt
};

enum LevelName {
    LUndefined = 0,
    L0Introduction = 1,
    L1Arrival,
    L2Mine,
    L3ResearchFacility,

    // For testing
    LXTestingChamber,
};

using namespace std;
void RaiseError( const string & text );

extern float mouseSens;

class Item;
class Inventory;
class Level;
class LevelArrival;
class Player;
class Menu;
class GUI;
class ScreenScreamer;

int CreateFog( int obj, int density = 3000 );

extern Player * pPlayer;
extern Menu * pMainMenu;
extern Level * pCurrentLevel;
extern GUI * pGUI;
extern ScreenScreamer * screamer;
extern bool g_running;
extern string localizationPath;
extern bool g_showFPS;
extern int g_initialLevel;
extern bool g_continueGame;
extern int g_keyQuickSave;
extern int g_keyQuickLoad;
extern float g_resW;
extern float g_resH;
extern double g_dt;
extern float g_musicVolume;


