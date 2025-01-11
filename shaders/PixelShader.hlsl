cbuffer MatrixBuffer : register(b0)
{
    float3 lightPos;
}

static const float3 materialCol = { 0.9f, 0.9f, 0.9f };
static const float3 ambient = { 0.2f, 0.2f, 0.2f };
static const float3 diffuseColor = { 1.0f, 1.0f, 1.0f };
static const float diffuseIntensity = 0.1f;
static const float attConst = 1.0f;
static const float attLin = 0.0014f;
static const float attQuad = 0.000007f;

float4 main (float3 worldPos: POSITION, float3 n : NORM) : SV_Target
{
    const float3 vToL = lightPos - worldPos;
    const float distToL = length(vToL);
    const float3 dirToL = vToL / distToL;
    
    const float att = 1.0f / (attConst + attLin * distToL + attQuad * (distToL * distToL));
    const float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f, dot(dirToL, n));
    
    return float4(saturate(diffuse + ambient), 1.0f);

}
