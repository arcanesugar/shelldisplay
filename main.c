#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <zlib.h>
#include "shelldisplay.h"

typedef uint8_t byte;

typedef struct Texture{
  unsigned int width, height;
  Color* pixels;
}Texture;

Color shader(double x, double y){
  Color out = (Color){0.0, y, x};
  return out;
}

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
  char* bytes = (char*)x;//more dark pointer magic
  //swap outer bytes
  bytes[0] ^= bytes[3];
  bytes[3] ^= bytes[0];
  bytes[0] ^= bytes[3];

  //swap inner bytes
  bytes[1] ^= bytes[2];
  bytes[2] ^= bytes[1];
  bytes[1] ^= bytes[2];
}
char paeth(unsigned char a, unsigned char b, unsigned char c){
    int p = a + b - c;
    int pa = abs(p - a);
    int pb = abs(p - b);
    int pc = abs(p - c);
    int Pr;
    if (pa <= pb && pa <= pc) Pr = a;
    else if (pb <= pc) Pr = b;
    else Pr = c;
    return Pr;
}

Texture decodeIdatData(byte* bytes, int iwidth, int iheight, int bytesPerPixel){
  Texture texture;
  ulong bytesPerRow = (iwidth*bytesPerPixel)+1;
  for(int row = 0; row<iheight; row++){
    ulong index = row*bytesPerRow;
    char filterType = bytes[index];
    for(int i = 1; i<bytesPerRow; i++){
      byte a = 0;
      if(i>4){
        a = bytes[(index+i)-4];
      }
      byte b = 0;
      if(row>0){
        b = bytes[(index+i)-bytesPerRow];
      }
      byte c = 0;
      if(row>0 && i>4){
        c = bytes[(index+i)-bytesPerRow-4];
      }
      switch(filterType){
        case 0://no filtering
        break;
        case 1://sub filtering (undo with addition)
          bytes[(index+i)] += a;
        break;
        case 2://up filtering (undo with addition)
          bytes[(index+i)] += b;
        break;
        case 3://average filtering (Recon(x) = Filt(x) + floor((Recon(a) + Recon(b)) / 2))
          bytes[(index+i)] += (int)((double)a + (double)b)>>1;
        break;
        case 4://paeth filtering
          bytes[(index+i)] += paeth(a, b, c);
        break;
        default:
          printf("Unrecognised filter type (%d)\n", filterType);
        break;
      }
    }
  }

  //load data into texture
  //texture.pixels = malloc(sizeof(Color)*texture.width*texture.height);
  for(int row = 0; row<iheight; row++){
    ulong index = row*bytesPerRow;
    index+=1;
    Color c;
    for(int i = 0; i<iwidth; i++){
      c.r = bytes[index+(i*4)]/255.0;
      c.g = bytes[index+1+(i*4)]/255.0;
      c.b = bytes[index+2+(i*4)]/255.0;
      double a = bytes[index+3+(i*4)]/255.0;
      truecolor(1, c);
      printf("  ");
    }
    clearFormatting();
    printf("\n");
  }
  return texture;
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
  
  size_t compressedDataSize = 0;
  char* compressedData = NULL;//will hold the content of all IDAT chunks

  int i = 0;
  unsigned char bitDepth = 0;
  unsigned char colorType = 0;
  unsigned char samplesPerPixel = 0;
  unsigned char bitsPerPixel = 0;
  while(1){
    i++;
    unsigned int chunkSize;
    unsigned char chunkType[5];
    fread(&chunkSize,sizeof(unsigned int), 1, file);
    swapEndianness(&chunkSize);
    fread(chunkType,sizeof(unsigned char), 4, file);
    chunkType[4] = 0;
    printf("chunkType: %s\n", chunkType);
    void* chunkData = malloc(chunkSize);
    fread(chunkData, chunkSize, 1, file);
    unsigned int CRC;
    fread(&CRC,sizeof(unsigned int), 1, file);


    if(strcmp((const char*)chunkType, "IHDR") == 0) {
      /*
        IHDR chunk layout
        Width 	4 bytes
        Height 	4 bytes
        Bit depth 	1 byte
        Colour type 	1 byte
        Compression method 	1 byte
        Filter method 	1 byte
        Interlace method 	1 byte
      */

      //dark pointer magic
      unsigned int* width  = (unsigned int*)chunkData;
      unsigned int* height = (unsigned int*)(chunkData+4);
      swapEndianness(width);//does affect chunkData 
      swapEndianness(height);
      
      //bad
      bitDepth = *(unsigned char*)(chunkData+8);
      colorType = *(unsigned char*)(chunkData+9);
      unsigned char compressionMethod = *(unsigned char*)(chunkData+10);
      unsigned char filterMethod = *(unsigned char*)(chunkData+11);
      unsigned char interlaceMethod = *(unsigned char*)(chunkData+12);
    
      if(compressionMethod != 0) printf("Unrecognised compression method\n");
      if(filterMethod != 0) printf("Unrecognised filter method\n");
      if(interlaceMethod != 0) printf("Unrecognised interlace method\n");//method 1 is also valid, implement later
      

      /*
        PNG image type 	Colour type 	Allowed bit depths 	Interpretation
        Greyscale	            0       1, 2, 4, 8, 16      Each pixel is a greyscale sample
        Truecolour 	          2       8, 16	              Each pixel is an R,G,B triple
        Indexed-colour 	      3       1, 2, 4, 8          Each pixel is a palette index; a PLTE chunk shall appear.
        Greyscale with alpha 	4       8, 16               Each pixel is a greyscale sample followed by an alpha sample.
        Truecolour with alpha	6       8, 16               Each pixel is an R,G,B triple followed by an alpha sample.
      */
      const char spp[7] = {1,0,3,1,2,0,4};
      samplesPerPixel = spp[colorType];
      bitsPerPixel = samplesPerPixel*bitDepth;
      printf("bitDepth: %d\n", bitDepth);
      printf("colorType: %d\n", colorType);
      texture.width = *width;
      texture.height = *height;
    }
    if(strcmp((const char*)chunkType, "IDAT") == 0) {
      size_t oldCompressedDataSize = compressedDataSize;
      compressedDataSize += chunkSize;
      //append the chunk data to compressedData
      compressedData = realloc(compressedData, compressedDataSize);
      memcpy(&compressedData[oldCompressedDataSize], chunkData, chunkSize);
    }
    free(chunkData);
    if(strcmp((const char*)chunkType, "IEND") == 0) break;//ignore the warning, its fine
  }
  
  //decompress the data
  size_t decompressedDataLen = 4096<<2;
  unsigned char *decompressedData = malloc(decompressedDataLen);//enough for a small image
  
  int r = uncompress((Bytef*)decompressedData, &decompressedDataLen, (Bytef*)compressedData, compressedDataSize);
  if(r!= Z_OK) printf("not enough space in decompressed data buffer\n");
  free(compressedData);
  texture = decodeIdatData(decompressedData, texture.width, texture.height, 4);
  free(decompressedData);
  return texture;
}

int main(void) {
  Texture t = loadPNG("test.png");
  printf("width: %d height: %d\n", t.width, t.height);
  return 0;
}
