#include <math.h>

#include "user_function.h"
#include "DataBank.h"
#include "VPlan.h"

//#include "DataBank.h"



/**********************************************  
   �����ٶȹ滮�㷨
   sTotal   Ϊ��Ҫ����ľ���
   accUp    Ϊ���ٶ�
   accDown  Ϊ���ٶ�
   vPath    Ϊ�����·���·���������
   vCurrent Ϊ����������ȡ�ĵ�ǰ�ٶ�
**********************************************/
void VPlan(float32 sTotal,float32 accUp,float32 accDown,float32 vCurrent,float32 VPath)
{
	float32 s10,s11,s3;
	
	s10 = ((VPath-vCurrent)* (VPath-vCurrent))/ (2 * accUp)  ;
  s11 =  vCurrent * (VPath-vCurrent)/ accUp ;  
	s3  =  VPath * VPath /(2 * accDown); 

	if(sTotal>0.0f)
	{
		if(s10 + s11+s3 < sTotal)  //��ʱ�����ٶΣ��ٶ����ֵΪ�����·���
		{

			fTargetVel = VPath;
		}
		else  //��ʱ�����ٶΣ���������ٵ�����ٶ�
		{
			fTargetVel = sqrt((( 2 * sTotal * accUp * accDown) + (accDown * vCurrent * vCurrent ))/ (accUp + accDown));
		}
	}
}

void CyclePlan(float32 Angle,float32 wAccUp, float32 wAccDn,float32 wCurrent, float32 wAngle)
{
	float32 angle_10,angle_11,angle_3;


	angle_10 = ((wAngle - wCurrent) * (wAngle - wCurrent)) / (2 * wAccUp);
	angle_11 = wCurrent * (wAngle - wCurrent) / wAccUp;
	angle_3  = wAngle * wAngle /  ( 2 * wAccDn );

	if( angle_10 + angle_11 + angle_3 < Angle)
	{

		wTarget = wAngle;
	}
	else
	{
		wTarget = sqrt((( 2 * Angle * wAccUp * wAccDn) + (wAccDn * wCurrent * wCurrent ))/ (wAccUp + wAccDn) );
	}

}
/*--------------------------end line-------------------*/


