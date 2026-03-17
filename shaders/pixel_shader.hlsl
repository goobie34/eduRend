Texture2D texDiffuse : register(t0);
Texture2D texNormal : register(t1);
TextureCube texCube : register(t2);

SamplerState texSampler: register(s0);

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
	float3 Tangent : TANGENT;
	float3 Binormal : BINORMAL;
	float2 TexCoord : TEX;
};

//-----------------------------------------------------------------------------------------
// Pixel Shader
//-----------------------------------------------------------------------------------------
float3 NormalSampleToWorldSpace(float3 normalSample, float3 unitNormalWorld, float3 tangentWorld, float3 binormalWorld)
{
    // Uncompress each component from [0,1] to [-1,1].
    float3 tangentSpaceNormal = normalize(2.0f * normalSample - 1.0f);
    
    // Build orthonormal basis.
    float3 N = normalize(unitNormalWorld);
    float3 T = normalize(tangentWorld - dot(tangentWorld, N) * N);
    float3 B = cross(T, N);
   
    float3x3 TBN = transpose(float3x3(T, B, N));
    
    // Transform from tangent space to world space.
    float3 worldSpaceNormal = mul(TBN, tangentSpaceNormal);
    return normalize(worldSpaceNormal);
}
float4 PS_main(PSIn input) : SV_Target
{
    //return float4(normalize(input.Normal) * 0.5 + 0.5, 1);
    //return float4(normalize(input.Binormal) * 0.5 + 0.5, 1);
    //return float4(normalize(input.Tangent) * 0.5 + 0.5, 1);

    float3 lightdir = normalize(LightPosition.xyz - input.PosWorld);    //from object --> light
    float3 cameradir = normalize(CameraPosition.xyz - input.PosWorld);  //from object --> camera    
    
    float3 normalSample = texNormal.Sample(texSampler, input.TexCoord).xyz;
    float3 normal = NormalSampleToWorldSpace(normalSample, input.Normal, input.Tangent, input.Binormal);
    
    //return float4(normalSample, 1);
    //float3 normal = normalize(input.Normal); //object surface normal
    
    //light intensity
    float intensity_scale = 10;
    //float intensity = min(intensity_scale / max(length(LightPosition.xyz - input.PosWorld), 1), 1); //intensity scales with distance to the light
    float intensity = 1;
    float shininess = 64;
    
    float4 diffuseTexture = texCube.Sample(texSampler, normalize(LightPosition.xyz));
    //float4 diffuseTexture = texDiffuse.Sample(texSampler, input.TexCoord);
    //float4 diffuseTexture = float4(normalSample, 1);
    
    float3 reflection = normalize(reflect(-lightdir, normal)); //vector for reflecting light in the object

    //phong shading 
    float ambientScale = 0.2;
    float4 ambient = float4(diffuseTexture.xyz * ambientScale, diffuseTexture.w); //fake ambient by scaling down diffuseTexture
    float4 diffuse = diffuseTexture * max(dot(normal, lightdir), 0);
    //float4 diffuse = Diffuse * max(dot(normal, lightdir), 0);
    float4 specular = Specular * pow(max(dot(reflection, cameradir), 0), shininess);
    float4 phong = ambient + diffuse + specular;
    return float4(phong.xyz * intensity, phong.w);
	
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

