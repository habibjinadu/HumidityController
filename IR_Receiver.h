 
#ifndef IR_RECEIVER_H
#define	IR_RECEIVER_H

#include <xc.h> // include processor files - each processor file is guarded.  

#define MAX_RECEIVE_BIT_SIZE            64
#define INT_M_SIZE                      uint64_t
#define ZERO_BIT_HI_CYCLES              2240    // 2240 cycles or (560us)     
#define ZERO_BIT_LO_CYCLES              2240    // 2240 cycles or (560us)
#define ONE_BIT_HI_CYCLES               7760    // 7760 cycles or (1690us)
#define ONE_BIT_LO_CYCLES               2240    // 2240 cycles or (560us)
#define START_BIT_LO_CYCLES             36000   // 36000 cycles or (9ms)        
#define START_BIT_HI_CYCLES             18000   // 18000 cycles or (4.5ms)
#define OFFSET_ERR                      1000    // cycle error offset  1000 cycles or (250us)
#define START_OFFSET_ERR                9000    // start bit offset error 9000 cycles or (2.25ms)
#define TIMEOUT_CYCLES                  60000   // 60000 cycles (15ms) of no activity


#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

INT_M_SIZE decode_IR(uint8_t* bitsReceived);
void display_message();
void measure_pulse_width();
void update_message();
INT_M_SIZE determine_bit (uint16_t hi_cycles, uint16_t lo_cycles);

extern int CN_interrupt_flag; // global variable

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* IR_RECEIVER_H */

