#pragma once

#include "Texture.h"

class Material
{
public:
  float opacity;
  Vector3 diffuseColor;
  Vector3 specularColor;
  Texture * diffuseTexture;
  Texture * normalTexture;
public:
  Material()
  {
    opacity = 1.0f;
    diffuseTexture = 0;
    normalTexture = 0;
    diffuseColor = Vector3( 255.0f, 255.0f, 255.0f );
    specularColor = Vector3( 255.0f, 255.0f, 255.0f );
  }

  // deferred renderer can't render transparent faces
  bool IsRendererDeferred()
  {
    return opacity > 0.995f;
  }
};