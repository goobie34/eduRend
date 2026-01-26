
Texture2D texDiffuse : register(t0);

struct PSIn
{
	float4 Pos  : SV_Position;
	float3 Normal : NORMAL;
	float2 TexCoord : TEX;
};

//-----------------------------------------------------------------------------------------
// Pixel Shader
//-----------------------------------------------------------------------------------------

float4 PS_main(PSIn input) : SV_Target
{
    float lambert = (dot(input.Normal, normalize(float3(0.5, 0.5, 1))) + 1)/2;
    float halfLambert = (dot(input.Normal, normalize(float3(0.5, 0.5, 1))));
	
    //return float4(lambert, lambert, lambert, 1);
    //return float4(halfLambert, halfLambert, halfLambert, 1);
	
	// Debug shading #1: map and return normal as a color, i.e. from [-1,1]->[0,1] per component
	// The 4:th component is opacity and should be = 1
    return float4(input.Normal * 0.5 + 0.5, 1);
	
	 //Debug shading #2: map and return texture coordinates as a color (blue = 0)
	 //return float4(input.TexCoord, 0, 1);
}