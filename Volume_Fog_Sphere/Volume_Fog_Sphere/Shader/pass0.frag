#version 430

out vec4 FragColor;
  
in vec2 TextCoord;

in float Fog_Val;

uniform sampler2D ourTexture;
 
void main()
{

	float Fog_Val_Temp = Fog_Val / 6000.0f;

	float r = Fog_Val_Temp * 0.1f;
	float g = Fog_Val_Temp *0.4f;
	float b = Fog_Val_Temp *0.6f;


	vec4 ResColor = texture(ourTexture, TextCoord);

	ResColor += vec4(r, g, b, ResColor.w);

    FragColor = ResColor;
}