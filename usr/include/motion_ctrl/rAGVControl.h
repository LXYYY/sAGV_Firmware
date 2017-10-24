#ifndef _RAGVCONTROL_H
#define _RAGVCONTROL_H
#include "porting.h"
#include "motors.h"

#define REVERSE_MODE_HORIZONTAL DUMPER_STATE_HORIZONTAL
#define REVERSE_MODE_REVERSEUP  DUMPER_STATE_MOVINGUP
#define REVERSE_MODE_TOP        DUMPER_STATE_TOP
#define REVERSE_MODE_REVERSEDN  DUMPER_STATE_MOVINGDOWN
#define  TIME_SERVO_DELAY_CLASP			 ( 10) 

//AGV״̬����
#define AGV_MODE_STANDBY     0  //����ģʽ
#define AGV_MODE_RUNNING     1  //����ģʽ
#define AGV_MODE_SUSPENDED   2  //����ģʽ 
#define AGV_MODE_OP          3  //����ģʽ

extern Uint16 reverseMode;
extern Uint16 reverseCnt ;
extern Uint16 nReverseFlag ;
extern Uint16 uServoDelayClaspCount;

void AGV_Standby(void);
void AGV_Running(void);
void AGV_Operation(void);
void AGV_SuspendRun(void); 

void  reverseTask(void);
#endif


