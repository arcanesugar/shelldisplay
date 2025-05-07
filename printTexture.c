#include <stdio.h>
#include "shelldisplay.h"

void printTextureDoubleSpace(Texture *t, int width, int height, int sampleType);
void printTextureSingleSpace(Texture *t, int width, int height, int sampleType);
void printTextureSingleSpaceRect(Texture *t, int width, int height, int sampleType);
void printTextureUnicodeUpperHalfBlock(Texture *t, int width, int height, int sampleType);

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
    case UPPER_HALF_BLOCK:
      printTextureUnicodeUpperHalfBlock(t, width, height, sampleType);
    break;
    default:
    raiseError("printTexture(): Unrecognised pixel type\n");
  }
}

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

void printTextureUnicodeUpperHalfBlock(Texture *t, int width, int height, int sampleType){
  char utf8[] = {0xE2, 0x96, 0x80, 0};
  for(int y = 0; y<height; y+=2){
    for(int x = 0; x<width; x++){
      Color a = sampleTexture(t, (double)x/width, (double)y/height, sampleType);
      Color b = sampleTexture(t, (double)x/width, (double)(y+1)/height, sampleType);
      truecolor(0,a);//forground is upper pixel
      truecolor(1,b);//background is lower pixel
      printf("%s", utf8);
    }
    clearFormatting();
    printf("\n");
  }
}

