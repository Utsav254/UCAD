struct PS_Input
{
	float4 position : SV_Position;
	float2 pixelCoord : TEXCOORD0;
	float3 normal : NORMAL;
	float4 color : COLOR;
	float isSelected : SELECTED;
};

float4 main(PS_Input input) : SV_Target
{
	float4 finalColor = input.color;
	if (input.isSelected > 0.5f)
	{
		finalColor = float4(0.06f, 0.25f, 0.63f, 1.0f);
	}
	return finalColor;
}