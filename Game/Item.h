#pragma  once

#include "Game.h"
#include "Parser.h"
#include "InteractiveObject.h"

class Item : public InteractiveObject {
public:
    static Parser loc;
    class Property {
    private:
        void Format();

        void SetFloatValue( float f );

        void SetstringValue( string s );
        string stringValue;
        float floatValue;
        string formatted;


    public:
        string desc;

        Property( string description, string theValue );

        Property( );

        Property( const Property & p );

        Property( string description, float theValue );


        operator float() const {
            return floatValue;
        }

        operator const char * () const {
            return formatted.c_str();
        }

        void operator=( const string & s ) {
            SetstringValue( s );
        }

        void operator=( const float & p ) {
            SetFloatValue( p );
        }
    };

    ruTextureHandle img;
    string desc;
    string name;
    int type;
    int combinesWith;
    int onCombineBecomes;
    bool throwable;
    bool inInventory;

    Property volume;
    Property mass;
    Property content;
    Property contentType;

    enum {
        Detonator = 1,
        FuelCanister,
        Wires,
        Explosives,
        Flashlight,
        Fuse,
        Medkit,
    };

    Item( ruNodeHandle obj, int typ );
    void SetType( int typ );
    static vector<Item*> Available;
    static Item * GetByObject( ruNodeHandle obj );
};

