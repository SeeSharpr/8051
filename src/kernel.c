#include<limits.h>
#include<stdlib.h>

#include"types.h"

/* Ajusta o timer */
#define __SETTIMER(r,t)         Clock.word=r.word.h; TH##t=r.byte.lh; TL##t=r.byte.ll
#define _SETTIMER(r,t)          __SETTIMER(r,t)
#define SETTIMER(r)             _SETTIMER(r,TIMER)

/* Habilita interrupcao de timer e coloca como prioridade maxima */
#define __ENABLEINTERRUPT(t)    EA=1; ET##t=1; PT##t=1
#define _ENABLEINTERRUPT(t)     __ENABLEINTERRUPT(t)
#define ENABLEINTERRUPT         _ENABLEINTERRUPT(TIMER)

/* Configura e liga o timer */
#define __ENABLETIMER(t)        TMOD|=(t)?0x10:0x01; TR##t=1
#define _ENABLETIMER(t)         __ENABLETIMER(t)
#define ENABLETIMER             _ENABLETIMER(TIMER)

/* Desliga o timer */
#define __DISABLETIMER(t)       TR##t=0
#define _DISABLETIMER(t)        __DISABLETIMER(t)
#define DISABLETIMER            _DISABLETIMER(TIMER)

/* Desabilita interrupcoes */
#define __DISABLEINTERRUPT(t)   EA=0; ET##t=0; PT##t=0
#define _DISABLEINTERRUPT(t)    __DISABLEINTERRUPT(t)
#define DISABLEINTERRUPT        _DISABLEINTERRUPT(TIMER)

/* Le o timer atual */
#define __GETTIMER(r,t)         do { r.word.h=Clock.word; r.byte.lh=TH##t; r.byte.ll=TL#0; } while (r.byte.lh != TH##t)
#define _GETTIMER(r,t)          __GETTIMER(r,t)
#define GETTIMER(r);            _GETTIMER(r,TIMER)

/* Converte pulsos para timer */
#define INTTOTIMER(n)           (0xFFFFFFFFL - n)

/* Informacoes sobre tarefa corrente e proxima */
TTaskID data CurrentTask;
TTaskID data NextTask;

/* Contextos das tarefas corrente e proxima */
PContext data CurrentContext;
PContext data NextContext;
bit SaveContext;
bit ForcedInt;

/* Contexto da tarefa vazia (main) */
TContext xdata MainContext;

/* Lista de tarefas */
TTaskID data TotalTasks;
TTaskList xdata TaskList;

/* Dados para calculo de tempo */
Reg32 data StartProcess, EndProcess;
Reg32 data StartSched, EndSched;
TTime data ProcessTime, SchedTime;
TTime data GivenTime, UsedTime;

/* Complemento para o relogio de 32 bits */
Reg16 data Clock;

void InitScheduler( void )
{
  TTaskID i;
  for (i = 0; i < TotalTasks; i++)
  {
     TaskList[i].TimeToRun = TaskList[i].Period;
  }
}

void GetInitTimers( void )
{
  GETTIMER(EndProcess);
  StartSched.dword = 0;
  SETTIMER(StartSched);
  ENABLETIMER;
}

void GetEndTimers( void )
{
   StartProcess.dword = INTTOTIMER(GivenTime);
   GETTIMER(EndSched);
   StartProcess.dword += EndSched.dword;
   SETTIMER(StartProcess);
}

void Scheduler( void )
{
  /* Dados para a tarefa que vai rodar */
  TTime data next_per = MAXPERIOD;
  /* Dados para a tarefa que pode preemptar */
  TTime data lim_per = MAXPERIOD;
  TTime data lim_ttr = MAXPERIOD;
  /* Dados auxiliares para a busca */
  TTime data aux_per;
  TTime data aux_ttr;
  TTaskID data i;
  
  /* Calcula o tempo que o processo e o escalonamento anteriores gastaram */
  UsedTime = ( EndProcess.dword - StartProcess.dword ) + ( EndSched.dword - StartSched.dword );
  /* Verifica se o processo atual terminou (gastou menos tempo do que foi dado) */
  /* if ( ( CurrentTask != NOTASK ) && ( UsedTime < GivenTime ) ) */
  if ( ( CurrentTask != NOTASK ) && ( ForcedInt ) )
     {
        /* Se terminou, ele so' vai voltar a rodar no prox. periodo */
        TaskList[CurrentTask].TimeToRun += TaskList[CurrentTask].Period;
        /* Volta o PC para o inicio do processo */
        TaskList[CurrentTask].Context.PC = TaskList[CurrentTask].InitPC;
        /* Avisa para nao salvar o contexto */
        SaveContext = 0;
     }
  else
     {
        /* Se ele foi preemptado, avisa para salvar o contexto */
        SaveContext = 1;
     }
  /* Busca candidatos a rodar */
  NextTask = NOTASK;
  for (i = 0; i < TotalTasks; i++)
  {
     /* Retira de todo mundo o tempo que a ultima tarefa gastou */
     TaskList[i].TimeToRun -= UsedTime;
     /* Tarefa que vai ser testada */
     aux_per = TaskList[i].Period;
     aux_ttr = TaskList[i].TimeToRun;
     /* Encontra uma tarefa que pode rodar */
     if ( aux_ttr <= aux_per )
        {
           /* Encontra a tarefa de maior prioridade */
           if ( aux_per < next_per )
              {
                 NextTask = i;
                 next_per = aux_per;
              }
        }
     /* Se ela nao pode rodar agora, verifica se ela vai rodar no futuro */
     else
        {
           /* Novamente busca a de maior prioridade */
           if ( aux_per < lim_per )
              {
                 lim_per = aux_per;
                 lim_ttr = aux_ttr;
              }
        }
  }
  /* Calcula o tempo total para a proxima execucao */
  /* Se for a tarefa de maior prioridade, da' todo o tempo possivel */
  GivenTime = (lim_per != MAXPERIOD)? (lim_ttr - lim_per) : MAXPERIOD;
  /* Aponta para as tabelas de contexto corretas */
  NextContext = ( NextTask == NOTASK )? &(MainContext) : &(TaskList[NextTask].Context);
  CurrentContext = ( CurrentTask == NOTASK )? &(MainContext) : &(TaskList[CurrentTask].Context);
  /* Ajusta a tarefa escolhida para a tarefa atual */
  CurrentTask = NextTask;
  /* Limpa flag de interrupcao forcada */
  ForcedInt = 0;
}

void InitKernel( void )
{
  CurrentTask = NOTASK;
  CurrentContext = NULL;
  TotalTasks = 0;
  GivenTime = 0;
  UsedTime = 0;
  ForcedInt = 0;
  Clock.word = 0;
  StartProcess.dword = 0;
  EndProcess.dword = 0;
  StartSched.dword = 0;
  EndSched.dword = 0;
}

void AddTask( TTime P,TaskPointer PTR )
{
  if ( TotalTasks < MAXTASKS )
  {
     TaskList[TotalTasks].Period = P;
     TaskList[TotalTasks].Context.PC = PTR;
     TaskList[TotalTasks].InitPC = PTR;
     TotalTasks++;
  }
}

void RunKernel( void )
{
  InitScheduler();
  ENABLEINTERRUPT;
  EndTask();
}

