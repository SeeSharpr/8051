#ifndef _kernel_h
#define _kernel_h

#include<reg51.h>

/* Variaveis exportadas */

extern bit ForcedInt;

/* Tipos */

typedef unsigned char TTaskID;
typedef unsigned long TTime;

typedef void (*TaskPointer)( void );

/* Constantes */

#define MAXTASKS    10
#define TIMER       0
#define CLK         921.583

/* Macros */

/* Forca a interrupcao pelo timer */
#define __CALLINTERRUPT(t)      ForcedInt=1; TF##t=1
#define _CALLINTERRUPT(t)       __CALLINTERRUPT(t)
#define CALLINTERRUPT           _CALLINTERRUPT(TIMER)

/* Funcoes */

/* Finaliza uma tarefa */
#define EndTask() CALLINTERRUPT; for(;;)
/* Converte de milissegundos para pulsos de clock */
#define mstoclk(ms) ((TTime)(ms*CLK))
/* Inicializa o kernel */
void InitKernel( void );
/* Adiciona uma tarefa */
void AddTask( TTime P,TaskPointer PTR );
/* Roda o sistema */
void RunKernel( void );

#endif
