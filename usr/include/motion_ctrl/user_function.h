/*
  user_function.h							  
  ����������������Χģ����ص���������	  

*/


#ifndef __USER_FUNCTION_H_
#define __USER_FUNCTION_H_

#include "porting.h"
#include "mc_config.h"

/*--------------------Tools.c var define-----------------------*/
typedef union
{
   float32 f;
   Uint32  u;
}CFloatData;

typedef union
{
   int32  s;
   Uint32 u;
}CInt32Data;

typedef union
{
   int16  s;
   Uint16 u;
}CInt16Data;

extern Uint16 uQEP1;  //����1�ļ���ֵ
extern Uint16 uQEP1Old ;   //��һ������1�Ķ���ֵ
extern int16 RealV1;  //��1��ʵ���ٶ�
extern int32 RealS1;  //��1��ʵ��λ��

extern Uint16 uQEP2;  //����2�ļ���ֵ
extern Uint16 uQEP2Old; //��һ������2�Ķ���ֵ
extern int16 RealV2; //��2��ʵ���ٶ�
extern int32 RealS2; //��2��ʵ��λ�� 


extern int32 deltaVL;  
extern int32 deltaVR;  


extern Uint16 AGV_Head_Dir;


//����ϵͳ
extern float32 GS_DEV; //��mmΪ��λ
extern float32 GS_DevFB;
extern float32 GS_Angle; //�Զ�Ϊ��λ  

#define  X_FORWARD    0x00
#define  Y_FORWARD    0x01
#define  X_BACKWARD   0x02
#define  Y_BACKWARD   0x03 


#define   LIMIT(x,lmt)      ((x > lmt)? lmt : ((x > -lmt)? x : (-lmt)))  //����x�� -lmt ~ lmt
#define   minusL(src,dest)  ((dest>=src) ? (dest-src) : (dest+4-src))
#define   minusR(src,dest)  ((dest>src) ? (4+src-dest) : (src-dest))
#define   absVar(x)			((x>0)?(x):(-x))

#define   radianToAngle(x)	(x * 180/PI)
#define		angleToRadian(x)	(x * PI/180)

#define    RatioW2V(x)    (x* 2* PI* VEHICLERADIUS/360)
#define    int16ToF(x)    (x * 1.0f) 


extern void  CalculateRealPosition(void);
extern void VControl(float32 fLinearVel, float32 fDiffVel);
extern void WControl(float32 wCurVel, float32 wRadio);
extern Uint16 FApproach(float32* pFrom, float32 fTo,float32 fStep);
extern void UpdateAgvHeadDirToNew(void);

extern Uint16 ServoLeftInvaild(void);
extern Uint16 ServoRightInvalid(void);
extern Uint16 ChargeFeedback(void);


extern void Get_TwoDDev(void);
extern int16  floatToInt16(float32 data);
extern Uint32 swapUint32(Uint32 value) ;

/*-------------------CanProcess.c var define------------------------------------*/
extern Uint16  iPath;		// ʣ��·����
extern float32 vPath;
extern Uint16  ethCommBreakFlag; 
extern Uint16  Motionstyle;

extern float32 fCurVel;  //��ǰ���ٶ�
extern float32 fCurVelL;  //��ǰ�������ٶ�
extern float32 fCurVelR;  //��ǰ�������ٶ�
extern float32 fDiffVel;
extern float32 sRemain; 

extern float32  angleRemain;  //�����·�����Ҫ��ת�ĽǶ�
extern float32  angleProcess; //ת90֮�侭���ĽǶ�ֵ
extern Uint16  AGV_Dir_Dest;  //�·���תָ����Ŀ�귽��
extern float32  wCurrent;
extern float32  wCycle;
extern  Uint16 turnWholeCycleCnt;

extern float32 twoDAngleConver;  //����ά�봫�����ĽǶ�ֵת��Ϊ��ʱ���0~360��
extern float32 twoDAngleOld;     //��¼��һʱ�̵Ķ�ά��Ƕ�ֵ
extern float32 twoDAngleNew ;	 //��¼��ǰʱ�̵Ķ�ά��Ƕ�ֵ��Ϊ�˽��п�X��ĽǶȱ任
extern Uint16  acrossXAxisFlag ;     //��x���˱�־λΪ1������������ֵ����������ߵݼ�
extern float32 twoDAngleDest;

extern Uint16   encoderError;

extern float32  S_Vdown;

#define  ENCODER_ERROR  0x00455252  //0ERR  �������������  

extern void CanProc(void);
extern void MotionReport(void);
extern void TwoDCodeControl(Uint16 bOnOff);

/*-------------rAGVControl.c var define--------------------*/
extern Uint16 reverseMode;
extern Uint16 reverseCnt;
extern Uint16 nReverseFlag;


//�ٶ����
extern Uint16  uServoDelayClaspCount;// ��բ������ 

extern Uint16 	nAgvWorkMode;
extern float32 RealTwoDProgress;


extern void AGV_Run(void);

/*------------------isr.c var define-------------------------------*/
extern Uint16 twoDMissingCnt;



/*----------------------VPlan.c var define---------------------------------------*/

extern void VPlan(float32 sTotal,float32 accUp,float32 accDown,float32 vCurrent,float32 VPath);
extern void CyclePlan(float32 Angle,float32 wAccUp, float32 wAccDn,float32 wCurrent, float32 wAngle);

#endif

/*****************************End line*************************************/
