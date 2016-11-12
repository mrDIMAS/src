float4x4 vp : register( c0 );
float4x4 world : register( c4 );
float4 cameraUp : register( c8 );
float4 cameraRight : register( c9 );
float3 cameraFront : register( c10 );
float4 params : register(c11);
float alpha : register(c12);

struct VS_INPUT {
	float4 position : POSITION;
	float2 texcoord : TEXCOORD0;
	float4 direction : TEXCOORD2;
};

struct VS_OUTPUT {
	float4 position : POSITION;
	float2 texcoord : TEXCOORD0;
	float4 color : TEXCOORD1;
};

VS_OUTPUT main( VS_INPUT input ) {
	VS_OUTPUT output;
	
	float4 offset = ( input.direction.x * cameraRight + input.direction.y * cameraUp ) * params.w;		
	output.position = mul( vp, mul( world, input.position ) + offset );
	output.texcoord = input.texcoord;
	output.color = float4(params.xyz, alpha);
	
	return output;
};