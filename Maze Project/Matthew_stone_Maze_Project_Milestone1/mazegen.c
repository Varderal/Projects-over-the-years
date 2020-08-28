/***************************************************************/
/*  Maze generator project                                     */
/*  CS 241-007                                                 */
/*  Matthew Stone                                              */
/*  Made on 4/6/2017                                           */
/***************************************************************/

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "mazegen.h"

#define START 128     //1000 0000

//functions
void maze_carve(int width, int height,              // [3, 1000],  [3, 1000]
                int wayPointX, int wayPointY,       // [1, width],   [1, height]
        //I'm indexing from 0, so [0,width-1],[0,height-1]
                int wayPointAlleyLength,            // [0,  min(width, height)/2 ]
                double wayPointDirectionPercent,    // [0.0,  1.0]
                double straightProbability);        // [0.0,  1.0]
void interpret_cells(int width, int height);

//global variables
int* maze_cells;
char* maze;

int startX;
int endX;

int freed = 1;

int x_size, y_size, point_x, point_y;

//void set_values(int *width, int *height,
//                int *waypoint_x, int *waypoint_y,
//                int*waypoint_ally_length,
//                double *waypoint_direction_percent, double *straight_probillity,
//                int *print_algorithm_steps);
////both textcolor and main used for testing. Will comment out before turning in
//void textcolor(int color)
//{
//  //30	Black
//  //31	Red
//  //32	Green
//  //33	Yellow
//  //34	Blue
//  //35	Magenta
//  //36	Cyan
//  //37	White
//  printf("%c[%d;%d;%dm", 0x1B, 0, color, 40);
//}
//int main()
//{
//  srand((unsigned)time(NULL));
//  int size_x, size_y;
//
//  int waypoint_x, waypoint_y;
//  int waypoint_ally_length;
//  double waypoint_direction_percent, straight_probillity;
//  int print_algorithm_steps;
//
//  set_values(&size_x, &size_y,
//             &waypoint_x, &waypoint_y,
//             &waypoint_ally_length, &waypoint_direction_percent,
//             &straight_probillity, &print_algorithm_steps);
//
//  mazeGenerate(size_x, size_y, waypoint_x, waypoint_y,
//               waypoint_ally_length, waypoint_direction_percent,
//               straight_probillity, print_algorithm_steps);
//
//  mazePrint(size_x, size_y, waypoint_x, waypoint_y);
//
//  //mazeSolve();
//  mazeSolve(size_x, size_y, startX, 0, SOUTH);
//
//  mazePrint(size_x, size_y, waypoint_x, waypoint_y);
//
//  return 0;
//}
//
//
////This function is to set up the values of the variables sent to mazegen;
////  at first most will be hardcoded but later on they will be more randomly
////  generated. This function is for testing purposes
////it Is also not called unless I'm using my testing main
//void set_values(int *width, int *height,
//                int *waypoint_x, int *waypoint_y,
//                int*waypoint_ally_length,
//                double *waypoint_direction_percent, double *straight_probillity,
//                int *print_algorithm_steps)
//{
//  //*width = (rand()%97) + 3;
//  //*height = (rand()%37) + 3;
//  *width =  4;
//  *height = 4;
//  //*waypoint_x = (rand() % (*width));
//  //*waypoint_y = (rand() % (*height));
//  *waypoint_x = 0;
//  *waypoint_y = 0;
//  *waypoint_ally_length = 2;
//  *waypoint_direction_percent = 0.16;
//  *straight_probillity = 0.4;
//  *print_algorithm_steps = TRUE;
//
//  //debugging
//  //printf("========================\n");
//  //printf("set variables:(%d x %d): (%d, %d)\n",
//  //       *width, *height, *waypoint_x, *waypoint_y);
//  //printf("========================\n");
//  //debugging
//}

int mazeGenerate(int width, int height,              // [3, 1000],  [3, 1000]
                 int wayPointX, int wayPointY,       // [1, width],   [1, height]
                                                     // I'm indexing from 0, so [0,width-1],[0,height-1]
                 int wayPointAlleyLength,            // [0,  min(width, height)/2 ]
                 double wayPointDirectionPercent,    // [0.0,  1.0]
                 double straightProbability,         // [0.0,  1.0]
                 int printAlgorithmSteps)            // [TRUE | FALSE]
{
  mazeFree();

  //printf("width = %d, height = %d\n", width, height);

  //check input
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