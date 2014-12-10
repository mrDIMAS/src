#pragma once

#include "Game.h"

static const char * keyNames [] =
{
    "Esc",
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    "0",
    "Minus",
    "Equals",
    "Backspace",
    "Tab",
    "Q",
    "W",
    "E",
    "R",
    "T",
    "Y",
    "U",
    "I",
    "O",
    "P",
    "LeftBracket",
    "RightBracket",
    "Enter",
    "LeftControl",
    "A",
    "S",
    "D",
    "F",
    "G",
    "H",
    "J",
    "K",
    "L",
    "Semicolon",
    "Apostrophe",
    "Grave",
    "LeftShift",
    "BackSlash",
    "Z",
    "X",
    "C",
    "V",
    "B",
    "N",
    "M",
    "Comma",
    "Period",
    "Slash",
    "RightShift",
    "Multiply",
    "LeftAlt",
    "Space",
    "Capital",
    "F1",
    "F2",
    "F3",
    "F4",
    "F5",
    "F6",
    "F7",
    "F8",
    "F9",
    "F10",
    "NumLock",
    "Scroll",
    "NumPad7",
    "NumPad8",
    "NumPad9",
    "SUBTRACT",
    "NUMPAD4",
    "NUMPAD5",
    "NUMPAD6",
    "ADD",
    "NUMPAD1",
    "NUMPAD2",
    "NUMPAD3",
    "NUMPAD0",
    "Decimal",
    "OEM_102",
    "F11",
    "F12",
    "F13",
    "F14",
    "F15",
    "KANA",
    "ABNT_C1",
    "CONVERT",
    "NOCONVERT",
    "YEN",
    "ABNT_C2",
    "NUMPADEQUALS",
    "PREVTRACK",
    "AT",
    "COLON",
    "UNDERLINE",
    "KANJI",
    "STOP",
    "AX",
    "UNLABELED",
    "NEXTTRACK",
    "NUMPADENTER",
    "RCONTROL",
    "MUTE",
    "CALCULATOR",
    "PLAYPAUSE",
    "MEDIASTOP",
    "VOLUMEDOWN",
    "VOLUMEUP",
    "WEBHOME",
    "NUMPADCOMMA",
    "DIVIDE",
    "SYSRQ",
    "RMENU",
    "PAUSE",
    "HOME",
    "UP",
    "PRIOR",
    "LEFT",
    "RIGHT",
    "END",
    "DOWN",
    "NEXT",
    "INSERT",
    "DEL",
    "LWIN",
    "RWIN",
    "APPS",
    "POWER",
    "SLEEP",
    "WAKE",
    "WEBSEARCH",
    "WEBFAVORITES",
    "WEBREFRESH",
    "WEBSTOP",
    "WEBFORWARD",
    "WEBBACK",
    "MYCOMPUTER",
    "MAIL",
    "MEDIASELECT"
};

static const char * GetKeyName( unsigned int vk )
{
    if( vk < 0 )
        return " ";
    if( vk > 127 )
        return " ";
    return keyNames[ vk - 1 ];
}

static bool FileExist( const char * file )
{
    fstream f( file );

    bool state = f.good();

    f.close();

    return state;
}

static void GetFilesWithDefExt( string folder, vector< string > & names )
{
    WIN32_FIND_DATAA fd;
    HANDLE hFind = FindFirstFileA( folder.c_str(), &fd);
    if(hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            if( !(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
                names.push_back(fd.cFileName);
        }
        while( FindNextFileA(hFind, &fd));
        FindClose(hFind);
    }
}