#ifndef __EINT_H__
#define __EINT_H__

#include <lpc21xx.h>
#include "types.h"

/* Original Switch Pin Assignments */
#define PIN_SW_WIN_UP       (1 << 16) /* P0.16 (EINT0) - Window UP */
#define PIN_SW_WIN_DOWN     (1 << 14) /* P0.14 (EINT1) - Window DOWN */
#define PIN_SW_REVERSE      (1 << 15) /* P0.15 (EINT2) - Reverse Mode */

/* Function Prototypes */
void Switches_Init(void);

extern volatile u8 flag_win_up;
extern volatile u8 flag_win_down;
extern volatile u8 flag_reverse;

#endif /* __EINT_H__ */
