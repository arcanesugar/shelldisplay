#pragma once

#include <stdbool.h>

typedef struct Color{
  double r,g,b;
}Color;

typedef struct Texture{
  unsigned int width, height;
  Color* pixels;
}Texture;

enum SampleMethods{
  SAMPLE_NEAREST,
  SAMPLE_BILINEAR
};

enum PixelTypes{
  //because characters are twice as tall as they are wide, some pixel types are rectangular
  PIX_DOUBLE_SPACE,//pixels are 2x1 chars(square)
  PIX_SINGLE_SPACE,//pixels are 1x1 chars(rect)
  PIX_UPPER_HALF_BLOCK,//pixels are 1x0.5 chars(square) requires unicode
};

enum colorModes{
  CM_TRUECOLOR,
  CM_ANSICOLOR
};

enum textStyles{
  S_BOLD          = 0b00000001,
  S_DIM           = 0b00000010,
  S_ITALIC        = 0b00000100,
  S_UNDERLINE     = 0b00001000,
  S_BLINK         = 0b00010000,
  S_INVERSE       = 0b00100000,
  S_HIDDEN        = 0b01000000,
  S_STRIKETHROUGH = 0b10000000
};

// -------shelldisplay.c-------
_Noreturn void raiseError(char* fmt, ...);
void setColorMode(int mode);
void setStyle(int style);
void setColor(bool background, Color color);
void resetColor();
void clearScreen();
Color lerpColors(Color a, Color b, double x);
bool compareColors(Color a, Color b);

// -------texture.c-------
Texture createTexture(int width, int height);
void freeTexture(Texture t);
void setPixel(Texture t, int x, int y, Color c);
Color getPixel(Texture t, int x, int y);
void fillTexture(Texture t, Color c);
Color sampleTexture(Texture t, double x, double y, int method);
Texture resampleTexture(Texture t, int width, int height, int method);
void printTexture(Texture t, int pixelType);

// -------shellpng.c-------
Texture loadPng(char* filename);