#include <stdint.h>

#include "bsp_io.h"

/*8·���ARM_DO0.1~ARM_DO0.12��ARM�ܽŵĶ�Ӧ��ϵ*/

const uint16_t arm_dout[9]={0,PIN_DO01,PIN_DO02,PIN_DO03,PIN_DO04,PIN_DO05,PIN_DO06, 
								               PIN_DO07,PIN_DO08};
GPIO_TypeDef* arm_dout_port[9]={0,GPIO_DO01,GPIO_DO02,GPIO_DO03,GPIO_DO04,GPIO_DO05,GPIO_DO06,
									                 GPIO_DO07,GPIO_DO08};

/*8·����ARM_DI0.1~ARM_DI0.14��ARM�ܽŵĶ�Ӧ��ϵ*/
const uint16_t arm_din[9] = {0,PIN_DI01,PIN_DI02,PIN_DI03,PIN_DI04,PIN_DI05,PIN_DI06,PIN_DI07,
								                PIN_DI08};
GPIO_TypeDef* arm_din_port[9]={0,GPIO_DI01,GPIO_DI02,GPIO_DI03,GPIO_DI04,GPIO_DI05,GPIO_DI06,GPIO_DI07, 
								                  GPIO_DI08};                              

void Arm_Poutx_Write(u8 xdo,u8 hl);


/*******************************************************************************
* Description    : ARM���ʹ��
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Arm_Poutx_Write(u8 xdo,u8 hl)
{
    if(hl)   
    {
	  	arm_dout_port[xdo]->BSRRL = arm_dout[xdo];    //��1,����ߵ�ƽ
    }
    else
    {
	  	arm_dout_port[xdo]->BSRRH = arm_dout[xdo];    //��0,����͵�ƽ
    }
}


int bsp_InitIO(void)
{
	u8 i;
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Configure GPIO in output pushpull mode */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	
	for (i = 1; i <= 8; i++)
	{
		GPIO_InitStructure.GPIO_Pin = arm_dout[i];
		GPIO_Init(arm_dout_port[i], &GPIO_InitStructure);
	}
	//�ϵ�Ĭ�����
	for (i = 1; i <= 8; i++)
	{
		Arm_Poutx_Write(i,0); //ARM_DO0.x����͵�ƽ,UL2004��Ϊ�ߵ�ƽ
	}

	/* Configure GPIO in Input pushpull Mode */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	
	for (i = 1; i <= 8; i++)
	{
		GPIO_InitStructure.GPIO_Pin = arm_din[i];
		GPIO_Init(arm_din_port[i], &GPIO_InitStructure);
	}
	return 0;
}

int bsp_DoSet(u8 ucdonum,u8 uclevel)
{
	if(ucdonum > 8 || ucdonum <= 0)
		return -1;
	if((uclevel != 0) && (uclevel != 1))
		return -1;
	Arm_Poutx_Write(ucdonum, uclevel);
	return 0;
}

int bsp_DiGet(u8 ucdinum)
{
	uint8_t temp;
	if(ucdinum > 8 || ucdinum <= 0)
		return -1;
	temp = GPIO_ReadInputDataBit(arm_din_port[ucdinum], arm_din[ucdinum]);
	return temp;
}



