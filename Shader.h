#pragma once

class Shader : public RendererComponent {
protected:
	string mSourceName;
	DWORD * LoadBinary();
public:
	static vector<Shader*> msShaderList;
	explicit Shader( const string & sourceName );
	virtual ~Shader();
	virtual void OnLostDevice();
	virtual void OnResetDevice();
};

class VertexShader : public Shader {
private:
    IDirect3DVertexShader9 * shader;
	static IDirect3DVertexShader9 * msLastBinded;
	virtual void Initialize();
public:
    explicit VertexShader( string fileName );
    virtual ~VertexShader();
    void Bind();
	virtual void OnLostDevice();
	virtual void OnResetDevice();
};

class PixelShader : public Shader {
private:
    IDirect3DPixelShader9 * shader;
	static IDirect3DPixelShader9 * msLastBinded;
	virtual void Initialize();
public:
    explicit PixelShader( string fileName );
    virtual ~PixelShader();
    void Bind();
	virtual void OnLostDevice();
	virtual void OnResetDevice();
};

