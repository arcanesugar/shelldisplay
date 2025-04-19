#pragma once
#include <stdbool.h>

typedef struct Color{
  double r,g,b;
}Color;


void drawCanvas(Color (*pixelShader)(double, double), int width, int height);
void setColor(bool background, Color color);
void truecolor(bool background, Color color);
void clearFormatting();
void clampColor(Color *c);
Color addColors(Color a, Color b);
Color subColors(Color a, Color b);
Color sMultColor(Color color, double x);
Color cMultColor(Color a, Color b);
Color lerpColor(Color a, Color b, double x);
