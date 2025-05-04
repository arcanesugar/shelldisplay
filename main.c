#include <stdio.h>
#include "shelldisplay.h"

enum PixelTypes{
  //pixel mapping descriptions assume texture size is the same as display size
  DOUBLE_SPACE,//2x1char maps 1->1
  SINGLE_SPACE,//1x1char pixels are rectangular so 2 pixels are averaged to get the color
  SINGLE_SPACE_RECT,//renders each space as one pixel, no averaging meaning pixels are rectangular
};

void printTextureDoubleSpace(Texture *t, int width, int height, int sampleType){
  for(int y = 0; y<height; y+=2){
    for(int x = 0; x<width; x+=2){
      Color c = sampleTexture(t, (double)x/width, (double)y/height, sampleType);
      truecolor(1,c);
      printf("  ");
    }
    clearFormatting();
    printf("\n");
  }
}

void printTextureSingleSpace(Texture *t, int width, int height, int sampleType){
  for(int y = 0; y<height; y+=2){
    for(int x = 0; x<width; x++){
      Color top = sampleTexture(t, (double)x/width, (double)y/height, sampleType);
      Color bottom = sampleTexture(t, (double)x/width, (double)(y+1)/height, sampleType);
      truecolor(1,lerpColors(top, bottom, 0.5));
      printf(" ");
    }
    clearFormatting();
    printf("\n");
  }
}

void printTextureSingleSpaceRect(Texture *t, int width, int height, int sampleType){
  for(int y = 0; y<height; y+=2){
    for(int x = 0; x<width; x++){
      Color c = sampleTexture(t, (double)x/width, (double)y/height, sampleType);
      truecolor(1,c);
      printf(" ");
    }
    clearFormatting();
    printf("\n");
  }
}

void printTexture(Texture *t, int width, int height, int pixelType, int sampleType){
  switch(pixelType){
    case DOUBLE_SPACE:
      printTextureDoubleSpace(t, width, height, sampleType);
    break;
    case SINGLE_SPACE:
      printTextureSingleSpace(t, width, height, sampleType);
    break;
    case SINGLE_SPACE_RECT:
      printTextureSingleSpaceRect(t, width, height, sampleType);
    break;
    default:
    raiseError("printTexture(): Unrecognised pixel type\n");
  }
}

int main(void) {
  Texture steve = loadPng("steve.png");
  printTexture(&steve, 90, 90, SINGLE_SPACE, SAMPLE_NEAREST);
  printTexture(&steve, 90, 90, SINGLE_SPACE_RECT, SAMPLE_NEAREST);
  printf("width: %d height: %d\n", steve.width, steve.height);
  return 0;
}
