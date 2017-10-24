#ifndef _ROTATING_H
#define _ROTATING_H

#define M_PINEW 3.1415926	// Բ����
#define NUM_ERROR (1e-6)   //��ֵ�����

#include "porting.h"

// #4 ��
#define CCD_X_ERROR (0.005) //CCD����ͷ�ڳ�ͷ����İ�װ���
#define CCD_Y_ERROR (0.0000) //CCD����ͷ��Y����İ�װ���


extern float32 rotateRadius;

extern void RotateInit(float pX0, float pY0, float pTheta0, float pThetaEnd);
extern float32 CalcRotateP(float Px, float Py, float Ptheta);

extern float PxFreeze; 
extern float PyFreeze; 
extern float PthetaFreeze;

#endif
/***********************END Line************************/
