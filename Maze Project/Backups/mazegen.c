/***************************************************************/
/*  Maze generator project                                     */
/*  CS 241-007                                                 */
/*  Matthew Stone                                              */
/*  Made on 4/6/2017                                           */
/***************************************************************/

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "mazegen.h"

#define START 128     //1000 0000
#define HEADER_SIZE 54
#define MIN(a,b) (((a)<(b))?(a):(b))

//functions
void maze_carve(int width, int height,              // [3, 1000],  [3, 1000]
                int wayPointX, int wayPointY,       // [1, width],   [1, height]
        //I'm indexing from 0, so [0,width-1],[0,height-1]
                int wayPointAlleyLength,            // [0,  min(width, height)/2 ]
                double wayPointDirectionPercent,    // [0.0,  1.0]
                double straightProbability);        // [0.0,  1.0]
void interpret_cells(int width, int height);
//borrowed these ones from Joel's programs
int byteArrayToInt(unsigned char* a);
void copyIntToAddress(int n, unsigned char bytes[]);
void setRGB(unsigned char data[], int x, int y, int rowSize,
            int pixelHeight,
            unsigned char r, unsigned char g, unsigned char b);

//global variables
int* maze_cells;
char* maze;

int startX;
int endX;

int freed = 1;

int x_size, y_size, point_x, point_y;

//rgb for pixel reading and writing
unsigned char b, g, r;

//debugging function declaration
void set_values(int *width, int *height,
                int *waypoint_x, int *waypoint_y,
                int*waypoint_ally_length,
                double *waypoint_direction_percent, double *straight_probillity,
                int *print_algorithm_steps);
//both textcolor and main used for testing. Will comment out before turning in
void textcolor(int color)
{
  //30	Black
  //31	Red
  //32	Green
  //33	Yellow
  //34	Blue
  //35	Magenta
  //36	Cyan
  //37	White
  printf("%c[%d;%d;%dm", 0x1B, 0, color, 40);
}
int main()
{
  srand((unsigned)time(NULL));
  int size_x, size_y;

  int waypoint_x, waypoint_y;
  int waypoint_ally_length;
  double waypoint_direction_percent, straight_probillity;
  int print_algorithm_steps;

  set_values(&size_x, &size_y,
             &waypoint_x, &waypoint_y,
             &waypoint_ally_length, &waypoint_direction_percent,
             &straight_probillity, &print_algorithm_steps);

  mazeGenerate(size_x, size_y, waypoint_x, waypoint_y,
               waypoint_ally_length, waypoint_direction_percent,
               straight_probillity, print_algorithm_steps);

  mazePrint();

  //mazeSolve();
  mazeSolve();

  mazePrint();

  return 0;
}


//This function is to set up the values of the variables sent to mazegen;
//  at first most will be hardcoded but later on they will be more randomly
//  generated. This function is for testing purposes
//it Is also not called unless I'm using my testing main
void set_values(int *width, int *height,
                int *waypoint_x, int *waypoint_y,
                int*waypoint_ally_length,
                double *waypoint_direction_percent, double *straight_probillity,
                int *print_algorithm_steps)
{
  //*width = (rand()%97) + 3;
  //*height = (rand()%37) + 3;
  *width =  4;
  *height = 4;
  //*waypoint_x = (rand() % (*width));
  //*waypoint_y = (rand() % (*height));
  *waypoint_x = 0;
  *waypoint_y = 0;
  *waypoint_ally_length = 2;
  *waypoint_direction_percent = 0.16;
  *straight_probillity = 0.4;
  *print_algorithm_steps = TRUE;

  //debugging
  //printf("========================\n");
  //printf("set variables:(%d x %d): (%d, %d)\n",
  //       *width, *height, *waypoint_x, *waypoint_y);
  //printf("========================\n");
  //debugging
}

