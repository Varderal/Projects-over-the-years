#include <stdio.h>
#include <string.h>

//========================================================================
//This function copies 4 bytes from in int to an unsigned char array where
//  the least significant byte of the int is placed in the first element
//  of the array.
//========================================================================
void copyIntToAddress(int n, unsigned char bytes[])
{
  bytes[0] = n & 0xFF;
  bytes[1] = (n >>  8) & 0xFF;
  bytes[2] = (n >> 16) & 0xFF;
  bytes[3] = (n >> 24) & 0xFF;
}



//========================================================================
//Sets the RGB value of a single pixel at coordinates (x,y) in the
//   character array, data in bitmap format with three bytes per pixel.
//
//Bitmap format stores rows from bottom to top. Therefore, this function
//   needs the pixelHeight to calculate the offset into data.
//
//Bitmap format requires that each row is divisible by 4. Therefore, 
//   rowSize may need to be padded up to 3 bytes past the end of the data. 
//========================================================================
void setRGB(unsigned char data[], int x, int y, int rowSize, 
            int pixelHeight,
            unsigned char r, unsigned char g, unsigned char b)
{
  y = (pixelHeight - y) - 1;
  int offset = (x * 3) + (y * rowSize);
  data[offset] = b;
  data[offset+1] = g;
  data[offset+2] = r;
}





void main(void)
{ 
  int pixelWidth = 5;
  int pixelHeight = 3;
  int rowSize = pixelWidth * 3;
  int rowPadding = 0;
  rowPadding = (4 - (rowSize % 4)) % 4;
  rowSize += rowPadding;
  int pixelDataSize = rowSize*pixelHeight;
  int fileSize = 54 + pixelDataSize;

  printf("rowPadding=%d bytes\n", rowPadding);
  printf("rowSize   =%d bytes\n", rowSize);
  
  unsigned char header[54] = 
  {
    'B','M',  // magic number
    0,0,0,0,  // size in bytes (set below)
    0,0,0,0,  // reserved
    54,0,0,0, // offset to start of pixel data
    40,0,0,0, // info hd size
    0,0,0,0,  // image width (set below)
    0,0,0,0,  // image heigth (set below)
    1,0,      // number color planes
    24,0,     // bits per pixel
    0,0,0,0,  // compression is none
    0,0,0,0,  // image byte size
    0x13,0x0B,0,0, // horz resoluition in pixel / m
    0x13,0x0B,0,0, // vert resolutions (0x03C3 = 96 dpi, 0x0B13 = 72 dpi)
    0,0,0,0,  // #colors in pallete
    0,0,0,0,  // #important colors
  };


  copyIntToAddress(fileSize, &header[2]);
  copyIntToAddress(pixelWidth, &header[18]);
  copyIntToAddress(pixelHeight, &header[22]);
  copyIntToAddress(pixelDataSize, &header[34]);

  unsigned char img[pixelDataSize];

  //Initilize all pixels to white. This also sets any row padding
  //  to 0FF, but that data is ignored by readers.
  memset(img,0xFF,sizeof(img));

  setRGB(img, 0, 1, rowSize, pixelHeight, 255, 0, 0); 
  setRGB(img, 1, 1, rowSize, pixelHeight, 0, 255, 0); 
  setRGB(img, 2, 1, rowSize, pixelHeight, 0, 255, 0); 
  setRGB(img, 3, 1, rowSize, pixelHeight, 0, 255, 0); 
  setRGB(img, 4, 1, rowSize, pixelHeight, 0, 0, 255); 
  setRGB(img, 2, 0, rowSize, pixelHeight, 255, 255, 0); 
  setRGB(img, 2, 2, rowSize, pixelHeight, 200, 0, 255); 
  
  FILE* myFile = fopen("myPic.bmp", "wb");
  fwrite(header, 1, sizeof(header), myFile);
  fwrite(img, 1, sizeof(img), myFile);
  fclose(myFile);

  if (0)
  { int i;
    for (i=0; i<54; i++)
    {
      printf("header[%2d]=%0X\n", i, header[i]);
    }
  }
}

