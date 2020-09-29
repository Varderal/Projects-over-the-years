#include <stdio.h>
#include <stdlib.h>

#define HEADER_SIZE 54

int byteArrayToInt(unsigned char* a)
{
   return a[0] | (a[1]<<8) | (a[2]<<16) | (a[3]<<24);
}


void main(void)
{ 
  int const BYTES_PER_PIXEL = 3;
  FILE *inFile = fopen("Sword_8x8.bmp","rb");


  unsigned char header[HEADER_SIZE];
  fread(header, sizeof(char), HEADER_SIZE, inFile);
  
 
  printf("%c%c\n", header[0],  header[1]);



  int imageWidth  = byteArrayToInt(&header[18]);
  int imageHeight = byteArrayToInt(&header[22]);

  //Each row in the Pixel array is padded to a multiple of 4 bytes in size.
  int rowBytes = imageWidth*BYTES_PER_PIXEL;
  if (rowBytes % 4) rowBytes += 4 - (rowBytes % 4);

  printf("imageWidth=%d, imageHeight=%d\n",imageWidth, imageHeight);

  int dataSize = imageWidth*rowBytes;
  unsigned char* data = malloc(dataSize);

  fread(data, sizeof(dataSize), HEADER_SIZE, inFile);

  fclose(inFile);


  int x, y;
  for (y=imageHeight-1; y>=0; y--)
  {
    for (x=0; x<imageWidth; x++)
    {
      int idx = ((imageHeight-1)-y)*rowBytes + x*BYTES_PER_PIXEL;	    
      unsigned char b = data[idx];
      unsigned char g = data[idx+1];
      unsigned char r = data[idx+2];

      printf("[%3d, %3d, %3d]   ", r, g, b);   
    }
    printf("\n");
  }
}

