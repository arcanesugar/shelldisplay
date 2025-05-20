#include <stdio.h>

#include "shelldisplay.h"

int main(){
  setColorMode(CM_TRUECOLOR);
  Texture t = createTexture(2, 2);
  setPixel(t, 0, 0, (Color){1,0,0});
  setPixel(t, 1, 0, (Color){0,1,0});
  setPixel(t, 0, 1, (Color){0,0,1});
  setPixel(t, 1, 1, (Color){0,0,0});
  for(int i = 0; i<t.width*t.height; i++){
    //t.pixels[i] = invertColor(t.pixels[i]);
    t.pixels[i] = multiplyColors(t.pixels[i], (Color){1,1,0});
  }
  Texture screen = resampleTexture(t, 90, 90, SAMPLE_BILINEAR);
  setStyle(S_BLINK|S_BOLD|S_UNDERLINE|S_ITALIC|S_STRIKETHROUGH);
  printf("TEXTTEXTTEXTTEXTTEXT\n");
  printTexture(screen, PIX_UPPER_HALF_BLOCK);
  return 0;
};