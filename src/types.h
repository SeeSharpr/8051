#ifndef _types_h
#define _types_h

#include<limits.h>
#include"kernel.h"

#define NOTASK		UCHAR_MAX
#define MAXPERIOD	ULONG_MAX

typedef unsigned char Byte;
typedef unsigned int Word;
typedef unsigned long DWord;

typedef union
{
  struct { Byte h,l; } byte;
  Word word;
} Reg16;

typedef union
{
  struct { Byte hh,hl,lh,ll; } byte;
  struct { Word h,l; } word;
  DWord dword;
} Reg32;

typedef struct
{
	Word PC,DPTR;
	Byte A,B,R0,R1,R2,R3,R4,R5,R6,R7;
	Byte PSW;
} TContext;

typedef TContext* PContext;

typedef struct
{
  /* Dados individuais de cada tarefa */
  TTime Period;
  /* Dados p/ o escalonador */
  TTime TimeToRun;
  /* Dados p/ mudanca de contexto */
  TaskPointer InitPC;
  TContext Context;
} TTask;

typedef TTask TTaskList[MAXTASKS];

#endif
