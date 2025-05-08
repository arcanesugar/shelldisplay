#include <stdio.h>
#include "shelldisplay.h"

void printTextureDoubleSpace(Texture *t, int width, int height, int sampleType);
void printTextureSingleSpace(Texture *t, int width, int height, int sampleType);
void printTextureUnicodeUpperHalfBlock(Texture *t, int width, int height, int sampleType);

void printTexture(Texture *t, int width, int height, int pixelType, int sampleType){
  switch(pixelType){
    case PIX_DOUBLE_SPACE:
      printTextureDoubleSpace(t, width, height, sampleType);
    break;
    case PIX_SINGLE_SPACE:
      printTextureSingleSpace(t, width, height, sampleType);
    break;
    case PIX_UPPER_HALF_BLOCK:
      printTextureUnicodeUpperHalfBlock(t, width, height, sampleType);
    break;
    default:
    raiseError("printTexture(): Unrecognised pixel type\n");
  }
}

void printTextureDoubleSpace(Texture *t, int width, int height, int sampleType){
  for(int y = 0; y<height; y+=2){
    Color lastColor = {1, 1, 1};
    truecolor(1, lastColor);
    for(int x = 0; x<width; x+=2){
      Color c = sampleTexture(t, (double)x/width, (double)y/height, sampleType);
      if(!compareColors(lastColor, c)){
        truecolor(1,c);
        lastColor = c;
      }
      printf("  ");
    }
    clearFormatting();
    printf("\n");
  }
}

void printTextureSingleSpace(Texture *t, int width, int height, int sampleType){
  for(int y = 0; y<height; y+=2){
    Color lastColor = {1, 1, 1};
    truecolor(1, lastColor);
    for(int x = 0; x<width; x++){
      Color c = sampleTexture(t, (double)x/width, (double)y/height, sampleType);
      if(!compareColors(lastColor, c)){
        truecolor(1,c);
        lastColor = c;
      }
      printf(" ");
    }
    clearFormatting();
    printf("\n");
  }
}

void printTextureUnicodeUpperHalfBlock(Texture *t, int width, int height, int sampleType){
  char utf8[] = {0xE2, 0x96, 0x80, 0};
  for(int y = 0; y<height; y+=2){
    Color lastA = {1, 1, 1};
    Color lastB = {1, 0, 1};
    truecolor(0,lastA);
    truecolor(1,lastB);
    for(int x = 0; x<width; x++){
      Color a = sampleTexture(t, (double)x/width, (double)y/height, sampleType);
      Color b = sampleTexture(t, (double)x/width, (double)(y+1)/height, sampleType);
      //background is lower pixel, foreground is upper
      if(!compareColors(lastA, a)){
        truecolor(0,a);
        lastA = a;
      }
      if(!compareColors(lastB, b)){
        truecolor(1,b);
        lastB = b;
      }
      printf("%s", utf8);
    }
    clearFormatting();
    printf("\n");
  }
}

