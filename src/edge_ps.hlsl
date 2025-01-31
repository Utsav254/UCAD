struct PS_Input
{
    float4 position : SV_Position;
    float4 color : COLOR;
    float isSelected : SELECTED;
};

float4 main(PS_Input input) : SV_Target
{
    float4 finalColor = input.color;

    // If selected, change the line colour to red
    if (input.isSelected > 0.5f)
    {
        finalColor = float4(1.0f, 1.0f, 0.0f, 1.0f); // Red for selected
    }

    return finalColor;
}
