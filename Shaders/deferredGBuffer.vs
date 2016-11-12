float4x4 g_world : register( c0 );
float4x4 g_WVP : register( c4 );
float3 g_camPos : register( c8 );
float3 g_texCoordFlow : register( c9 );
float4x4 g_bones[61] : register( c10 );

bool useSkeletalAnimation : register( b0 );
bool useVegetationAnimation : register( b1 );

struct VS_INPUT {
	float4 position : POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD0;
	float3 tangent : TANGENT;
	float4 boneIndices : TEXCOORD1;
	float4 boneWeights : TEXCOORD2;
};

struct VS_OUTPUT {
	float4 position : POSITION;
	float4 screenPosition : TEXCOORD0;
	float3 normal : TEXCOORD1;
	float2 diffuseMapCoord : TEXCOORD2;
	float3 binormal : TEXCOORD3;
	float3 tangent : TEXCOORD4;	
	float3 eyeInTangent : TEXCOORD5;
	float3 worldPosition : TEXCOORD6;
};

void BoneTransform( 
	in int boneIndex, 
	in float boneWeight, 
	in float4 position, 
	in float3 normal, 
	in float3 tangent,
	inout float4 localPosition, 
	inout float3 localNormal, 
	inout float3 localTangent 
) {
	localPosition += mul( g_bones[boneIndex], position ) * boneWeight;
	localNormal += mul( (float3x3)g_bones[boneIndex], normal ) * boneWeight;
	localTangent += mul( (float3x3)g_bones[boneIndex], tangent ) * boneWeight;	
} 

// for vegetation 
float4 SmoothCurve( float4 x ) {  
	return x * x *( 3.0 - 2.0 * x );  
}  

float4 TriangleWave( float4 x ) {  
	return abs( frac( x + 0.5 ) * 2.0 - 1.0 );  
} 

float4 SmoothTriangleWave( float4 x ) {  
	return SmoothCurve( TriangleWave( x ) );  
}  
	
VS_OUTPUT main(VS_INPUT input) {
	VS_OUTPUT output;
	
	float4 localPosition;
	float3 localNormal;
	float3 localTangent;
	
	if (useSkeletalAnimation) {
		localPosition = float4( 0.0f, 0.0f, 0.0f, 0.0f );
		localNormal = float3( 0.0f, 0.0f, 0.0f );
		localTangent = float3( 0.0f, 0.0f, 0.0f );

		BoneTransform( (int)input.boneIndices.x, input.boneWeights.x, input.position, input.normal, input.tangent, localPosition, localNormal, localTangent );
		BoneTransform( (int)input.boneIndices.y, input.boneWeights.y, input.position, input.normal, input.tangent, localPosition, localNormal, localTangent );
		BoneTransform( (int)input.boneIndices.z, input.boneWeights.z, input.position, input.normal, input.tangent, localPosition, localNormal, localTangent );
		BoneTransform( (int)input.boneIndices.w, input.boneWeights.w, input.position, input.normal, input.tangent, localPosition, localNormal, localTangent );
		
	} else {
		localPosition = input.position;
		localNormal = input.normal;
		localTangent = input.tangent;
	}

	// compute screen vertex position
	if (useVegetationAnimation) {
		localPosition.xyz += (0.03f * SmoothTriangleWave(0.05f * g_texCoordFlow.z * localPosition)).xyz;
	}
	output.position = mul( g_WVP, localPosition );	
	

	
	// compute normal in world space
	output.normal = normalize( mul((float3x3)g_world, localNormal ));	
	// compute tangent in world space
	output.tangent = normalize( mul((float3x3)g_world, localTangent ));	
	// compute binormal as a cross product of out normal and tangent
	output.binormal = normalize( cross( output.tangent, output.normal ));	
	// pass vertex screen position into pixel shader
	output.screenPosition = output.position;	
	// write texcoord
	output.diffuseMapCoord = input.texCoord + g_texCoordFlow.xy;
		
	output.worldPosition = mul( g_world, localPosition ).xyz;
	
	float3 toCamera = g_camPos - output.worldPosition;
	float3x3 worldToTangent = float3x3( output.tangent, output.binormal, output.normal );
	output.eyeInTangent = normalize( mul( worldToTangent, toCamera ));

	return output;
};