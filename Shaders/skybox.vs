float4x4 worldViewProjection : register( c0 );

struct VSInput {
   float4 position : POSITION;
   float2 texCoord : TEXCOORD0;
};

struct VSOutput {
   float4 position : POSITION;
   float2 texCoord : TEXCOORD0;
};

VSOutput main( VSInput input ) {
   VSOutput output;
   output.position = mul( worldViewProjection, input.position );
   output.texCoord = input.texCoord;
   return output;
};