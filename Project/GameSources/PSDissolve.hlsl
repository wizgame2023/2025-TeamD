
//下二つは他のシェーダーのデータ型をコピペ
struct PSPNTInput
{
    float4 position : SV_POSITION;
    float3 norm : NORMAL;
    float4 specular : COLOR;
    float2 tex : TEXCOORD;
};

cbuffer ParametersBuffer : register(b0)
{
    float4x4 World : packoffset(c0);
    float4x4 View : packoffset(c4);
    float4x4 Projection : packoffset(c8);
    float4 Emissive : packoffset(c12);
    float4 Diffuse : packoffset(c13);
    float4 Specular : packoffset(c14);
    uint4 Activeflags : packoffset(c15);
    float4 LightDir : packoffset(c16);
    float4 LightPos : packoffset(c17);
    float4 EyePos : packoffset(c18);
    float4x4 LightView : packoffset(c19);
    float4x4 LightProjection : packoffset(c23);
    float4x3 Bones[100] : packoffset(c27);
};

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