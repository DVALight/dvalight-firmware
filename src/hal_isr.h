#ifndef __HAL_ISR_H
#define __HAL_ISR_H
#include <stdio.h>

#define ISR_HANDLER(name) void name ## _Handler(void)

#define ISR_DUMMY_HANDLER_PRINTF_NORET(name)  \
  ISR_HANDLER(name) \
  { \
    printf(#name "\r\n"); \
    while(1) {} \
  }

#define ISR_DUMMY_HANDLER_PRINTF(name)  \
  ISR_HANDLER(name) \
  { \
    printf(#name "\r\n"); \
  }

ISR_HANDLER(NMI);
ISR_HANDLER(HardFault);
ISR_HANDLER(MemManage);
ISR_HANDLER(BusFault);
ISR_HANDLER(UsageFault);
ISR_HANDLER(SVC);
ISR_HANDLER(DebugMon);
ISR_HANDLER(PendSV);
ISR_HANDLER(SysTick);

void EXTI1_IRQHandler(void);

#endif
