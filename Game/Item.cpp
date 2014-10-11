#include "Item.h"

vector<Item*> Item::Available;
Locale Item::lang;

Item * Item::GetByObject( NodeHandle obj )
{
  for( auto item : Available )
    if( item->object == obj )
      return item;
  return 0;
}

void Item::SetType( int typ )
{
  this->type = typ;

  throwable = true;

  if( type == Detonator )
  {
    desc = lang.loc[ "detonatorDesc" ];
    name = lang.loc[ "detonatorName" ];
    img = GetTexture( "data/gui/inventory/items/detonator.png" );;
    combinesWith = 0;
    onCombineBecomes = 0;

    content = lang.loc[ "detonatorContent" ];
    contentType = lang.loc[ "detonatorContentType" ];
  }

  if( type == FuelCanister )
  {
    desc = lang.loc[ "fuelDesc" ];
    name = lang.loc[ "fuelName" ];
    img = GetTexture( "data/gui/inventory/items/fuel.png" );
    combinesWith = Flashlight;
    onCombineBecomes = Flashlight;

    content = lang.loc[ "fuelContent" ];
    volume = content;

    contentType = lang.loc[ "fuelContentType" ];
  }


  if( type == Wires )
  {
    desc = "Моток проводов. Обычные медные провода в резиновой изоляции для горных работ. Устойчивы к перетиранию и механическим нагрузкам.";
    name = "Провода";
    img = GetTexture( "data/gui/inventory/items/wires.png" );
    combinesWith = 0;
    onCombineBecomes = 0;

    content = "Около килограмма";
    contentType = "Медь";
  }

  if( type == Explosives )
  {
    desc = "Шашка с аммонитом. Хорошее бризантное взрывчатое вещество. Для подрыва требуется детонатор.";
    name = "Шашка с аммонитом";
    img = GetTexture( "data/gui/inventory/items/ammonit.png" );
    combinesWith = 0;

    mass = "0.3";
    contentType = "Аммонит";      
  }

  if( type == Flashlight )
  {
    desc = "Мощный фонарик с топливным элементом питания. Заправляется безводным этиловым/метиловым спиртом или водкой( светит меньше на одной заливке ).";
    name = "Мощный фонарик";
    img = GetTexture( "data/gui/inventory/items/flashlight.png" );
    combinesWith = FuelCanister;
    onCombineBecomes = Flashlight;

    throwable = false;
    mass = "1.3";
    volume = "0.6";
    contentType = "Электроника";      
  } 

  if( type == Fuse )
  {
    desc = "Предохранитель на 1000 Ампер. Мощная плавкая вставка, служит последней мерой для защиты от коротких замыканий.";
    name = "Предохранитель";
    img = GetTexture( "data/gui/inventory/items/fuse.png" );
    combinesWith = 0;

    mass = "6.3";
    contentType = "Медь";      
  } 

  if( type == Medkit )
  {
    desc = "Аптечка первой помощи. Используется на предприятих для оказания неотложной медицинской помощи.";
    name = "Аптечка";
    img = GetTexture( "data/gui/inventory/items/medkit.png" );
    combinesWith = 0;

    mass = "1.3";
    contentType = "Медикаменты";      
  }
}

Item::Item( NodeHandle obj, int typ )
{
  if( !lang.loc.size() )
    lang.LoadLocalizationFromFile( localizationPath + "items.loc" );

  object = obj;     
  onCombineBecomes = 0;
  combinesWith = 0;   

  Available.push_back( this );

  volume = Property( lang.loc["volume"] + "\n", "-" );
  mass = Property( lang.loc["mass"] + "\n", "-" );
  content = Property( lang.loc["content"] + "\n", "-" );
  contentType = Property( lang.loc["contentType"] + "\n", "-" );

  inInventory = false;

  SetType( typ );
}


Item::Property::Property( string description, float theValue )
{
  desc = description;
  SetFloatValue( theValue );
}

Item::Property::Property( const Property & p )
{
  floatValue = p.floatValue;
  stringValue = p.stringValue;
  desc = p.desc;
  formatted = p.formatted;
}

Item::Property::Property()
{
  floatValue = 0;

  Format();
}

Item::Property::Property( string description, string theValue )
{
  desc = description;
  SetStringValue( theValue );
}

void Item::Property::SetStringValue( string s )
{
  stringValue = s;
  floatValue = atof( s.c_str() );

  Format();
}

void Item::Property::SetFloatValue( float f )
{
  char buffer[ 64 ];
  sprintf( buffer, "%.1f", f );
  stringValue = buffer;
  floatValue = f;

  Format();
}

void Item::Property::Format()
{
  formatted = desc + stringValue;
}
