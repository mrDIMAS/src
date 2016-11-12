float4x4 gProj : register(c0);

struct VSOutput {
	float4 position : POSITION;
	float2 texCoord : TEXCOORD0;
    float4 color : TEXCOORD1;
};

VSOutput main( float4 position : POSITION, float2 texCoord : TEXCOORD0, float4 color : TEXCOORD1 ) { 
	VSOutput output;
	output.position = mul( gProj, position );
	output.texCoord = texCoord;
	output.color = color;
	return output;
};