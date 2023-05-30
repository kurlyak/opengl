#version 430

layout (location = 0) out float FragColor;

in float TexDepth;

void main()
{
    FragColor = TexDepth;
}