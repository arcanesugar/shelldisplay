#include "shelldisplay.h"
#include <malloc.h>
#include <math.h>

Texture createTexture(int width, int height){
  Texture t;
  t.width = width;
  t.height = height;
  t.pixels = malloc(width*height*sizeof(Color));
  return t;
}
void freeTexture(Texture t){
  free(t.pixels);
}

void setPixel(Texture t, int x, int y, Color c){
  t.pixels[(y*t.width)+x] = c;
}

Color getPixel(Texture t, int x, int y){
  return t.pixels[(y*t.width)+x];
}

void fillTexture(Texture t, Color c){
  for(int i = 0; i<(t.width*t.height); i++){
    t.pixels[i] = c;
  }
}

static void printTextureDoubleSpace(Texture t);
static void printTextureSingleSpace(Texture t);
static void printTextureUnicodeUpperHalfBlock(Texture t);

void printTexture(Texture t, int pixelType){
  switch(pixelType){
    case PIX_DOUBLE_SPACE:
      printTextureDoubleSpace(t);
    break;
    case PIX_SINGLE_SPACE:
      printTextureSingleSpace(t);
    break;
    case PIX_UPPER_HALF_BLOCK:
      printTextureUnicodeUpperHalfBlock(t);
    break;
    default:
    raiseError("printTexture(): Unrecognised pixel type\n");
  }
}

void printTextureDoubleSpace(Texture t){
  for(int y = 0; y<t.height; y++){
    Color lastColor = {1, 1, 1};
    truecolor(1, lastColor);
    for(int x = 0; x<t.width; x++){
      Color c = getPixel(t, x,  y);
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

void printTextureSingleSpace(Texture t){
  for(int y = 0; y<t.height; y++){
    Color lastColor = {1, 1, 1};
    truecolor(1, lastColor);
    for(int x = 0; x<t.width; x++){
      Color c = getPixel(t, x,  y);
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

void printTextureUnicodeUpperHalfBlock(Texture t){
  char utf8[] = {0xE2, 0x96, 0x80, 0};
  for(int y = 0; y<t.height; y+=2){
    Color lastA = {1, 1, 1};
    Color lastB = {1, 0, 1};
    truecolor(0,lastA);
    truecolor(1,lastB);
    for(int x = 0; x<t.width; x++){
      Color a = getPixel(t, x,  y);
      Color b = getPixel(t, x,  y+1);
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

static Color sampleBilinear(Texture t, double x, double y);

Color sampleTexture(Texture t, double x, double y, int method){
  switch(method){
    case SAMPLE_NEAREST:
      return getPixel(t, x*t.width, y*t.height);
    case SAMPLE_BILINEAR:
      return sampleBilinear(t, x, y);
    default:
      raiseError("Unrecognised sample type\n");
  }
}

Texture resampleTexture(Texture t, int width, int height, int method){
  Texture n = createTexture(width, height);
  for(int y = 0; y<height; y++){
    for(int x = 0; x<width; x++){
      Color c;
      setPixel(n, x, y, sampleTexture(t, (double)x/width, (double)y/height, method));
      setColor(0, sampleTexture(t, (double)x/width, (double)y/height, method));
    } 
  }
  return n;
}

Color sampleBilinear(Texture t, double x, double y){
  //  a --e-- b
  //  |   |  |
  //  |   g  |
  //  |   |  |
  //  c --f-- d
  int ax, ay, bx, by, cx, cy, dx, dy;
  ax = floor(x*(t.width-1));
  ay = floor(y*(t.height-1));
  dx = ax+1;
  dy = ay+1;
  //printf(" %d, %d, %d, %d |", ax, ay, dx, dy);
  bx = dy;
  by = ay;

  cx = ax;
  cy = dy;
  
  Color a = getPixel(t, ax, ay);
  Color b = getPixel(t, bx, by);
  Color c = getPixel(t, cx, cy);
  Color d = getPixel(t, dx, dy);
  
  Color e = lerpColors(a, b, (x*(t.width-1))-ax);
  Color f = lerpColors(c, d, (x*(t.width-1))-ax);

  return lerpColors(e, f, (y*(t.height-1))-ay);//g
}
