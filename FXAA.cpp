#include "FXAA.h"

void FXAA::DoAntialiasing( IDirect3DTexture9 * outTexture ) {
    g_device->SetRenderTarget( 0, backBufferRT );
    g_device->Clear( 0, 0, D3DCLEAR_TARGET | D3DCLEAR_STENCIL, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0, 0 );

    g_device->SetTexture( 0, outTexture );

    effectsQuad->Bind();
    pixelShader->Bind();

    pixelShader->GetConstantTable()->SetFloat( g_device, screenWidth, g_width );
    pixelShader->GetConstantTable()->SetFloat( g_device, screenHeight, g_height );

    g_device->SetRenderState( D3DRS_COLORWRITEENABLE, 0xFFFFFFFF );
    g_device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_NOTEQUAL);
    g_device->SetRenderState(D3DRS_CCW_STENCILFUNC, D3DCMP_NOTEQUAL);
    g_device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_ZERO );
    g_device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
    g_device->SetRenderState( D3DRS_STENCILENABLE, FALSE );
    g_device->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    g_device->SetRenderState( D3DRS_ZENABLE, FALSE );

    effectsQuad->Render();
}

void FXAA::BeginDrawIntoTexture() {
    g_device->SetRenderTarget( 0, renderTarget );
}

FXAA::FXAA() {
    string source =
        "#define FxaaBool bool\n"
        "#define FxaaDiscard clip(-1)\n"
        "#define FxaaFloat float\n"
        "#define FxaaFloat2 float2\n"
        "#define FxaaFloat3 float3\n"
        "#define FxaaFloat4 float4\n"
        "#define FxaaHalf half\n"
        "#define FxaaHalf2 half2\n"
        "#define FxaaHalf3 half3\n"
        "#define FxaaHalf4 half4\n"
        "#define FxaaSat(x) saturate(x)\n"

        "#define FxaaInt2 float2\n"
        "#define FxaaTex sampler2D\n"
        "#define FxaaTexTop(t, p) tex2Dlod(t, float4(p, 0.0, 0.0))\n"
        "#define FxaaTexOff(t, p, o, r) tex2Dlod(t, float4(p + (o * r), 0, 0))\n"


        "FxaaFloat FxaaLuma(FxaaFloat4 rgba) {\n"
        "    rgba.w = dot(rgba.rgb, FxaaFloat3(0.299, 0.587, 0.114));\n"
        "return  rgba.w; }\n"


        "FxaaFloat4 FxaaPixelShader(\n"
        "    FxaaFloat2 pos,\n"
        "    FxaaFloat4 fxaaConsolePosPos,\n"
        "    FxaaTex tex,\n"
        "    FxaaFloat4 fxaaConsoleRcpFrameOpt,\n"
        "    FxaaFloat4 fxaaConsoleRcpFrameOpt2,\n"
        "    FxaaFloat fxaaConsoleEdgeSharpness,\n"
        "    FxaaFloat fxaaConsoleEdgeThreshold,\n"
        "    FxaaFloat fxaaConsoleEdgeThresholdMin) {\n"

        "    FxaaFloat lumaNw = FxaaLuma(FxaaTexTop(tex, fxaaConsolePosPos.xy));\n"
        "    FxaaFloat lumaSw = FxaaLuma(FxaaTexTop(tex, fxaaConsolePosPos.xw));\n"
        "    FxaaFloat lumaNe = FxaaLuma(FxaaTexTop(tex, fxaaConsolePosPos.zy));\n"
        "    FxaaFloat lumaSe = FxaaLuma(FxaaTexTop(tex, fxaaConsolePosPos.zw));\n"
        "    FxaaFloat4 rgbyM = FxaaTexTop(tex, pos.xy);\n"
        "    #if (FXAA_GREEN_AS_LUMA == 0)\n"
        "        FxaaFloat lumaM = rgbyM.w;\n"
        "    #else\n"
        "        FxaaFloat lumaM = rgbyM.y;\n"
        "    #endif\n"
        "    FxaaFloat lumaMaxNwSw = max(lumaNw, lumaSw);\n"
        "    lumaNe += 1.0/384.0;\n"
        "    FxaaFloat lumaMinNwSw = min(lumaNw, lumaSw);\n"
        "    FxaaFloat lumaMaxNeSe = max(lumaNe, lumaSe);\n"
        "    FxaaFloat lumaMinNeSe = min(lumaNe, lumaSe);\n"
        "    FxaaFloat lumaMax = max(lumaMaxNeSe, lumaMaxNwSw);\n"
        "    FxaaFloat lumaMin = min(lumaMinNeSe, lumaMinNwSw);\n"
        "    FxaaFloat lumaMaxScaled = lumaMax * fxaaConsoleEdgeThreshold;\n"
        "    FxaaFloat lumaMinM = min(lumaMin, lumaM);\n"
        "    FxaaFloat lumaMaxScaledClamped = max(fxaaConsoleEdgeThresholdMin, lumaMaxScaled);\n"
        "    FxaaFloat lumaMaxM = max(lumaMax, lumaM);\n"
        "    FxaaFloat dirSwMinusNe = lumaSw - lumaNe;\n"
        "    FxaaFloat lumaMaxSubMinM = lumaMaxM - lumaMinM;\n"
        "    FxaaFloat dirSeMinusNw = lumaSe - lumaNw;\n"
        "    if(lumaMaxSubMinM < lumaMaxScaledClamped) return rgbyM;\n"
        "    FxaaFloat2 dir;\n"
        "    dir.x = dirSwMinusNe + dirSeMinusNw;\n"
        "    dir.y = dirSwMinusNe - dirSeMinusNw;\n"
        "    FxaaFloat2 dir1 = normalize(dir.xy);\n"
        "    FxaaFloat4 rgbyN1 = FxaaTexTop(tex, pos.xy - dir1 * fxaaConsoleRcpFrameOpt.zw);\n"
        "    FxaaFloat4 rgbyP1 = FxaaTexTop(tex, pos.xy + dir1 * fxaaConsoleRcpFrameOpt.zw);\n"
        "    FxaaFloat dirAbsMinTimesC = min(abs(dir1.x), abs(dir1.y)) * fxaaConsoleEdgeSharpness;\n"
        "    FxaaFloat2 dir2 = clamp(dir1.xy / dirAbsMinTimesC, -2.0, 2.0);\n"
        "    FxaaFloat4 rgbyN2 = FxaaTexTop(tex, pos.xy - dir2 * fxaaConsoleRcpFrameOpt2.zw);\n"
        "    FxaaFloat4 rgbyP2 = FxaaTexTop(tex, pos.xy + dir2 * fxaaConsoleRcpFrameOpt2.zw);\n"
        "    FxaaFloat4 rgbyA = rgbyN1 + rgbyP1;\n"
        "    FxaaFloat4 rgbyB = ((rgbyN2 + rgbyP2) * 0.25) + (rgbyA * 0.25);\n"
        "    #if (FXAA_GREEN_AS_LUMA == 0)\n"
        "        FxaaBool twoTap = (rgbyB.w < lumaMin) || (rgbyB.w > lumaMax);\n"
        "    #else\n"
        "        FxaaBool twoTap = (rgbyB.y < lumaMin) || (rgbyB.y > lumaMax);\n"
        "    #endif\n"
        "    if(twoTap) rgbyB.xyz = rgbyA.xyz * 0.5;\n"
        "    return rgbyB; \n"
        "}\n"

        "float SCREEN_WIDTH;\n"
        "float SCREEN_HEIGHT;\n"
        "texture gScreenTexture;\n"

        "sampler screenSampler : register(s0) = sampler_state\n"
        "{\n"
        "    Texture = <gScreenTexture>;\n"
        "};\n"

        "float4 main(float2 tc : TEXCOORD0) : COLOR0\n"
        "{\n"
        "    float pixelWidth = (1 / SCREEN_WIDTH);\n"
        "    float pixelHeight = (1 / SCREEN_HEIGHT);\n"

        "    float2 pixelCenter = float2(tc.x - pixelWidth, tc.y - pixelHeight);\n"
        "    float4 fxaaConsolePosPos = float4(tc.x, tc.y, tc.x + pixelWidth, tc.y + pixelHeight);\n"
        //"    return float4( 1, 0, 0, 1 );\n"

        "    return FxaaPixelShader("
        "        pixelCenter,"
        "        fxaaConsolePosPos,"
        "        screenSampler,"
        "        float4(-0.50 / SCREEN_WIDTH, -0.50 / SCREEN_HEIGHT, 0.50 / SCREEN_WIDTH, 0.50 / SCREEN_HEIGHT),"
        "        float4(-2.0 / SCREEN_WIDTH, -2.0 / SCREEN_HEIGHT, 2.0 / SCREEN_WIDTH, 2.0 / SCREEN_HEIGHT),"
        "        8.0,"
        "        0.125,"
        "        0.05);\n"
        "}\n";

    pixelShader = new PixelShader( source );
    screenWidth = pixelShader->GetConstantTable()->GetConstantByName( 0, "SCREEN_WIDTH" );
    screenHeight = pixelShader->GetConstantTable()->GetConstantByName( 0, "SCREEN_HEIGHT" );
    if( FAILED( D3DXCreateTexture( g_device, g_width, g_height, 0, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &texture )))
        MessageBoxA( 0, "Failed to create FXAA texture.", 0, MB_OK | MB_ICONERROR );

    texture->GetSurfaceLevel( 0, &renderTarget );
    g_device->GetRenderTarget( 0, &backBufferRT );

    effectsQuad = new EffectsQuad;
}

//////////////////////////////////////////////////////////////////////////
// API
//////////////////////////////////////////////////////////////////////////

API void EnableFXAA( ) {
    g_fxaaEnabled = true;
}

API void DisableFXAA( ) {
    g_fxaaEnabled = false;
}

API bool FXAAEnabled() {
    return g_fxaaEnabled;
}