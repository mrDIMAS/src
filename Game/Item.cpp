#include "Item.h"

vector<Item*> Item::Available;
Parser Item::loc;

Item * Item::GetByObject( NodeHandle obj ) {
    for( auto item : Available )
        if( item->object == obj ) {
            return item;
        }
    return 0;
}

void Item::SetType( int typ ) {
    this->type = typ;

    throwable = true;

    if( type == Detonator ) {
        desc = loc.GetString( "detonatorDesc" );
        name = loc.GetString( "detonatorName" );
        img = GetTexture( "data/gui/inventory/items/detonator.png" );;
        combinesWith = 0;
        onCombineBecomes = 0;

        content = loc.GetString( "detonatorContent" );
        contentType = loc.GetString( "detonatorContentType" );
    }

    if( type == FuelCanister ) {
        desc = loc.GetString( "fuelDesc" );
        name = loc.GetString( "fuelName" );
        img = GetTexture( "data/gui/inventory/items/fuel.png" );
        combinesWith = Flashlight;
        onCombineBecomes = Flashlight;

        content = loc.GetString( "fuelContent" );
        volume = content;

        contentType = loc.GetString( "fuelContentType" );
    }


    if( type == Wires ) {
        desc = "����� ��������. ������� ������ ������� � ��������� �������� ��� ������ �����. ��������� � ����������� � ������������ ���������.";
        name = "�������";
        img = GetTexture( "data/gui/inventory/items/wires.png" );
        combinesWith = 0;
        onCombineBecomes = 0;

        content = "����� ����������";
        contentType = "����";
    }

    if( type == Explosives ) {
        desc = "����� � ���������. ������� ���������� ���������� ��������. ��� ������� ��������� ���������.";
        name = "����� � ���������";
        img = GetTexture( "data/gui/inventory/items/ammonit.png" );
        combinesWith = 0;

        mass = "0.3";
        contentType = "�������";
    }

    if( type == Flashlight ) {
        desc = "������ ������� � ��������� ��������� �������. ������������ ��������� ��������/��������� ������� ��� ������( ������ ������ �� ����� ������� ).";
        name = "������ �������";
        img = GetTexture( "data/gui/inventory/items/flashlight.png" );
        combinesWith = FuelCanister;
        onCombineBecomes = Flashlight;

        throwable = false;
        mass = "1.3";
        volume = "0.6";
        contentType = "�����������";
    }

    if( type == Fuse ) {
        desc = "�������������� �� 1000 �����. ������ ������� �������, ������ ��������� ����� ��� ������ �� �������� ���������.";
        name = "��������������";
        img = GetTexture( "data/gui/inventory/items/fuse.png" );
        combinesWith = 0;

        mass = "6.3";
        contentType = "����";
    }

    if( type == Medkit ) {
        desc = "������� ������ ������. ������������ �� ����������� ��� �������� ���������� ����������� ������.";
        name = "�������";
        img = GetTexture( "data/gui/inventory/items/medkit.png" );
        combinesWith = 0;

        mass = "1.3";
        contentType = "�����������";
    }
}

Item::Item( NodeHandle obj, int typ ): InteractiveObject( obj ) {
    if( !loc.IsParsed() ) {
        loc.ParseFile( localizationPath + "items.loc" );
    }
    onCombineBecomes = 0;
    combinesWith = 0;
    Available.push_back( this );
    volume = Property( string( loc.GetString( "volume" )) + "\n", "-" );
    mass = Property( string( loc.GetString( "mass" )) + "\n", "-" );
    content = Property( string( loc.GetString( "content" )) + "\n", "-" );
    contentType = Property( string( loc.GetString( "contentType" ))+ "\n", "-" );
    inInventory = false;
    SetType( typ );
}


Item::Property::Property( string description, float theValue ) {
    desc = description;
    SetFloatValue( theValue );
}

Item::Property::Property( const Property & p ) {
    floatValue = p.floatValue;
    stringValue = p.stringValue;
    desc = p.desc;
    formatted = p.formatted;
}

Item::Property::Property() {
    floatValue = 0;

    Format();
}

Item::Property::Property( string description, string theValue ) {
    desc = description;
    SetstringValue( theValue );
}

void Item::Property::SetstringValue( string s ) {
    stringValue = s;
    floatValue = atof( s.c_str() );

    Format();
}

void Item::Property::SetFloatValue( float f ) {
    char buffer[ 64 ];
    sprintf( buffer, "%.1f", f );
    stringValue = buffer;
    floatValue = f;

    Format();
}

void Item::Property::Format() {
    formatted = desc + stringValue;
}
