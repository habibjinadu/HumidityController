/* 
 * File:   IR.h
 * Author: ismail
 *
 * Created on November 27, 2020, 12:44 PM
 */

#ifndef IR_H
#define	IR_H

#ifdef	__cplusplus
extern "C" {
#endif

#define MAX_IR_BIT_SIZE                         64
#define INT_MSG_SIZE                            uint64_t
#define FAN_POWER_ON_ECO_SPEED                  0x1120304350006072
#define FAN_POWER_OFF                           0x1020304350006073
#define FAN_POWER_ON_ECO_SPEED_NATURAL_WIND     0x1120314353006073
#define SWITCH_TO_EVENING_WIND                  0x1120314353046077    
    
void carrier(uint16_t clkCycles);
void oneBit();
void startBit();
void zeroBit();
void txIRCommand(bool *cmdArray);
void sendMessage(INT_MSG_SIZE command);


#ifdef	__cplusplus
}
#endif

#endif	/* IR_H */


