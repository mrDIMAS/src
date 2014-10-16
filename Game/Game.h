#pragma once


#define _CRT_SECURE_NO_WARNINGS
#pragma warning( disable:4244 )
#pragma warning( disable:4305 )

#include "Engine.h"
#include <math.h>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <stdarg.h>
#include <fstream>
#include <vld.h>
#include <windows.h>

enum class FootstepsType
{
  Rock,
  Metal,
  Dirt
};

enum class LevelName
{
  LUndefined = 0,
  L0Introduction = 1,
  L1Arrival,
  L2Mine,
  L3ResearchFacility,

  // For testing
  LXTestingChamber,
};


static std::string Format( const char * format, ... )
{
  char buffer[ 1024 ];
  va_list	argumentList;

  va_start( argumentList, format );
  vsprintf_s( buffer, format , argumentList);
  va_end(argumentList);

  return string( buffer );
}
using namespace std;

#pragma comment( lib, "Ruthenium.lib" )

static float frandom( float low, float high )
{
  return low + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(high-low)));
}

extern float mouseSens;

class Item;
class Inventory;
class Level;
class LevelArrival;
class Player;
class Menu;
class GUI;
class ScreenScreamer;

void ParseFile( string fn, map<string,string> & values);
int CreateFog( int obj, int density = 3000 );

extern Player * player;
extern Menu * menu;
extern Level * currentLevel;
extern GUI * gui;
extern ScreenScreamer * screamer;
extern bool g_running;
extern string localizationPath;
extern bool g_showFPS;
extern LevelName g_initialLevel;
extern bool g_continueGame;
extern int g_keyQuickSave;
extern int g_keyQuickLoad;


