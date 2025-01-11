cbuffer MatrixBuffer : register(b0)
{
    matrix modelViewProjection;
}

struct VSOutput
{
    float3 worldPos : POSITION;
    float3 normal : NORM;
    float4 pos : SV_Position;
};

VSOutput main(float3 pos: POSITION, float3 n: NORM)
{
    VSOutput vso;
    
    vso.worldPos = pos;
    vso.normal = n;
    vso.pos = mul(float4(pos, 1.0f), modelViewProjection);
    
    return vso;
}
