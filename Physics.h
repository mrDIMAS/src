#pragma once

#include "Common.h"

// physic world can be only one, so all members are static
class Physics
{
public:
    static void CreateWorld();
    static void DestructWorld();
};