#include "Precompiled.h"
#include "RutheniumAPI.h"
#include "BitmapFont.h"

void ruFontHandle::Free() {
	delete ((BitmapFont*)(pointer));
}