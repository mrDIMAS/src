float4x4 vp : register( c0 );
float4x4 world : register( c4 );
float4 cameraUp : register( c8 );
float4 cameraRight : register( c9 );
float3 cameraFront : register( c10 );

struct VS_INPUT {
	float4 position : POSITION;
	float2 texcoord : TEXCOORD0;
	float4 color : TEXCOORD1;
	float4 direction : TEXCOORD2;
};

struct VS_OUTPUT {
	float4 position : POSITION;
	float2 texcoord : TEXCOORD0;
	float4 color : TEXCOORD1;
	float3 normal : TEXCOORD2;
	float4 worldPosition : TEXCOORD3;
};

VS_OUTPUT main( VS_INPUT input ) {
	VS_OUTPUT output;
	
	output.worldPosition = mul( world, input.position );	
	float4 offset = ( input.direction.x * cameraRight + input.direction.y * cameraUp ) * input.direction.z;		
	output.position = mul( vp, output.worldPosition + offset );
	output.texcoord = input.texcoord;
	output.normal = cameraFront;
	output.color = input.color;
	
	return output;
};