#include <stdio.h>
#include "shelldisplay.h"


int main(void) {
  Texture steve = loadPng("steve.png");
  printTexture(&steve, 90, 90, SINGLE_SPACE_RECT, SAMPLE_NEAREST);
  printTexture(&steve, 90, 90, UPPER_HALF_BLOCK, SAMPLE_NEAREST);
  printf("width: %d height: %d\n", steve.width, steve.height);
  return 0;
}
