#include "Tip.h"

void Tip::Deserialize( TextFileStream & in )
{
    in.Readstring( txt );
    alpha.Deserialize( in );
    in.ReadInteger( x );
    y.Deserialize( in );
    in.ReadInteger( w );
    in.ReadInteger( h );
}

void Tip::Serialize( TextFileStream & out )
{
    out.Writestring( txt );
    alpha.Serialize( out );
    out.WriteInteger( x );
    y.Serialize( out );
    out.WriteInteger( w );
    out.WriteInteger( h );
}

void Tip::AnimateAndDraw()
{
    ruDrawGUIText( txt.c_str(), x, y, w, h, gui->font, ruVector3( 255, 0, 0 ), 1, alpha );

    if( ruGetElapsedTimeInSeconds( timer ) > 1.5 ) {
        alpha.SetTarget( alpha.GetMin() );
        alpha.ChaseTarget( 0.1 );

        y.SetTarget( y.GetMax() );
        y.ChaseTarget( 0.1 );
    };
}

void Tip::SetNewText( string text )
{
    ruRestartTimer( timer );
    txt = text;
    alpha.Set( 255.0f );
    y.SetMax( ruGetResolutionHeight() - h );
    y.SetMin( ruGetResolutionHeight() / 2 + h );
    y.Set( y.GetMin());
}

Tip::Tip() : alpha( 255.0f, 0.0f, 255.0f )
{
    timer = ruCreateTimer();
    w = 256;
    h = 32;
    x = ruGetResolutionWidth() / 2 - w / 2;
    SetNewText( " " );
}
