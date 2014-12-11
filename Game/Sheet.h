#pragma once

#include "InteractiveObject.h"

class Sheet : public InteractiveObject
{
private:
	string mText;
	string mDescription;
	ruTextureHandle mBackgroundTexture;
public:
    explicit Sheet( ruNodeHandle object, string desc, string text );
	virtual ~Sheet();
    void Update();
    void Draw( );
	void SetText( const char * text );
	const char * GetText( ) const;
	void SetDescription( const char * description );
	const char * GetDescription( ) const;
    static Sheet * GetSheetByObject( ruNodeHandle o );
    static vector<Sheet*> msSheetList;
    static ruSoundHandle msPaperFlipSound;
    static ruFontHandle msSheetFont;
};