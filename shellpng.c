#include "shelldisplay.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>

#include <zlib.h>

typedef uint8_t byte;

typedef struct IHDRChunk{
  unsigned int width;//4 bytes
  unsigned int height;//4 bytes
  char bitDepth;
  char colorType;
  char compressionMethod;
  char filterMethod;
  char interlaceMethod;
}IHDRChunk;

_Noreturn void raiseError(char* fmt, ...){
  fprintf(stderr, "Err: ");
  va_list argptr;
  va_start(argptr, fmt);
  vfprintf(stderr, fmt, argptr);
  va_end(argptr);
  exit(EXIT_FAILURE);
}

static void swapEndianness(unsigned int *x){
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

static byte paeth(byte a, byte b, byte c){
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

static void undoFiltering(byte *bytes, int bytesPerPixel, int iwidth, int iheight){
  ulong bytesPerRow = (iwidth*bytesPerPixel)+1;
  for(int row = 0; row<iheight; row++){
    ulong index = row*bytesPerRow;
    byte filterType = bytes[index];
    for(int i = 1; i<bytesPerRow; i++){
      byte a = 0;
      if(i>bytesPerPixel){
        a = bytes[(index+i)-bytesPerPixel];
      }
      byte b = 0;
      if(row>0){
        b = bytes[(index+i)-bytesPerPixel];
      }
      byte c = 0;
      if(row>0 && i>4){
        c = bytes[(index+i)-bytesPerRow-bytesPerPixel];
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
          raiseError("Unrecognised filter type (%d)\n", filterType);
          break;
      }
    }
  }
}

static Texture textureFromIdatData(byte* bytesCompressed, size_t numCompressedBytes, int iwidth, int iheight, int bytesPerPixel){
  //decompress the data
  size_t numBytes = 4096<<9;//big buffer, temporary solution
  byte *bytes = malloc(numBytes);
  if(bytes == NULL) raiseError("Out of memory\n");
  
  int r = uncompress((Bytef*)bytes, &numBytes, (Bytef*)bytesCompressed, numCompressedBytes);
  if(r!= Z_OK) raiseError("not enough space in decompressed data buffer\n");
  bytes = realloc(bytes, numBytes);//shrink memory to required size
  if(bytes == NULL) raiseError("Out of memory\n");
  
  undoFiltering(bytes, bytesPerPixel, iwidth, iheight);

  //load data into texture
  Texture texture;
  texture.width = iwidth;
  texture.height = iheight;
  texture.pixels = malloc(sizeof(Color)*iwidth*iheight);
  if(texture.pixels == NULL) raiseError("Out of memory\n");
  for(int row = 0; row<iheight; row++){
    ulong index = row*(bytesPerPixel*iwidth+1);
    index+=1;//first byte in each row is the filter type
    for(int col = 0; col<iwidth; col++){
      Color *c = &texture.pixels[(row*iwidth)+col];
      c->r = bytes[index+(col*4)]/255.0;
      c->g = bytes[index+1+(col*4)]/255.0;
      c->b = bytes[index+2+(col*4)]/255.0;
    }
  }
  free(bytes);
  return texture;
}

Texture loadPng(char* filename){
  FILE* file = fopen(filename, "r");

  //check signature
  char signature[9];
  fread(signature,sizeof(char), 8, file);
  signature[8] = 0;
  if(strcmp("\211PNG\r\n\032\n", signature) != 0)
    raiseError("File signature invalid");
  

  //file info
  byte bitDepth = 0;
  byte colorType = 0;
  byte samplesPerPixel = 0;
  byte bitsPerPixel = 0;
  uint32_t width, height;
  
  size_t compressedDataSize = 0;
  char* compressedData = NULL;//will hold the content of all IDAT chunks 
  while(1){
    uint32_t chunkSize;
    byte chunkType[5];
    fread(&chunkSize,sizeof(unsigned int), 1, file);
    swapEndianness(&chunkSize);
    fread(chunkType,sizeof(unsigned char), 4, file);
    chunkType[4] = 0;
    if(strcmp((const char*)chunkType, "IEND") == 0) break;

    byte* chunkData;
    if(chunkSize != 0){
      chunkData = malloc(chunkSize);
      if(chunkData == NULL) raiseError("Out of memory");
      fread(chunkData, chunkSize, 1, file);
    }
    uint32_t CRC;
    fread(&CRC,sizeof(unsigned int), 1, file);
    if(chunkSize == 0) continue;


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
      width  = *(uint32_t*)chunkData;
      height = *(uint32_t*)(chunkData+4);
      swapEndianness(&width);
      swapEndianness(&height);
      
      bitDepth = *(byte*)(chunkData+8);
      colorType = *(byte*)(chunkData+9);
      byte compressionMethod = *(byte*)(chunkData+10);
      byte filterMethod = *(byte*)(chunkData+11);
      byte interlaceMethod = *(byte*)(chunkData+12);
    
      if(compressionMethod != 0) raiseError("Unrecognised compression method\n");
      if(filterMethod != 0) raiseError("Unrecognised filter method\n");
      if(interlaceMethod != 0) raiseError("Unrecognised interlace method\n");//method 1 is also valid, implement later
      

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
    }
    if(strcmp((const char*)chunkType, "IDAT") == 0) {
      size_t oldCompressedDataSize = compressedDataSize;
      compressedDataSize += chunkSize;
      //append the chunk data to compressedData
      compressedData = realloc(compressedData, compressedDataSize);
      memcpy(&compressedData[oldCompressedDataSize], chunkData, chunkSize);
    }
    free(chunkData);
  }
  
  Texture texture = textureFromIdatData((byte*)compressedData, compressedDataSize, width, height, bitsPerPixel/8);
  free(compressedData);
  return texture;
}