int mazeGenerate(int width, int height,              // [3, 1000],  [3, 1000]
                 int wayPointX, int wayPointY,       // [1, width],   [1, height]
                                                     // I'm indexing from 0, so [0,width-1],[0,height-1]
                 int wayPointAlleyLength,            // [0,  min(width, height)/2 ]
                 double wayPointDirectionPercent,    // [0.0,  1.0]
                 double straightProbability,         // [0.0,  1.0]
                 int printAlgorithmSteps)            // [TRUE | FALSE]
{
  mazeFree();

  if((width < 3) || (width > 1000))
  {
    return TRUE;
  }
  if((height < 3) || (height > 1000))
  {
    return TRUE;
  }
  if((wayPointX < 1) || (wayPointX > width))
  {
    return TRUE;
  }
  if((wayPointY < 1) || (wayPointY > height))
  {
    return TRUE;
  }
  if((wayPointAlleyLength < 0) || (wayPointAlleyLength > MIN(width, height)/2))
  {
    return TRUE;
  }
  if((wayPointDirectionPercent < 0.0) || (wayPointDirectionPercent > 1.0))
  {
    return TRUE;
  }
  if((straightProbability < 0.0) || (straightProbability > 1.0))
  {
    return TRUE;
  }
  if((printAlgorithmSteps != TRUE) && (printAlgorithmSteps != FALSE))
  {
    return TRUE;
  }

  maze_cells = malloc(sizeof(int*)*width*height);
  maze = malloc(sizeof(char*)*width*height);
  freed = 0;

  int i;
  int j;
  for(i = 0; i < width; i++)
  {
    //printf("i = %d\n", i);
    for(j = 0; j < height; j++)
    {
      //printf("j = %d\n", j);
      *(maze_cells + j*width + i) = NO_DIRECTIONS;
    }
  }

  maze_carve(width, height, wayPointX, wayPointY, wayPointAlleyLength,
             wayPointDirectionPercent, straightProbability);

  //pick random spot in top and bottom for entrance and exit
  
  x_size = width;
  y_size = height;
  point_x = wayPointX;
  point_y = wayPointY;

  startX = rand()%(width - 1);
  endX = rand()%(width - 1);
  *(maze_cells + 0*width + startX) += NORTH;
  *(maze_cells + (height-1)*width + endX) += SOUTH;



  //Debugging ========================================
  //printf("maze_cells:\n");
  //for(i = 0; i < width; i++)
  //{
  //  for(j = 0; j < height; j++)
  //  {
  //    printf("(%d)", *(maze_cells + i*width + j));
  //  }
  //  printf("\n");
  //}
  //Debugging ========================================
  //interpret maze_cells into the corresponding characters
  interpret_cells(width, height);

  *(maze_cells + 0*width + startX) += START;
  *(maze_cells + (height-1)*width + endX) += GOAL;

  //mazeSolve();

  return FALSE;
}

