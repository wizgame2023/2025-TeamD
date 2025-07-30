#include "../../Libs/BaseLib/DxShaders/INCParameters.hlsli"
#include "../../Libs/BaseLib/DxShaders/INCStructs.hlsli"


cbuffer DissolveBuffer : register(b1)
{
	float dissolveFactor; // 溶解の進行度
	float4 dissolveColor; // 溶解色
};

Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

Texture2D g_noiseTexture : register(t1);

float4 main(PSPNTInput input) : SV_TARGET
{
    //ディゾルブ効果
    float4 noiseColor = g_noiseTexture.Sample(g_sampler, input.tex);
    float noiseValue = noiseColor.r;
    clip(noiseValue - dissolveFactor);
    
    //ライティング処理
    float3 lightdir = normalize(LightDir.xyz);
    float3 N1 = normalize(input.norm);
    
    float4 color = (saturate(dot(N1, -lightdir)) * Diffuse) + Emissive;
    color += input.specular;
    color.a = Diffuse.a;
    if (Activeflags.x)
    {
        color = g_texture.Sample(g_sampler, input.tex) * color;
    }
    
    //エッジ処理
    float EdgeWidth = 0.1f;

    float edge = smoothstep(dissolveFactor, dissolveFactor + EdgeWidth, noiseValue);
    color.rgb = lerp(dissolveColor.rgb, color.rgb, edge);

    return color;
}