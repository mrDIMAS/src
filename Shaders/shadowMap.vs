float4x4 wvp : register( c0 );
float4x4 world : register( c4 );
float4x4 bones[60] : register( c8 );

bool useSkeletalAnimation : register( b0 );
bool isPointShadowMap : register( b1 );
bool useDirectionalLightShadows : register( b2 );

struct Input {
	float4 position : POSITION;
	float2 texCoord : TEXCOORD0;
	float4 boneIndices : TEXCOORD1;
	float4 boneWeights : TEXCOORD2;
};

struct VSOutput {
   float4 position : POSITION;
   float4 vertexPosition : TEXCOORD0;
   float2 texCoord : TEXCOORD1;
};


VSOutput main( Input input ) {
	VSOutput output;

	float4 localPosition;

	if( useSkeletalAnimation ) {
		localPosition = float4( 0.0f, 0.0f, 0.0f, 0.0f );

		localPosition += mul( bones[ (int)input.boneIndices.x ], input.position ) * input.boneWeights.x;
		localPosition += mul( bones[ (int)input.boneIndices.y ], input.position ) * input.boneWeights.y;
		localPosition += mul( bones[ (int)input.boneIndices.z ], input.position ) * input.boneWeights.z;
		localPosition += mul( bones[ (int)input.boneIndices.w ], input.position ) * input.boneWeights.w;
	} else {
		localPosition = input.position;
	}

	output.position = mul( wvp, localPosition );   
	if( isPointShadowMap ) {
		// write world space position
		output.vertexPosition = mul( world, localPosition );   
	} else {
		// write clip-space position
		output.vertexPosition = output.position; 
	}
	output.texCoord = input.texCoord;   

	return output;
};