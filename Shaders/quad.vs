float4x4 g_projection : register( c0 );

struct VS_INPUT {
	float4 position : POSITION;
	float3 texcoord : TEXCOORD0;
};

struct VS_OUTPUT {
	float4 position : POSITION;
	float3 texcoord : TEXCOORD0;
};

VS_OUTPUT main(VS_INPUT input) {
	VS_OUTPUT output;
	output.position = mul( g_projection, input.position );
	output.texcoord = input.texcoord;
	return output;
};