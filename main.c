#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

typedef struct Color{
  double r,g,b;
}Color;

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

double randDouble(double min, double max){
  double i = (double)rand()/RAND_MAX;
  return (i*(max-min))+min;
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

int time = 0;
typedef struct Texture{
  unsigned int width, height;
  Color* pixels;
}Texture;

Color shader(double x, double y){
  Color out = (Color){0.0, y, x};
  return out;
}


typedef struct PNGChunk{
  unsigned int size;//4 bytes
  unsigned char type[5];//4 bytes, 5th to terminate string for debug
  unsigned char* data;//<size> bytes
  unsigned int CRC;//4 bytes
}PNGChunk;
  //IHDR chunk
  /*
   Width:              4 bytes
   Height:             4 bytes
   Bit depth:          1 byte
   Color type:         1 byte
   Compression method: 1 byte
   Filter method:      1 byte
   Interlace method:   1 byte
  */
typedef struct IHDRChunk{
  unsigned int width;//4 bytes
  unsigned int height;//4 bytes
  char bitDepth;
  char colorType;
  char compressionMethod;
  char filterMethod;
  char interlaceMethod;
}IHDRChunk;

void swapEndianness(unsigned int *x){
  char* bytes = (char*)x;
  //swap outer bytes
  bytes[0] ^= bytes[3];
  bytes[3] ^= bytes[0];
  bytes[0] ^= bytes[3];

  //swap inner bytes
  bytes[1] ^= bytes[2];
  bytes[2] ^= bytes[1];
  bytes[1] ^= bytes[2];
}

void loadChunk(FILE* stream, PNGChunk* chunk){
  fread(&chunk->size,sizeof(unsigned int), 1, stream);
  swapEndianness(&chunk->size);
  fread(chunk->type,sizeof(unsigned char), 4, stream);
  chunk->type[4] = 0;
  chunk->data = malloc(chunk->size);
  fread(chunk->data, 1, chunk->size, stream);
  fread(&chunk->CRC,sizeof(unsigned int), 1, stream);
}

void freeChunk(PNGChunk* chunk){
  free(chunk->data);
}


Texture loadPNG(char* filename){
  FILE* file = fopen(filename, "r");
  char signature[9];
  fread(signature,sizeof(char), 8, file);
  signature[8] = 0;
  if(strcmp("\211PNG\r\n\032\n", signature) != 0){
    printf("File signature invalid");
  }

  Texture texture;
  
  PNGChunk chunk;
  while(1){
    loadChunk(file, &chunk);
    if(strcmp(chunk.type, "IEND") == 0) break;//ignore the warning, its fine
    if(strcmp(chunk.type, "IHDR") == 0) {
      IHDRChunk* ihdr = (IHDRChunk*)chunk.data;
      swapEndianness(&ihdr->width);
      swapEndianness(&ihdr->height);
      texture.width = ihdr->width;
      texture.height = ihdr->height;
      if(ihdr->compressionMethod != 0){
        printf("Unrecognised compression method\n");
      }
    }
    if(strcmp(chunk.type, "sRGB") == 0) {
    }
    if(strcmp(chunk.type, "IDAT") == 0) {
      printf("Data header\n");
      for(int j = 0; j<= chunk.size; j++){
        printf("%x ", chunk.data[j]);
      }
      
      printf("\n");
      break;
    }
    freeChunk(&chunk);   
  }

  
  return texture;
}

int main(void) {
  Texture t = loadPNG("steve.png");
  printf("width: %d height: %d\n", t.width, t.height);
  return 0;
}
