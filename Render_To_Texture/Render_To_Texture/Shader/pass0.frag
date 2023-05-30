#version 430

out vec4 FragColor;
  
in vec2 TextCoord;

uniform sampler2D ourTexture;
 
void main()
{
    FragColor = texture(ourTexture, TextCoord);
}