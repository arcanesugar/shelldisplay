#include <stdio.h>
#include "shelldisplay.h"


int main(void) {
  Texture steve = loadPng("steve.png");
  drawTexture(steve, 40, 40);
  printf("width: %d height: %d\n", steve.width, steve.height);
  return 0;
}
