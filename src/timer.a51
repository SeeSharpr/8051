$NOMOD51
$INCLUDE(REG51.INC)

; elementos importados
EXTRN CODE	(Scheduler,GetInitTimers,GetEndTimers)
EXTRN DATA	(CurrentContext,NextContext,Clock)
EXTRN BIT  (SaveContext,ForcedInt)

; elementos exportados

; definicao de segmentos relocaveis

s_data  SEGMENT DATA
s_code  SEGMENT CODE

; definicao de segmentos absolutos

; vetor de interrupcoes

CSEG AT 000Bh          ; endereco de Timer0
  LJMP t0_int	         ; salta para a rotina de tratamento
	
; definicao das variaveis

RSEG s_data

t_acc:  DS 1
t_b:    DS 1
t_psw:  DS 1
t_dph:  DS 1
t_dpl:  DS 1
t_pch:  DS 1
t_pcl:  DS 1
t_r0:   DS 1
t_r1:   DS 1
t_r2:   DS 1
t_r3:   DS 1
t_r4:   DS 1
t_r5:   DS 1
t_r6:   DS 1
t_r7:   DS 1

; implementacao
	
RSEG s_code

t0_int:
  JB    ForcedInt,begin_int
  PUSH  PSW
  PUSH  ACC
  INC   (Clock+1)      ; LSB de Clock
  MOV   ACC,(Clock+1)
  JZ    estouro_l
  POP   ACC
  POP   PSW
  RETI

estouro_l:
  INC   (Clock+0)      ; MSB de Clock
  MOV   ACC,(Clock+0)
  JZ    estouro_h
  POP   ACC
  POP   PSW
  RETI

estouro_h:
  POP   ACC
  POP   PSW

begin_int:
  MOV   t_psw,PSW      ; salva o contexto inteiro da maquina
  MOV   t_acc,A
  MOV   t_b,B
  MOV   t_dpl,DPL
  MOV   t_dph,DPH
  MOV   t_r0,R0
  MOV   t_r1,R1
  MOV   t_r2,R2
  MOV   t_r3,R3
  MOV   t_r4,R4
  MOV   t_r5,R5
  MOV   t_r6,R6
  MOV   t_r7,R7
  POP   ACC            ; retira endereco de retorno da pilha
  MOV   t_pch,ACC
  POP   ACC
  MOV   t_pcl,ACC
  LCALL GetInitTimers  ; guarda o timer atual
  LCALL Scheduler      ; chama o escalonador

testa_salva:
  JNB   SaveContext,restaura_contexto
  
salva_contexto:
  MOV   DPH,(CurrentContext+1)  ; DPTR aponta para CurrentContext
  MOV   DPL,(CurrentContext+2)
  MOV   ACC,t_pch               ; escreve um byte
  MOVX  @DPTR,A                 
  INC   DPTR                    ; salta uma posicao de memoria
  MOV   ACC,t_pcl
  MOVX  @DPTR,A
  INC   DPTR
  MOV   ACC,t_dph
  MOVX  @DPTR,A
  INC   DPTR
  MOV   ACC,t_dpl
  MOVX  @DPTR,A
  INC   DPTR
  MOV   ACC,t_acc
  MOVX  @DPTR,A
  INC   DPTR
  MOV   ACC,t_b
  MOVX  @DPTR,A
  INC   DPTR
  MOV   ACC,t_r0
  MOVX  @DPTR,A
  INC   DPTR
  MOV   ACC,t_r1
  MOVX  @DPTR,A
  INC   DPTR
  MOV   ACC,t_r2
  MOVX  @DPTR,A
  INC   DPTR
  MOV   ACC,t_r3
  MOVX  @DPTR,A
  INC   DPTR
  MOV   ACC,t_r4
  MOVX  @DPTR,A
  INC   DPTR
  MOV   ACC,t_r5
  MOVX  @DPTR,A
  INC   DPTR
  MOV   ACC,t_r6
  MOVX  @DPTR,A
  INC   DPTR
  MOV   ACC,t_r7
  MOVX  @DPTR,A
  INC   DPTR
  MOV   ACC,t_psw
  MOVX  @DPTR,A

restaura_contexto:
  MOV   DPH,(NextContext+1)  ; DPTR aponta para NextContext
  MOV   DPL,(NextContext+2)
  MOVX  A,@DPTR              ; le um byte
  MOV   t_pch,ACC
  INC   DPTR                 ; salta uma posicao de memoria
  MOVX  A,@DPTR
  MOV   t_pcl,ACC
  INC   DPTR
  MOVX  A,@DPTR
  MOV   t_dph,ACC
  INC   DPTR
  MOVX  A,@DPTR
  MOV   t_dpl,ACC
  INC   DPTR
  MOVX  A,@DPTR
  MOV   t_acc,ACC
  INC   DPTR
  MOVX  A,@DPTR
  MOV   t_b,ACC
  INC   DPTR
  MOVX  A,@DPTR
  MOV   t_r0,ACC
  INC   DPTR
  MOVX  A,@DPTR
  MOV   t_r1,ACC
  INC   DPTR
  MOVX  A,@DPTR
  MOV   t_r2,ACC
  INC   DPTR
  MOVX  A,@DPTR
  MOV   t_r3,ACC
  INC   DPTR
  MOVX  A,@DPTR
  MOV   t_r4,ACC
  INC   DPTR
  MOVX  A,@DPTR
  MOV   t_r5,ACC
  INC   DPTR
  MOVX  A,@DPTR
  MOV   t_r6,ACC
  INC   DPTR
  MOVX  A,@DPTR
  MOV   t_r7,ACC
  INC   DPTR
  MOVX  A,@DPTR
  MOV   t_psw,ACC

continua:
  LCALL GetEndTimers   ; le e ajusta os timers para a proxima tarefa
  MOV   PSW,t_psw
  MOV   B,t_b
  MOV   DPL,t_dpl
  MOV   DPH,t_dph
  MOV   R0,t_r0
  MOV   R1,t_r1
  MOV   R2,t_r2
  MOV   R3,t_r3
  MOV   R4,t_r4
  MOV   R5,t_r5
  MOV   R6,t_r6
  MOV   R7,t_r7
  MOV   ACC,t_pcl	   ; coloca endereco de retorno da pilha
  PUSH  ACC
  MOV   ACC,t_pch
  PUSH  ACC
  MOV   PSW,t_psw
  RETI

END