//recursive function to build the maze
void maze_carve(int width, int height,              // [3, 1000],  [3, 1000]
                int wayPointX, int wayPointY,       // [1, width],   [1, height]
                                                    //I'm indexing from 0, so [0,width-1],[0,height-1]
                int wayPointAlleyLength,            // [0,  min(width, height)/2 ]
                double wayPointDirectionPercent,    // [0.0,  1.0]
                double straightProbability)         // [0.0,  1.0]
{
  int left, right, top, bottom;
  left = right = top = bottom = 0;
  //check if on edge yet
  //  --set variables if on right, bottom, left, or top
  if(wayPointX == 0)
  {
    left = 1;
  }
  else if(wayPointX == (width - 1))
  {
    right = 1;
  }
  if(wayPointY == 0)
  {
    top = 1;
  }
  else if(wayPointY == height-1)
  {
    bottom = 1;
  }
  //if not check to see if each tile in the 4 directions is carved already
  //  --check right - if not on right
  int order[4];
  int temp = -1;

  temp = order[0] = rand()%4;
  while(temp == order[0])
  {
    temp = rand()%4;
  }
  order[1] = temp;
  while((temp == order[0]) || (temp == order[1]))
  {
    temp = rand()%4;
  }
  order[2] = temp;
  while((temp == order[0]) || (temp == order[1]) || (temp == order[2]))
  {
    temp = rand()%4;
  }
  order[3] = temp;
  //DEBUGGING
  //int j;
  //printf("order: ");
  //for(j = 0; j < 4; j ++)
  //{
  //  printf("%d, ", order[j]);
  //}
  //printf("\n");
  //DEBUGGING

  int i;
  for(i = 0; i < 4; i++)
  {
    switch(order[i])
    {
      case 0:
        if(right == 0)
        {
          if(*(maze_cells + (wayPointY)*width + wayPointX+1) == NO_DIRECTIONS)
          {
            *(maze_cells + (wayPointY)*width + wayPointX) += EAST;
            *(maze_cells + (wayPointY)*width + wayPointX+1) += WEST;
            maze_carve(width, height, wayPointX+1, wayPointY, wayPointAlleyLength,
                       wayPointDirectionPercent, straightProbability);
          }
        }
        break;
      case 1:
        if(bottom == 0)
        {
          if(*(maze_cells + (wayPointY+1)*width + wayPointX) == NO_DIRECTIONS)
          {
            *(maze_cells + (wayPointY)*width + wayPointX) += SOUTH;
            *(maze_cells + (wayPointY+1)*width + wayPointX) += NORTH;
            maze_carve(width, height, wayPointX, wayPointY+1, wayPointAlleyLength,
                       wayPointDirectionPercent, straightProbability);
          }
        }
        break;
      case 2:
        if(left == 0)
        {
          if(*(maze_cells + (wayPointY)*width + wayPointX-1) == NO_DIRECTIONS)
          {
            *(maze_cells + (wayPointY)*width + wayPointX) += WEST;
            *(maze_cells + (wayPointY)*width + wayPointX-1) += EAST;
            maze_carve(width, height, wayPointX-1, wayPointY, wayPointAlleyLength,
                       wayPointDirectionPercent, straightProbability);
          }
        }
        break;
      case 3:
        if(top == 0)
        {
          if(*(maze_cells + (wayPointY-1)*width + wayPointX) == NO_DIRECTIONS)
          {
            *(maze_cells + (wayPointY)*width + wayPointX) += NORTH;
            *(maze_cells + (wayPointY-1)*width + wayPointX) += SOUTH;
            maze_carve(width, height, wayPointX, wayPointY-1, wayPointAlleyLength,
                       wayPointDirectionPercent, straightProbability);
          }
        }
        break;
    }
  }

  //once all is done, return
}

//What this function does is take maze_Cells and interpret
//  them into the characters
//Right now it does it based on printing itself but
//  but soon it will fill another array and then print from that
void interpret_cells(int width, int height)
{
  int i, j;
  for(i = 0; i < height; i++)
  {
    for(j = 0; j < width; j++)
    {
      switch(*(maze_cells + i*width + j))//*(maze + i*width + j)
      {
        case NO_DIRECTIONS:
          //printf("%c", (unsigned char)219);
          *(maze + i*width + j) = (unsigned char)219;
          break;
        case NORTH:
          //printf("%c", (unsigned char)208);
          *(maze + i*width + j) = (unsigned char)208;
          break;
        case EAST:
          //printf("%c", (unsigned char)198);
          *(maze + i*width + j) = (unsigned char)198;
          break;
        case EAST + NORTH:
          //printf("%c", (unsigned char)200);
          *(maze + i*width + j) = (unsigned char)200;
          break;
        case SOUTH:
          //printf("%c", (unsigned char)210);
          *(maze + i*width + j) = (unsigned char)210;
          break;
        case SOUTH + NORTH:
          //printf("%c", (unsigned char)186);
          *(maze + i*width + j) = (unsigned char)186;
          break;
        case SOUTH + EAST:
          //printf("%c", (unsigned char)201);
          *(maze + i*width + j) = (unsigned char)201;
          break;
        case SOUTH + EAST + NORTH:
          //printf("%c", (unsigned char)204);
          *(maze + i*width + j) = (unsigned char)204;
          break;
        case WEST:
          //printf("%c", (unsigned char)181);
          *(maze + i*width + j) = (unsigned char)181;
          break;
        case WEST + NORTH:
          //printf("%c", (unsigned char)188);
          *(maze + i*width + j) = (unsigned char)188;
          break;
        case WEST + EAST:
          //printf("%c", (unsigned char)205);
          *(maze + i*width + j) = (unsigned char)205;
          break;
        case WEST + EAST + NORTH:
          //printf("%c", (unsigned char)202);
          *(maze + i*width + j) = (unsigned char)202;
          break;
        case WEST + SOUTH:
          //printf("%c", (unsigned char)187);
          *(maze + i*width + j) = (unsigned char)187;
          break;
        case WEST + SOUTH + NORTH:
          //printf("%c", (unsigned char)185);
          *(maze + i*width + j) = (unsigned char)185;
          break;
        case WEST + SOUTH + EAST:
          //printf("%c", (unsigned char)203);
          *(maze + i*width + j) = (unsigned char)203;
          break;
        case ALL_DIRECTIONS:
          //printf("%c", (unsigned char)206);
          *(maze + i*width + j) = (unsigned char)206;
          break;
      }
    }
    //printf("\n");
  }
}

