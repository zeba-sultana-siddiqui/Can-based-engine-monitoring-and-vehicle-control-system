;/*
; * Standard ARM7TDMI Startup File for NXP LPC2129 Microcontroller
; * Keil ARM Toolchain Compatible
; */

; Standard definitions of Mode bits and Interrupt flags
Mode_USR        EQU     0x10
Mode_FIQ        EQU     0x11
Mode_IRQ        EQU     0x12
Mode_SVC        EQU     0x13
Mode_ABT        EQU     0x17
Mode_UND        EQU     0x1B
Mode_SYS        EQU     0x1F

I_Bit           EQU     0x80    ; when I bit is set, IRQ is disabled
F_Bit           EQU     0x40    ; when F bit is set, FIQ is disabled

; Stack Sizes
UND_Stack_Size  EQU     0x00000004
SVC_Stack_Size  EQU     0x00000004
ABT_Stack_Size  EQU     0x00000004
FIQ_Stack_Size  EQU     0x00000004
IRQ_Stack_Size  EQU     0x00000080
USR_Stack_Size  EQU     0x00000400

Stack_Size      EQU     (UND_Stack_Size + SVC_Stack_Size + ABT_Stack_Size + \
                         FIQ_Stack_Size + IRQ_Stack_Size + USR_Stack_Size)

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp

                AREA    RESET, CODE, READONLY
                ARM

; Vector Table
Vectors         LDR     PC, Reset_Addr         
                LDR     PC, Undef_Addr
                LDR     PC, SWI_Addr
                LDR     PC, PAbt_Addr
                LDR     PC, DAbt_Addr
                NOP                            ; Reserved Vector
                LDR     PC, IRQ_Addr
                LDR     PC, FIQ_Addr

Reset_Addr      DCD     Reset_Handler
Undef_Addr      DCD     Undef_Handler
SWI_Addr        DCD     SWI_Handler
PAbt_Addr       DCD     PAbt_Handler
DAbt_Addr       DCD     DAbt_Handler
                DCD     0                      ; Reserved
IRQ_Addr        DCD     IRQ_Handler
FIQ_Addr        DCD     FIQ_Handler

Undef_Handler   B       Undef_Handler
SWI_Handler     B       SWI_Handler
PAbt_Handler    B       PAbt_Handler
DAbt_Handler    B       DAbt_Handler
IRQ_Handler     B       IRQ_Handler
FIQ_Handler     B       FIQ_Handler

; Reset Handler Entry Point
Reset_Handler   

                ; Setup Stack Pointer for each CPU mode
                LDR     R0, =Stack_Mem

                ; Enter IRQ Mode and set its Stack Pointer
                MSR     CPSR_c, #Mode_IRQ:OR:I_Bit:OR:F_Bit
                MOV     SP, R0
                SUB     R0, R0, #IRQ_Stack_Size

                ; Enter Supervisor / User Mode
                MSR     CPSR_c, #Mode_SYS
                MOV     SP, R0

                ; Import and jump to C runtime entry
                IMPORT  __main
                LDR     R0, =__main
                BX      R0

                END
