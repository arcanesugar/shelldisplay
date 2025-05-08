#include "shelldisplay.h"
#include <stdio.h>
#include <math.h>

void setColor(bool background, Color color){
  int r = color.r*5.0;
  int g = color.g*5.0;
  int b = color.b*5.0;
  int colorID = 16 + 36*r + 6*g + b;
  if(background) printf("\x1b[48;5;%dm", colorID);
  else printf("\x1b[38;5;%dm", colorID);
}

void truecolor(bool background, Color color){
  int r = color.r*255.0;
  int g = color.g*255.0;
  int b = color.b*255.0;
  if(background) printf("\x1b[48;2;%d;%d;%dm", r, g, b);
  else printf("\x1b[38;2;%d;%d;%dm", r, g, b);
}

void clearFormatting(){
  printf("\x1b[0m");
}

void clearScreen(){
  printf("\033[2J");//clear screen
  printf("\033[H");//return cursor home
}

Color lerpColors(Color a, Color b, double x){
  if(x<0 || x>1) raiseError("lerpColors(): x out of range (%f)", x);
  Color c;
  c.r = (a.r+(b.r-a.r)*x);
  c.g = (a.g+(b.g-a.g)*x);
  c.b = (a.b+(b.b-a.b)*x);
  return c;
}

bool compareColors(Color a, Color b){
  if(a.r != b.r) return false;
  if(a.g != b.g) return false;
  if(a.b != b.b) return false;
  return true;
}

Color sampleBilinear(Texture *t, double x, double y);
Color sampleNearest(Texture *t, int x, int y);

Color sampleTexture(Texture *t, double x, double y, int method){
  switch(method){
    case SAMPLE_NEAREST:
      return sampleNearest(t, x*t->width, y*t->height);
    case SAMPLE_BILINEAR:
      return sampleBilinear(t, x, y);
    default:
      raiseError("Unrecognised sample type\n");
  }
}

Color sampleNearest(Texture *t, int x, int y){

  if(y<t->width && x<t->height && x>=0 && y>=0) return t->pixels[(y*t->width)+x];
  return (Color){0, 0, 0};
}

Color sampleBilinear(Texture *t, double x, double y){
  //  a --e-- b
  //  |   |  |
  //  |   g  |
  //  |   |  |
  //  c --f-- d
  int ax, ay, bx, by, cx, cy, dx, dy;
  ax = floor(x*(t->width-1));
  ay = floor(y*(t->height-1));
  dx = ax+1;
  dy = ay+1;
  //printf(" %d, %d, %d, %d |", ax, ay, dx, dy);
  bx = dy;
  by = ay;

  cx = ax;
  cy = dy;
  
  Color a = sampleNearest(t, ax, ay);
  Color b = sampleNearest(t, bx, by);
  Color c = sampleNearest(t, cx, cy);
  Color d = sampleNearest(t, dx, dy);
  
  Color e = lerpColors(a, b, (x*(t->width-1))-ax);
  Color f = lerpColors(c, d, (x*(t->width-1))-ax);

  return lerpColors(e, f, (y*(t->height-1))-ay);//g
}
