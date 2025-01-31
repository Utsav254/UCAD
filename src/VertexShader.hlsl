cbuffer Transform : register(b0)
{
    matrix mvp;
};

struct VSOut
{    
    float4 color : COLOR;
    float4 pos : SV_Position;
};

VSOut main( float3 pos : POSITION, float4 Col : COLOR)
{
    VSOut vso;
    vso.pos = mul(float4(pos, 1.0f), mvp);
    vso.color = Col;
    return vso;
}

