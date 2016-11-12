float GetAttenuationWithoutNdotL( in float3 lightVector, in float lightRange) {
	float distance = length( lightVector );
	float kD = distance / lightRange;
	return 1 - min( 1, kD * kD );
}

float GetAttenuation( in float3 lightVector, in float lightRange, in float3 normal ) {
	return saturate(dot( normalize(lightVector), normal ) * GetAttenuationWithoutNdotL(lightVector, lightRange));
}

float4 GetProjection( float3 worldPosition, float4x4 viewProjectionMatrix ) {
	float4 projPos = mul( viewProjectionMatrix, float4( worldPosition, 1 ) );
	
	projPos /= projPos.w;	

	return float4( projPos.x * 0.5f + 0.5f, -projPos.y * 0.5f + 0.5f, projPos.z, 0 );
}

float3 RestorePositionFromDepth(in float2 texcoord, in float4x4 invViewProj, in sampler depthSampler) {
	float4 screenPosition = float4( texcoord.x * 2.0f - 1.0f, -texcoord.y * 2.0f + 1.0f, tex2D( depthSampler, texcoord ).r, 1.0f );
	float4 p = mul( invViewProj, screenPosition );
	p /= p.w;
	return p.xyz;
}