#pragma once

#include "EffectsQuad.h"
#include "GBuffer.h"
#include "Utility.h"
#include "Camera.h"

class SSAO
{
public:
  PixelShader * aoPixelShader;
  PixelShader * blurPixelShader;

  IDirect3DTexture9 * rotateTexture;

  IDirect3DTexture9 * aoTexture;
  IDirect3DSurface9 * aoRenderTarget;

  IDirect3DTexture9 * blurTexture;
  IDirect3DSurface9 * blurRenderTarget;

  D3DXHANDLE aoScreenWidth;
  D3DXHANDLE aoScreenHeight;

  D3DXHANDLE aoInvViewProj;

  D3DXHANDLE aoRotateWidth;
  D3DXHANDLE aoRotateHeight;

  D3DXHANDLE aoRadius;
  D3DXHANDLE aoDistScale;
  D3DXHANDLE aoBias;

  D3DXHANDLE blurScreenWidth;
  D3DXHANDLE blurScreenHeight;

  /*
  "float ssao( float3x3 kernelBasis, float3 originPos, float radius) \n"
    "{\n"
    "   float occlusion = 0.0;\n"
    "   for (int i = 0; i < uKernelSize; ++i) \n"
    "   {\n"

    "     vec3 samplePos = kernelBasis * uKernelOffsets[i];\n"
    "     samplePos = samplePos * radius + originPos;\n"

    "     vec4 offset = uProjectionMatrix * vec4(samplePos, 1.0);\n"
    "     offset.xy /= offset.w;\n"
    "     offset.xy = offset.xy * 0.5 + 0.5;\n"

    "     float sampleDepth = tex2D( depthSampler, offset.xy).r;\n"
    "     sampleDepth = linearizeDepth(sampleDepth, uProjectionMatrix);\n"

    "     float rangeCheck = smoothstep(0.0, 1.0, radius / abs(originPos.z - sampleDepth));\n"
    "     occlusion += rangeCheck * step(sampleDepth, samplePos.z);\n"
    "  }\n"

    "  occlusion = 1.0 - (occlusion / float(uKernelSize));\n"
    "  return pow(occlusion, uPower);\n"
    "}\n"
  */

  GBuffer * gBuffer;

  EffectsQuad * screenQuad;

  SSAO( GBuffer * theGBuffer );

  void FillAOMap();

  void DoBlurAndAddAOToSourceMap( IDirect3DTexture9 * sourceMap );
};

/*
string aoSource = 
  "texture depthTexture;\n"

  "sampler depthSampler : register(s0) = sampler_state\n"
  "{\n"
  "   texture = <depthTexture>;\n"
  "};\n"

  "texture rotateTexture;\n"

  "sampler rotateSampler : register(s1) = sampler_state\n"
  "{\n"
  "   texture = <rotateTexture>;\n"
  "};\n"

  "float4x4 invViewProj;\n"

  "float screenWidth;\n"
  "float screenHeight;\n"

  "float rotateMapWidth;\n"
  "float rotateMapHeight;\n"

  "float radius;\n"
  "float distScale;\n"
  "float bias;\n"

  "float4 main( float2 texCoord : TEXCOORD0 ) : COLOR0\n "
  "{\n"

  "   float4	rndTable[8];\n"

  "   rndTable[0] = float4 ( -0.5, -0.5, -0.5, 0.0 );\n"
  "   rndTable[1] = float4 (  0.5, -0.5, -0.5, 0.0 );\n"
  "   rndTable[2] = float4 ( -0.5,  0.5, -0.5, 0.0 );\n"
  "   rndTable[3] = float4 (  0.5,  0.5, -0.5, 0.0 );\n"
  "   rndTable[4] = float4 ( -0.5, -0.5,  0.5, 0.0 );\n"
  "   rndTable[5] = float4 (  0.5, -0.5,  0.5, 0.0 );\n"
  "   rndTable[6] = float4 ( -0.5,  0.5,  0.5, 0.0 );\n"
  "   rndTable[7] = float4 (  0.5,  0.5,  0.5, 0.0 );\n"

  "   float depth = tex2D( depthSampler, texCoord ).r;\n"

  "   float4 screenPosition;\n"
  "   screenPosition.x =    texCoord.x * 2.0f - 1.0f ;\n"
  "   screenPosition.y = -( texCoord.y * 2.0f - 1.0f);\n"
  "   screenPosition.z = depth;\n"
  "   screenPosition.w = 1.0f;\n"

  "   float4 position = mul( screenPosition, invViewProj );\n"
  "   position /= position.w;\n"   

  "   float2 rotateMapCoord = float2( texCoord.x * screenWidth / rotateMapWidth, texCoord.y * screenHeight / rotateMapHeight );\n"
  "   float3 plane = tex2D( rotateSampler, rotateMapCoord ).xyz;\n"

  "   float attenuation = 0.0;\n"

  "   for( int i = 0; i < 8; i++ )\n"
  "   {\n"
  "     float3 sample = reflect( rndTable[ i ].xyz, plane );\n"

  "     float2 zSampleTexCoord = texCoord + ( radius * sample.xy ) / position.z;\n"
  "     float	zSample = tex2D( depthSampler, zSampleTexCoord ).r;\n"

  "     screenPosition.x =    zSampleTexCoord.x * 2.0f - 1.0f ;\n"
  "     screenPosition.y = -( zSampleTexCoord.y * 2.0f - 1.0f);\n"
  "     screenPosition.z = zSample;\n"
  "     screenPosition.w = 1.0f;\n"

  "     float4 iPosition = mul( screenPosition, invViewProj );\n"
  "     iPosition /= iPosition.w;\n" 

  "     float	dist = max ( iPosition.z - position.z, 0.0 ) / distScale;\n"	
  "     float	occl = 15 * max ( dist * (2.0 - dist), 0.0 );\n"

  "     attenuation += 1.0 / ( 1.0 + occl*occl );\n"
  "   }\n"

  "   attenuation = clamp ( attenuation / 8.0 + bias, 0.0, 1.0 );\n"

  "   return float4( attenuation, attenuation, attenuation, 1.0f );\n"
  "};\n";*/