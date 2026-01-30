/*!
 * @brief チェッカーボードワイプ
 */

cbuffer cb : register(b0)
{
    float4x4 mvp;           // MVP行列
    float4 mulColor;        // 乗算カラー
};

cbuffer NagaCB : register(b1)
{
    float negaRate;         // ネガポジ反転率
    float wipeProgress;
};

struct VSInput
{
    float4 pos : POSITION;
    float2 uv  : TEXCOORD0;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};

Texture2D<float4> colorTexture : register(t0); // カラーテクスチャ
sampler Sampler : register(s0);

PSInput VSMain(VSInput In)
{
    PSInput psIn;
    psIn.pos = mul(mvp, In.pos);
    psIn.uv = In.uv;
    return psIn;
}

float4 PSMain(PSInput In) : SV_Target0
{
    float4 color = colorTexture.Sample(Sampler, In.uv);

    float3 negaColor = 1.0f - color.rgb;

    float gray = dot(color.rgb, float3(0.299f, 0.587f, 0.114f));
    float3 grayColor = float3(gray, gray, gray);

    color.rgb = lerp(grayColor, negaColor, negaRate);

    return color;
}