#pragma once



class VertexShader {
private:
    IDirect3DVertexShader9 * shader;
    ID3DXConstantTable * constants;
	
	static IDirect3DVertexShader9 * msLastBinded;
public:
    VertexShader( string source );
    VertexShader( string fileName, bool binary );
    ~VertexShader();
    ID3DXConstantTable * GetConstantTable();
    void Bind();

};

class PixelShader {
private:
    IDirect3DPixelShader9 * shader;
    ID3DXConstantTable * constants;

	static IDirect3DPixelShader9 * msLastBinded;
public:
    PixelShader( string source );
    PixelShader( string fileName, bool binary );
    ~PixelShader();
    ID3DXConstantTable * GetConstantTable();
    void Bind();

};

