#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES

#pragma warning( disable:4244 )
#pragma warning( disable:4305 )
#pragma warning( disable:4018 )
#pragma warning( disable:4800 )

#ifdef _DEBUG
#	pragma comment( lib, "Ruthenium_d.lib" )
#else
#	pragma comment( lib, "Ruthenium.lib" )
#endif 

#include <string>
#include "RutheniumAPI.h"
#include <math.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <stdarg.h>
#include <fstream>
#include <unordered_map>
#include <windows.h>
#include <assert.h>
#include <sstream>
#include <regex>
#include <math.h>
 
using namespace std;