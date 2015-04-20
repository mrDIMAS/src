#include "Precompiled.h"
#include "Engine.h"
#include "TextRenderer.h"
#include "GUIRenderer.h"

void TextRenderer::RenderText( GUIText* guiText ) {
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

    if( guiText->GetTextAlignment() ) {
        caretY = boundingRect.top + (( boundingRect.bottom - boundingRect.top ) - height ) / 2.0f;
        caretX = boundingRect.left + (( boundingRect.right - boundingRect.left ) - avWidth ) / 2.0f;
    }

    char buf[8192];
    strcpy( buf, guiText->GetText().c_str() );
    char * ptr = strtok( buf, " " );
    while( ptr ) {
        // word wrap
        int wordLen = strlen( ptr );
        if( caretX + wordLen * avSymbolWidth > boundingRect.right ) {
            caretX = boundingRect.left;
            caretY += guiText->GetFont()->glyphSize;
        }
        char * strPtr = ptr;
		bool lineEnd = false;
        while( !lineEnd ) {
            unsigned char symbol = *strPtr;
            lineEnd = (symbol == 0);
            if( lineEnd ) {
                symbol = ' '; // draw space
            }
            BitmapFont::CharMetrics & charMetr = guiText->GetFont()->charsMetrics[ symbol ];

            int currentX = caretX + charMetr.bitmapLeft;
            int currentY = caretY - charMetr.bitmapTop + guiText->GetFont()->glyphSize;

			int color = guiText->GetPackedColor();

            quad->v1.p.x = currentX;
			quad->v1.p.y = currentY;
			quad->v1.p.z = 0.0f;
			quad->v1.t = charMetr.texCoords[0];
			quad->v1.color = color;

            quad->v2.p.x = currentX + guiText->GetFont()->glyphSize;
			quad->v2.p.y = currentY;
			quad->v2.p.z = 0.0f;
			quad->v2.t = charMetr.texCoords[1];
			quad->v2.color = color;

            quad->v3.p.x = currentX + guiText->GetFont()->glyphSize;
			quad->v3.p.y = currentY + guiText->GetFont()->glyphSize;
			quad->v3.p.z = 0.0f;
			quad->v3.t = charMetr.texCoords[2];
			quad->v3.color = color;

            quad->v4.p.x = currentX;
			quad->v4.p.y = currentY + guiText->GetFont()->glyphSize;
			quad->v4.p.z = 0.0f;
			quad->v4.t = charMetr.texCoords[3];
			quad->v4.color = color;

            caretX += charMetr.advanceX;

            if( caretX >= boundingRect.right || symbol == '\n' ) {
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
            strPtr++;
        }
        // get next token
        ptr = strtok( 0, " " );
    }

    CheckDXErrorFatal( vertexBuffer->Unlock());
    CheckDXErrorFatal( indexBuffer->Unlock());

    CheckDXErrorFatal( Engine::Instance().GetDevice()->SetTexture( 0, guiText->GetFont()->atlas ) );

    CheckDXErrorFatal( Engine::Instance().GetDevice()->SetStreamSource( 0, vertexBuffer, 0, sizeof( TextVertex )));
    CheckDXErrorFatal( Engine::Instance().GetDevice()->SetIndices( indexBuffer ));
	if( totalLetters > 0 ) {
		CheckDXErrorFatal( Engine::Instance().GetDevice()->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, totalLetters * 4, 0, totalLetters * 2 ));
	}
    Engine::Instance().RegisterDIP();
}


void TextRenderer::ComputeTextMetrics( GUIText * guiText, int & lines, int & height, int & avWidth, int & avSymbolWidth  ) {
    lines = 1;

    RECT boundingRect = guiText->GetBoundingRect();

    int caretX = boundingRect.left;
    int caretY = boundingRect.top;

    int totalWidth = 0;
    int totalHeight = 0;

    int symbolCount = 0;
	string & str = guiText->GetText();
	int size = str.size();
    //for( unsigned char symbol : str ) {
	for( int i = 0; i < size; i++ ) {
		unsigned char symbol = str[i];
        BitmapFont::CharMetrics & charMetr = guiText->GetFont()->charsMetrics[ symbol ];

        caretX += charMetr.advanceX;
        totalWidth += charMetr.advanceX;
        totalHeight += guiText->GetFont()->glyphSize * 2 - charMetr.bitmapTop ;

        if( caretX >= boundingRect.right || symbol == '\n' ) {
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

TextRenderer::TextRenderer() {
    maxChars = 8192;
    int vBufLen = maxChars * sizeof( TextQuad );
	CheckDXErrorFatal( Engine::Instance().GetDevice()->CreateVertexBuffer( vBufLen, D3DUSAGE_DYNAMIC, D3DFVF_TEX1 | D3DFVF_XYZ | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &vertexBuffer, nullptr ));
    int iBufLen = maxChars * sizeof( Face );
    CheckDXErrorFatal( Engine::Instance().GetDevice()->CreateIndexBuffer( iBufLen, D3DUSAGE_DYNAMIC, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &indexBuffer, nullptr ));
}

TextRenderer::~TextRenderer() {
    vertexBuffer->Release();
    indexBuffer->Release();
}

