#include "TextRenderer.h"
#include "GUIRenderer.h"

TextRenderer * g_textRenderer = nullptr;

void TextRenderer::RenderText( GUIText* guiText )
{
    TextQuad * quad = nullptr;
    CheckDXErrorFatal( vertexBuffer->Lock( 0, maxChars * sizeof( TextQuad ), (void**)&quad, 0 ));
    Face * face = nullptr;
    CheckDXErrorFatal( indexBuffer->Lock( 0, maxChars * sizeof( Face ), (void**)&face, 0 ));
    int n = 0, totalLetters = 0;

	RECT boundingRect = guiText->GetBoundingRect();

    int caretX = boundingRect.left;
    int caretY = boundingRect.top;

    int lines, height, avWidth, avSymbolWidth ;
    ComputeTextMetrics( guiText, lines, height, avWidth, avSymbolWidth );

    if( guiText->GetTextAlignment() )
    {
        caretY = boundingRect.top + (( boundingRect.bottom - boundingRect.top ) - height ) / 2.0f;
        caretX = boundingRect.left + (( boundingRect.right - boundingRect.left ) - avWidth ) / 2.0f;
    }

    char buf[8192];
    strcpy( buf, guiText->GetText().c_str() );
    char * ptr = strtok( buf, " " );
    while( ptr )
    {
        // word wrap
        int wordLen = strlen( ptr );
        if( caretX + wordLen * avSymbolWidth > boundingRect.right )
        {
            caretX = boundingRect.left;
            caretY += guiText->GetFont()->glyphSize;
        }
        char * strPtr = ptr;
        while( true )
        {
            unsigned char symbol = *strPtr;
            char lineEnd = symbol == 0;
            if( lineEnd )
            {
                symbol = ' '; // draw space
            }
            BitmapFont::CharMetrics & charMetr = guiText->GetFont()->charsMetrics[ symbol ];

            int currentX = caretX + charMetr.bitmapLeft;
            int currentY = caretY - charMetr.bitmapTop + guiText->GetFont()->glyphSize;

            quad->v1 = TextVertex( ruVector3( currentX, currentY, 0.0f ), charMetr.texCoords[0], guiText->GetPackedColor() );
            quad->v2 = TextVertex( ruVector3( currentX + guiText->GetFont()->glyphSize, currentY, 0.0f ), charMetr.texCoords[1], guiText->GetPackedColor() );
            quad->v3 = TextVertex( ruVector3( currentX + guiText->GetFont()->glyphSize, currentY + guiText->GetFont()->glyphSize, 0.0f ), charMetr.texCoords[2], guiText->GetPackedColor() );
            quad->v4 = TextVertex( ruVector3( currentX, currentY + guiText->GetFont()->glyphSize, 0.0f ), charMetr.texCoords[3], guiText->GetPackedColor() );

            caretX += charMetr.advanceX;

            if( caretX >= boundingRect.right || symbol == '\n' )
            {
                caretX = boundingRect.left;
                caretY += guiText->GetFont()->glyphSize;
            }

            quad++;
            totalLetters++;

            // indices
            face->index[0] = n;
            face->index[1] = n + 1;
            face->index[2] = n + 2;
            face->index[3] = n;
            face->index[4] = n + 2;
            face->index[5] = n + 3;
            face++;
            n += 4;
            if( lineEnd )
                break;
            strPtr++;
        }
        // get next token
        ptr = strtok( 0, " " );
    }
  
    CheckDXErrorFatal( vertexBuffer->Unlock());
    CheckDXErrorFatal( indexBuffer->Unlock());

    CheckDXErrorFatal( g_pDevice->SetTexture( 0, guiText->GetFont()->atlas ) );

    CheckDXErrorFatal( g_pDevice->SetStreamSource( 0, vertexBuffer, 0, sizeof( TextVertex )));
    CheckDXErrorFatal( g_pDevice->SetIndices( indexBuffer ));
    CheckDXErrorFatal( g_pDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, totalLetters * 4, 0, totalLetters * 2 ));
    g_dips++;
}


void TextRenderer::ComputeTextMetrics( GUIText * guiText, int & lines, int & height, int & avWidth, int & avSymbolWidth  )
{
    lines = 1;

	RECT boundingRect = guiText->GetBoundingRect();

    int caretX = boundingRect.left;
    int caretY = boundingRect.top;

    int totalWidth = 0;
    int totalHeight = 0;

	int symbolCount = 0;
    for( unsigned char symbol : guiText->GetText() )
    {
        BitmapFont::CharMetrics & charMetr = guiText->GetFont()->charsMetrics[ symbol ];

        caretX += charMetr.advanceX;
        totalWidth += charMetr.advanceX;
        totalHeight += guiText->GetFont()->glyphSize * 2 - charMetr.bitmapTop ;

        if( caretX >= boundingRect.right || symbol == '\n' )
        {
            lines++;
            caretX = boundingRect.left;
            caretY += guiText->GetFont()->glyphSize;
        }
		symbol++;
		symbolCount++;
    }

    avSymbolWidth = (float)totalWidth / (float)symbolCount;
    avWidth = (float)totalWidth / (float)lines;
    height = (float)totalHeight / (float)symbolCount;
}

TextRenderer::TextRenderer()
{
    maxChars = 8192;
    int vBufLen = maxChars * sizeof( TextQuad );
    CheckDXErrorFatal( g_pDevice->CreateVertexBuffer( vBufLen, D3DUSAGE_DYNAMIC, D3DFVF_TEX1 | D3DFVF_XYZ, D3DPOOL_DEFAULT, &vertexBuffer, nullptr ));
    int iBufLen = maxChars * sizeof( Face );
    CheckDXErrorFatal( g_pDevice->CreateIndexBuffer( iBufLen, D3DUSAGE_DYNAMIC, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &indexBuffer, nullptr ));
}

TextRenderer::~TextRenderer()
{
    vertexBuffer->Release();
    indexBuffer->Release();
}

