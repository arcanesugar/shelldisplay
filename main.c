#include <stdio.h>

#include "shelldisplay.h"

int main(){
  Texture t = createTexture(2, 2);
  setPixel(t, 0, 0, (Color){1,0,0});
  setPixel(t, 1, 0, (Color){0,1,0});
  setPixel(t, 0, 1, (Color){0,0,1});
  setPixel(t, 1, 1, (Color){1,1,1});
  Texture screen = resampleTexture(t, 40, 40, SAMPLE_BILINEAR);
  printTexture(screen, PIX_UPPER_HALF_BLOCK);
  return 0;
};
