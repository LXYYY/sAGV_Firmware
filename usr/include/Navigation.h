#ifndef __NAVIGATION_H_20170401111742__
#define __NAVIGATION_H_20170401111742__

#include "porting.h"

extern float32  twoD_YP ;      //Y���ϵ�ƫ�ƣ���λΪmm
extern float32  twoD_XP;      //X���ϵ�ƫ�ƣ���λΪmm
extern float32  twoD_Angle ;   //�Ƕ�ƫ�ƣ�Ϊ��ά��X���������ά��������˳ʱ��нǣ��ܷ�ΧΪ0~359
extern Uint16 NewTag;
extern Uint32 twoD_TagCode ; //��ǩ����
extern Uint32 twoD_TagCodeOld ; //��һ�εı�ǩ����


int NavigationInit(void);

int NavigationOnOff(int on);

#endif
