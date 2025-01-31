cbuffer ViewProjection : register(b0)
{
    matrix modelViewProjection;
};

struct VS_Input
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float4 color : COLOR;
    float isSelected : SELECTED;
};

struct VS_Output
{
    float4 position : SV_Position;
    float2 pixelCoord : TEXCOORD0;
    float3 normal : NORMAL;
    float4 color : COLOR;
    float isSelected : SELECTED;
};

VS_Output main(VS_Input input)
{
    VS_Output output;
    output.position = mul(float4(input.position, 1.0f), modelViewProjection);
    output.pixelCoord = output.position.xy;
    output.normal = input.normal;
    output.color = input.color;
    output.isSelected = input.isSelected;
    return output;
}