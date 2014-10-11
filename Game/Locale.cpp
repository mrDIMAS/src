#include "Locale.h"




void Locale::LoadLocalizationFromFile( string fn )
{
  ParseFile( fn, loc );
}
