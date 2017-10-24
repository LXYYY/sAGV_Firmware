#include <stdlib.h>
#include <math.h>

#include "MCTools.h"
#include "user_function.h"
#include "rAGVControl.h"
#include "app_cfg.h"
#include "bsp_pwm.h"
#include "Navigation.h"

Uint16 uQEP1 = 0;  //����1�ļ���ֵ
Uint16 uQEP1Old = 0;   //��һ������1�Ķ���ֵ
int16 RealV1 = 0;  //��1��ʵ���ٶ�
int32 RealS1 = 0;  //��1��ʵ��λ��

Uint16 uQEP2 = 0;  //����2�ļ���ֵ
Uint16 uQEP2Old = 0; //��һ������2�Ķ���ֵ
int16 RealV2 = 0; //��2��ʵ���ٶ�
int32 RealS2 = 0; //��2��ʵ��λ�� 

Uint16 AGV_Head_Dir = X_FORWARD;  //��ͷ����Ĭ����X������ͬ�� 

//����ϵͳ
float32 GS_DEV = 0 ; //���ղ������ĵ���ϵͳ������ƫ���mmΪ��λ,����Ϊ��
float32 GS_DevFB = 0; //ǰ��ƫ���mmΪ��λ����ǰΪ��
float32 GS_Angle = 0; //���ղ������ĵ���ϵͳ�ĽǶ�ƫ��Զ�Ϊ��λ ������Ϊ�� 

/*--------------��ȡDI״̬------------------------*/

//�ж����ŷ��Ƿ�ʧЧ   ����1��ʾ���ŷ�ʧЧ ����0���ʾ��Ч
Uint16  ServoLeftInvaild(void)
{
    return  ( 0x0  );
}

//�ж����ŷ��Ƿ�ʧЧ   ����1��ʾ���ŷ�ʧЧ ����0���ʾ��Ч
Uint16  ServoRightInvalid(void)
{
    return   ( 0x0 );
}


//�жϳ��Ӵ����Ƿ�պϣ�����1��ʾ�պ���,����0��ʾδ�պ�
Uint16 ChargeFeedback(void)
{
	return   (0x0 );
}

// �ж϶����Ƿ��ϵ磬����1��ʾ�ϵ磬����0��ʾ�ϵ�
Uint16 ServoPowerFeedback(void)
{
	return (0x1 );
}

// �ж��Ƿ񵽴�����λ ����1��ʾ��������λ������0��ʾ����������λ
Uint16 DnLimitPos(void)
{
	return (0x0 );
}

void VControl(float32 fLinearVel, float32 fDiffVel)
{
     fDiffVel = LIMIT(fDiffVel,35);

     if(abs(fDiffVel) > abs(fLinearVel))
       fDiffVel = (fDiffVel>0)?abs(fLinearVel):-(abs(fLinearVel));

	 //����������Ӧ���е��ٶ�
	 fCurVelL = fLinearVel + fDiffVel;  //����
	 fCurVelR = fLinearVel - fDiffVel;  //����

	 fCurVelL = LIMIT(fCurVelL,2500);
	 fCurVelR = LIMIT(fCurVelR,2500);
}

/*********************************************
    ��תʱ�ٶȿ���
    ���ݵ���������ƫ�� ���������ֵ������ٶ�
	wCurVel ��ǰ��ת�����ٶ� (�ԽǶȼǣ��ǻ��ȣ�������mm��)
	wRadio ������ת�뾶��Ĭ��Ϊ0(mm)
********************************************/
void WControl(float32 wCurVel, float32 wRadio)
{
     
	 //����������Ӧ���е��ٶ�(mm)
	 
	 //fCurVelL = RatioW2V((wCurVel + fDiffVel));  //����
	 //fCurVelR = RatioW2V((wCurVel - fDiffVel));  //����

	 wRadio=LIMIT(wRadio,90);

	 fCurVelL=-1*(wCurVel*PI/180)*(wRadio-VEHICLERADIUS);
	 fCurVelR=(wCurVel*PI/180)*(wRadio+VEHICLERADIUS);
	 
	 fCurVelL = LIMIT(fCurVelL,650);
	 fCurVelR = LIMIT(fCurVelR,650);
} 


/*********************************************
    ʹԭ��������ָ���Ĳ�����Ŀ������仯(������)
	���أ�0 ��ʾ��û��ΪĿ��ֵ
	      1 ��ֵ��Ϊ��Ŀ��ֵ
**********************************************/
Uint16 FApproach(float32* pFrom, float32 fTo,float32 fStep)
{
	if(* pFrom + fStep < fTo)
	{
	   * pFrom += fStep;
	   return 0x0;
	}
	else if(* pFrom - fStep > fTo)
	{
	   * pFrom -= fStep;
	   return 0x0;
	}
	else if(* pFrom != fTo)
	{
		* pFrom = fTo;
		return 0x0;
	}
	else
	{
	    return 0x1;
	}
} 

void UpdateAgvHeadDirToNew(void)
{
	if((twoD_Angle< 45.0001f) || ((twoD_Angle > 315.0001f)&&(twoD_Angle< 360.1f)))
	{
		AGV_Head_Dir = X_FORWARD;
	}
	else if((twoD_Angle > 45.0001f)&&(twoD_Angle< 135.0001f))
	{
		AGV_Head_Dir =  Y_BACKWARD ;
	}
	else if((twoD_Angle > 135.0001f)&&(twoD_Angle< 225.0001f))
	{
		AGV_Head_Dir = X_BACKWARD;
	}
	else if((twoD_Angle > 225.0001f)&&(twoD_Angle< 315.0001f))
	{
		AGV_Head_Dir = Y_FORWARD;
	}
}

/*------------------------��ȡ��ά��ƫ����Ϣ-----------------------------*/
void Get_TwoDDev(void)
{
	if( AGV_Head_Dir == X_FORWARD)
	{
		GS_DEV = twoD_YP;
		GS_DevFB = twoD_XP;
		GS_Angle = ( twoD_Angle > 180.0f) ? (360.0f - twoD_Angle) : ( - twoD_Angle);
	}
	else if ( AGV_Head_Dir == X_BACKWARD)
	{
		GS_DEV =  - twoD_YP;
		GS_DevFB = - twoD_XP;
		GS_Angle = 180.0f - twoD_Angle ;
	}
	else if ( AGV_Head_Dir == Y_FORWARD)
	{
		GS_DEV = - twoD_XP;
		GS_DevFB = twoD_YP;
		GS_Angle = 270.0f - twoD_Angle ;			
	}
	else if ( AGV_Head_Dir ==  Y_BACKWARD)
	{
		GS_DEV = twoD_XP;
		GS_DevFB = -twoD_YP;
		GS_Angle = 90.0f - twoD_Angle;	
	}
} 


//floatת��Ϊint16 ����������
int16  floatToInt16(float32 data)
{
    int16 reverData = 0;
	if(data>0)
	   reverData = (data * 10+5)/10;
	else if(data<0)
	   reverData = (data * 10-5)/10;
	else
	   reverData = 0;
	return reverData;
} 

/*---------------------��С��ת������----------------------*/
Uint32 swapUint32(Uint32 value)  
{
    Uint32 data = 0;
	data =  ((value & 0x000000FF) << 24);
	data |= ((value & 0x0000FF00) << 8);
	data |= ((value & 0x00FF0000) >> 8);
	data |= ((value & 0xFF000000) >> 24);
	return data;
} 
/*----------------end line--------------------*/
