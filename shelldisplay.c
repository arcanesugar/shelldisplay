#include "shelldisplay.h"

#include <stdio.h>


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

void drawCanvas(Color (*pixelShader)(double, double), int width, int height){
  for(int y = 0; y<height; y++){
    for(int x = 0; x<width; x++){
      double screenspaceX = (double)x/(double)width;
      double screenspaceY = (double)y/(double)height;
      Color color = pixelShader(screenspaceX, screenspaceY);
      truecolor(0, color);
      //setColor(1, color);
      printf(":3");
    } 
    clearFormatting();
    printf("\n");
  }
  clearFormatting();
  
};
