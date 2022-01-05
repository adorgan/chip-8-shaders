#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

// texture samplers
uniform sampler2D texture1;
uniform int usingTexture;
uniform vec3 myColor;

void main() {
  if(usingTexture > 0){
    FragColor = texture(texture1, TexCoord);
  }
  else{
    FragColor = vec4(myColor, 1.0f);
  }
}