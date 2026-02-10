Texture2D texDiffuse : register(t0);

cbuffer LightCamBuffer : register(b0)
{
    float4 CameraPosition;
    float4 LightPosition;
};

cbuffer MaterialBuffer : register(b1)
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
};

struct PSIn
{
	float4 Pos  : SV_Position;
	float3 PosWorld  : TEXCOORD0;
	float3 Normal : NORMAL;
	float2 TexCoord : TEX;
};

//-----------------------------------------------------------------------------------------
// Pixel Shader
//-----------------------------------------------------------------------------------------

float4 PS_main(PSIn input) : SV_Target
{
    float3 lightdir = normalize(LightPosition.xyz - input.PosWorld);
    //float3 lightdir = normalize(float3(0.5, 0.5, 1));
    float3 normal = normalize(input.Normal);
    
    float3 cameradir = normalize(CameraPosition.xyz - input.PosWorld);
    float3 reflection = reflect(-lightdir, normal);
    //float3 reflection = lightdir - 2 * mul(dot(lightdir, normal), normal);
    
    //light intensity
    //float intensity = min(20 / max(length(LightPosition.xyz - input.PosWorld), 1), 1);
    float intensity = 1;
    float shininess = 200;
    
    float4 ambient = float4(0, 0, 0, 1); //black
    //float4 diffuse = float4(1, 0, 0, 1); //red
    //float4 ambient = Ambient;
    float4 diffuse = Diffuse * max(dot(normal, lightdir), 0);
    float4 specular = Specular * max(pow(abs(dot(reflection, cameradir)), shininess), 0);
    float4 lambert = ambient + diffuse + specular;
    return float4(lambert.xyz * intensity, 1);
	
    //return float4(normalize(LightPosition.xyz - input.PosWorld), 1);
    //return float4(normalize(CameraPosition.xyz), 1);
	
	// Debug shading #1: map and return normal as a color, i.e. from [-1,1]->[0,1] per component
	// The 4:th component is opacity and should be = 1
    //return float4(input.Normal * 0.5 + 0.5, 1);
	
	 //Debug shading #2: map and return texture coordinates as a color (blue = 0)
	 //return float4(input.TexCoord, 0, 1);
	
	//float lambert = (dot(input.Normal, normalize(float3(0.5, 0.5, 1))) + 1) / 2;
    //float halfLambert = (dot(input.Normal, normalize(float3(0.5, 0.5, 1))));
	
    //return float4(lambert, lambert, lambert, 1);
    //return float4(halfLambert, halfLambert, halfLambert, 1);
    //normalize(float4(0.5, 0.5, 1, 0));
}