void mazePrint()
{
  printf("\n\n");
  printf("========================\n");
  printf("Maze(%d x %d): (%d, %d)\n",
         x_size, y_size, point_x, point_y);
  printf("========================\n");
  int i, j;
  for(i = 0; i < y_size; i++)
  {
    for(j = 0; j < x_size; j++)
    {
      if(*(maze_cells + i * x_size + j) & VISITED)
      {
        textcolor(TEXTCOLOR_GREEN);
      }
      if((j == point_x) && (i == point_y))
      {
        textcolor(TEXTCOLOR_RED);
      }
      printf("%c", *(maze + i * x_size + j));
      textcolor(TEXTCOLOR_WHITE);
    }
    printf("\n");
  }
  
  //open tile files
  //  14 unvisited and 13 visited varieties
  //create header for maze bmp
  //  use Joel's programs
  //read maze[] and load in the maze tile data in to maze bmp
  //for 1 to maze_width    i
  //  for 1 to maze_width  j
  //    for 0 to 8         k
  //      for 0 to 8       m
  //        setRGB(maze_img[(8*i) + k][(8*j) + m])

  int pixelWidth = x_size*8;
  int pixelHeight = y_size*8;
  int rowSize = pixelWidth * 3;
  int rowPadding = 0;
  rowPadding = (4 - (rowSize % 4)) % 4;
  rowSize += rowPadding;
  int pixelDataSize = rowSize*pixelHeight;
  int fileSize = 54 + pixelDataSize;

  printf("rowPadding = %d bytes\n", rowPadding);
  printf("rowSize    = %d bytes\n", rowSize);

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

  unsigned char maze_img[pixelDataSize];

  //Initialize all pixels to white. This also sets any row padding
  //  to 0FF, but that data is ignored by readers.
  memset(maze_img,0xFF,sizeof(maze_img));

  int const BYTES_PER_PIXEL = 3;

  for(i = 1; i < y_size; i++)
  {
    for(j = 1; j < x_size; j++)
    {
      char file_in[30];
      switch((unsigned char)*(maze + i * x_size + j))
      {
        case 219:
          strcpy(file_in, "no_direction.bmp");
          break;
        case 208:
          strcpy(file_in, "north.bmp");
          if(*(maze_cells + i * x_size + j) & VISITED)
          {
            strcpy(file_in, strcat("s_", file_in));
          }
          break;
        case 198:
          strcpy(file_in, "east.bmp");
          if(*(maze_cells + i * x_size + j) & VISITED)
          {
            strcpy(file_in, strcat("s_", file_in));
          }
          break;
        case 200:
          strcpy(file_in, "north_east.bmp");
          if(*(maze_cells + i * x_size + j) & VISITED)
          {
            strcpy(file_in, strcat("s_", file_in));
          }
          break;
        case 210:
          strcpy(file_in, "south.bmp");
          if(*(maze_cells + i * x_size + j) & VISITED)
          {
            strcpy(file_in, strcat("s_", file_in));
          }
          break;
        case 186:
          strcpy(file_in, "south_north.bmp");
          if(*(maze_cells + i * x_size + j) & VISITED)
          {
            strcpy(file_in, strcat("s_", file_in));
          }
          break;
        case 201:
          strcpy(file_in, "south_east.bmp");
          if(*(maze_cells + i * x_size + j) & VISITED)
          {
            strcpy(file_in, strcat("s_", file_in));
          }
          break;
        case 181:
          strcpy(file_in, "west.bmp");
          if(*(maze_cells + i * x_size + j) & VISITED)
          {
            strcpy(file_in, strcat("s_", file_in));
          }
          break;
        case 188:
          strcpy(file_in, "west_north.bmp");
          if(*(maze_cells + i * x_size + j) & VISITED)
          {
            strcpy(file_in, strcat("s_", file_in));
          }
          break;
        case 205:
          strcpy(file_in, "west_east.bmp");
          if(*(maze_cells + i * x_size + j) & VISITED)
          {
            strcpy(file_in, strcat("s_", file_in));
          }
          break;
        case 202:
          strcpy(file_in, "west_north_east.bmp");
          if(*(maze_cells + i * x_size + j) & VISITED)
          {
            strcpy(file_in, strcat("s_", file_in));
          }
          break;
        case 187:
          strcpy(file_in, "west_south.bmp");
          if(*(maze_cells + i * x_size + j) & VISITED)
          {
            strcpy(file_in, strcat("s_", file_in));
          }
          break;
        case 185:
          strcpy(file_in, "west_north_south.bmp");
          if(*(maze_cells + i * x_size + j) & VISITED)
          {
            strcpy(file_in, strcat("s_", file_in));
          }
          break;
        case 203:
          strcpy(file_in, "west_east_south.bmp");
          if(*(maze_cells + i * x_size + j) & VISITED)
          {
            strcpy(file_in, strcat("s_", file_in));
          }
          break;
        case 206:
          strcpy(file_in, "all_directions.bmp");
          if(*(maze_cells + i * x_size + j) & VISITED)
          {
            strcpy(file_in, strcat("s_", file_in));
          }
          break;
      }
      FILE *inFile = fopen(file_in,"rb");
      unsigned char header[HEADER_SIZE];
      fread(header, sizeof(char), HEADER_SIZE, inFile);

      int imageWidth  = byteArrayToInt(&header[18]);
      int imageHeight = byteArrayToInt(&header[22]);

      //Each row in the Pixel array is padded to a multiple of 4 bytes in size.
      int rowBytes = imageWidth*BYTES_PER_PIXEL;
      if (rowBytes % 4) rowBytes += 4 - (rowBytes % 4);

      //printf("imageWidth=%d, imageHeight=%d\n",imageWidth, imageHeight);

      int dataSize = imageWidth*rowBytes;
      unsigned char* data = malloc(dataSize);

      fread(data, sizeof(dataSize), HEADER_SIZE, inFile);

      fclose(inFile);

      int x, y;
      for(y=imageHeight-1; y>=0; y--)
      {
        for (x=0; x<imageWidth; x++)
        {
          int idx = ((imageHeight-1)-y)*rowBytes + x*BYTES_PER_PIXEL;
          unsigned char b = data[idx];
          unsigned char g = data[idx+1];
          unsigned char r = data[idx+2];

          setRGB(maze_img, (j*8)+x, (i*8)+y, rowSize, pixelHeight, r, g, b);
        }
      }
    }
  }

  FILE* mazeFile = fopen("maze_out.bmp", "wb");
  fwrite(header, 1, sizeof(header), mazeFile);
  fwrite(maze_img, 1, sizeof(maze_img), mazeFile);
  fclose(mazeFile);
}


