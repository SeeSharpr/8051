#include"kernel.h"

void TaskA( void )
{
  int i = 0;
  
  while (i < 25)
  {
     i++;
  }
  EndTask();
}

void TaskB( void )
{
  int i = 0;
  
  while (i < 10)
  {
     i++;
  }
  EndTask();
}

void TaskC( void )
{
  int i = 0;
  
  while (i < 50)
  {
     i++;
  }
  EndTask();
}

void main( void )
{
  InitKernel();
  AddTask(mstoclk(1000),TaskA);
  AddTask(mstoclk(2000),TaskB);
  AddTask(mstoclk(5000),TaskC);
  RunKernel();
}

