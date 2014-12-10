#pragma  once

#include "Game.h"
#include "Parser.h"
#include "InteractiveObject.h"

class Item : public InteractiveObject
{
public:
    static Parser msLoc;
    class Property
    {
    private:
        void Format();

        void SetFloatValue( float f );

        void SetstringValue( string s );
        string mStringValue;
        float mFloatValue;
        string mFormatted;


    public:
        string mDesc;

        Property( string description, string theValue );

        Property( );

        Property( const Property & p );

        Property( string description, float theValue );


        operator float() const
        {
            return mFloatValue;
        }

        operator const char * () const
        {
            return mFormatted.c_str();
        }

        void operator=( const string & s )
        {
            SetstringValue( s );
        }

        void operator=( const float & p )
        {
            SetFloatValue( p );
        }
    };

    ruTextureHandle mPic;
    string mDesc;
    string mName;
    int mType;
    int mCombineType;
    int mOnCombineBecomes;
    bool mThrowable;
    bool mInInventory;

    Property mVolume;
    Property mMass;
    Property mContent;
    Property mContentType;

    enum
    {
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