//  -move direction of facing while left of facing is wall
//    --move in direction of facing is switch(facing)
//      --case direction:
//    --mark as visited as you go
//  -if wall no longer on right turn right
//    --wall on right = cell does not have right as direction
//      --left means divide facing by 2, unless facing = NORTH, then reset to EAST
//  -if wall on right, but can't go facing, turn left
//    --wall on right but can't go facing = cell does not have facing or right
//      --turn right means multiply facing by 2, unless facing = EAST, then set to NORTH
//  -If current cell is already visited, remove visited from previous cell
void mazeSolve()
{
  int posX, posY, direction;
  posX = startX;
  posY = 0;
  direction = SOUTH;
  
  *(maze_cells + (posY)*x_size + posX) += VISITED;
  *(maze_cells + (posY)*x_size + posX) -= NORTH;

  //*(maze_cells + (posY)*width + posX)

  //while current cell is not goal
  while((posX != endX) || (posY != y_size))
  {
    //DEBUGGING ==================================================================
    //mazePrint(width, height, posX, posY);
    //DEBUGGING ==================================================================
    //  -move direction of facing while left of facing is wall
    //    --move in direction of facing is switch(facing)
    //      --case direction:
    //    --mark as visited as you go
    //  -if wall no longer on right turn right
    //    --wall on right = cell does not have right as direction
    //      --left means divide facing by 2, unless facing = NORTH, then reset to EAST
    //  -if wall on right, but can't go facing, turn left
    //    --wall on right but can't go facing = cell does not have facing or right
    //      --turn right means multiply facing by 2, unless facing = EAST, then set to NORTH
    //  -If next cell is already visited, remove visited from current cell
    //    --OR perhaps It'll be if previous and current are visited
    //update facing
    switch(direction)
    {
      case NORTH:
        if(*(maze_cells + (posY)*x_size + posX) & EAST)
        {
          direction = EAST;
        }
        else
        {
          if(*(maze_cells + (posY)*x_size + posX) & NORTH)
          {
            break;
          }
          else if(*(maze_cells + (posY)*x_size + posX) & WEST)
          {
            direction = WEST;
          }
          else
          {
            direction = SOUTH;
          }
        }
        break;
      case WEST:
        if(*(maze_cells + (posY)*x_size + posX) & NORTH)
        {
          direction = NORTH;
        }
        else
        {
          if(*(maze_cells + (posY)*x_size + posX) & WEST)
          {
            break;
          }
          else if(*(maze_cells + (posY)*x_size + posX) & SOUTH)
          {
            direction = SOUTH;
          }
          else
          {
            direction = EAST;
          }
        }
        break;
      case SOUTH:
        if(*(maze_cells + (posY)*x_size + posX) & WEST)
        {
          direction = WEST;
        }
        else
        {
          if(*(maze_cells + (posY)*x_size + posX) & SOUTH)
          {
            break;
          }
          else if(*(maze_cells + (posY)*x_size + posX) & EAST)
          {
            direction = EAST;
          }
          else
          {
            direction = NORTH;
          }
        }
        break;
      case EAST:
        if(*(maze_cells + (posY)*x_size + posX) & SOUTH)
        {
          direction = SOUTH;
        }
        else
        {
          if(*(maze_cells + (posY)*x_size + posX) & EAST)
          {
            break;
          }
          else if(*(maze_cells + (posY)*x_size + posX) & NORTH)
          {
            direction = NORTH;
          }
          else
          {
            direction = WEST;
          }
        }
        break;
    }
    //update location
    switch(direction)
    {
      case NORTH:
        posY -= 1;
        if(*(maze_cells + (posY)*x_size + posX) & VISITED)
        {
          *(maze_cells + (posY+1)*x_size + posX) -= VISITED;
        }
        break;
      case WEST:
        posX -= 1;
        if(*(maze_cells + (posY)*x_size + posX) & VISITED)
        {
          *(maze_cells + (posY)*x_size + posX+1) -= VISITED;
        }
        break;
      case SOUTH:
        posY += 1;
        if(*(maze_cells + (posY)*x_size + posX) & VISITED)
        {
          *(maze_cells + (posY-1)*x_size + posX) -= VISITED;
        }
        break;
      case EAST:
        posX += 1;
        if(*(maze_cells + (posY)*x_size + posX) & VISITED)
        {
          *(maze_cells + (posY)*x_size + posX-1) -= VISITED;
        }
        break;
    }
    *(maze_cells + (posY)*x_size + posX) = *(maze_cells + (posY)*x_size + posX) | VISITED;
  }
  
  //when at goal, mark current cell as visited, then return
  *(maze_cells + (posY)*x_size + posX) = *(maze_cells + (posY)*x_size + posX) | VISITED;
  
  *(maze_cells + (0)*x_size + startX) += NORTH;
}

void mazeFree()
{
  if(freed = 0)
  {
    free(maze_cells);
    free(maze);
    freed = 1;
  }
}

int byteArrayToInt(unsigned char* a)
{
  return a[0] | (a[1]<<8) | (a[2]<<16) | (a[3]<<24);
}

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