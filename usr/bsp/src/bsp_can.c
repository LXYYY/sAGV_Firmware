#include <ucos_ii.h>
#include "app_cfg.h"

#include "bsp_can.h"

#define	CAN1_RX_GPIO		GPIOD
#define	CAN1_RX_PIN			GPIO_Pin_0
#define	CAN1_RX_PINSOURCE	GPIO_PinSource0

#define	CAN1_TX_GPIO		GPIOD
#define	CAN1_TX_PIN			GPIO_Pin_1
#define	CAN1_TX_PINSOURCE	GPIO_PinSource1


#define	CAN1_RCC			RCC_APB1Periph_CAN1
#define	CAN1_PORT_RCC		RCC_AHB1Periph_GPIOD


#define	CAN2_RX_GPIO		GPIOB
#define	CAN2_RX_PIN			GPIO_Pin_12
#define	CAN2_RX_PINSOURCE	GPIO_PinSource12

#define	CAN2_TX_GPIO		GPIOB
#define	CAN2_TX_PIN			GPIO_Pin_13
#define	CAN2_TX_PINSOURCE	GPIO_PinSource13


#define	CAN2_RCC			RCC_APB1Periph_CAN2
#define	CAN2_PORT_RCC		RCC_AHB1Periph_GPIOB

/* ����ȫ�ֱ��� */
#define	CAN_BUFF_SIZE	128   //must be power of 2

typedef struct
{
	u8 ucRxdataWrite;	//���뻺����д��ַ
	u8 ucRxdataRead;	//���뻺��������ַ
	u8 ucCanRxDatacount;//���뻺������Ч���ݸ���
	CanRxMsg sCanReceiveBuf[CAN_BUFF_SIZE];
}structCanBuf;

typedef struct
{
	s8 ucCanSendState;	
	u8 ucCanSendMbox;
	CanTxMsg	structCanTxMsg;
}StructCanxSendState;

structCanBuf g_structCanRe;
StructCanxSendState gs_CanSendState;

static void Can1_RX_ISR(void);
static void Can2_RX_ISR(void);




/************************************************************************
PB12 : CAN2_RX;
PB13 : CAN2_TX;
_uscbaudrata: khz

memset(Uart1RxBuf,0,sizeof(Uart1RxBuf));


************************************************************************/
int bsp_InitCan(CAN_TypeDef* _CANx, u16 _uscbaudrata)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	CAN_InitTypeDef        CAN_InitStructure;
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;	
	NVIC_InitTypeDef  NVIC_InitStructure;

	GPIO_StructInit(&GPIO_InitStructure);
	CAN_StructInit(&CAN_InitStructure);
	
	memset(&g_structCanRe,0,sizeof(g_structCanRe));

	if(0 == _uscbaudrata)
	{
		return -1;
	}
	
	if(CAN2 == _CANx)
	{
		/* CAN GPIOs ����*/
		/* ʹ��GPIOʱ�� */
		/* ʹ��CANʱ��, ʹ��CAN2�����CAN1��ʱ�� */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
		RCC_AHB1PeriphClockCmd(CAN2_PORT_RCC, ENABLE);
		RCC_APB1PeriphClockCmd(CAN2_RCC, ENABLE);
		
		/* ����ӳ��ΪCAN����  */
		GPIO_PinAFConfig(CAN2_RX_GPIO, CAN2_RX_PINSOURCE, GPIO_AF_CAN2);
		GPIO_PinAFConfig(CAN2_TX_GPIO, CAN2_TX_PINSOURCE, GPIO_AF_CAN2); 

		/* ���� CAN RX �� TX ���� */
		GPIO_InitStructure.GPIO_Pin = CAN2_RX_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
		GPIO_Init(CAN2_RX_GPIO, &GPIO_InitStructure);

		GPIO_InitStructure.GPIO_Pin = CAN2_TX_PIN;
		GPIO_Init(CAN2_TX_GPIO, &GPIO_InitStructure);

		
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
		
		/* Configures the NVIC for CAN */
		NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX0_IRQn;   //CAN1 FIFO0 Interrupt(����3���ж�:CAN_IT_FMP0\CAN_IT_FF0\CAN_IT_FOV0) 
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;        
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
	}
	else if(CAN1 == _CANx)
	{
		/* CAN GPIOs ����*/
		/* ʹ��GPIOʱ�� */
		/* ʹ��CANʱ��, ʹ��CAN2�����CAN1��ʱ�� */
		RCC_AHB1PeriphClockCmd(CAN1_PORT_RCC, ENABLE);
		RCC_APB1PeriphClockCmd(CAN1_RCC, ENABLE);
		
		/* ����ӳ��ΪCAN����  */
		GPIO_PinAFConfig(CAN1_RX_GPIO, CAN1_RX_PINSOURCE, GPIO_AF_CAN1);
		GPIO_PinAFConfig(CAN1_TX_GPIO, CAN1_TX_PINSOURCE, GPIO_AF_CAN1); 

		/* ���� CAN RX �� TX ���� */
		GPIO_InitStructure.GPIO_Pin = CAN1_RX_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
		GPIO_Init(CAN1_RX_GPIO, &GPIO_InitStructure);

		GPIO_InitStructure.GPIO_Pin = CAN1_TX_PIN;
		GPIO_Init(CAN1_TX_GPIO, &GPIO_InitStructure);

		
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
		
		/* Configures the NVIC for CAN */
		NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;   //CAN1 FIFO0 Interrupt(����3���ж�:CAN_IT_FMP0\CAN_IT_FF0\CAN_IT_FOV0) 
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
	}
	else
	{
		return -1;
	}
	/* ��λCAN�Ĵ��� */
	CAN_DeInit(_CANx);
	CAN_InitStructure.CAN_TTCM = DISABLE;			/* ��ֹʱ�䴥��ģʽ��������ʱ���), T  */
	CAN_InitStructure.CAN_ABOM = DISABLE;			/* ��ֹ�Զ����߹رչ��� */
	CAN_InitStructure.CAN_AWUM = DISABLE;			/* ��ֹ�Զ�����ģʽ */
	CAN_InitStructure.CAN_NART = DISABLE;			/* ��ֹ�ٲö�ʧ��������Զ��ش����� */
	CAN_InitStructure.CAN_RFLM = DISABLE;			/* ��ֹ����FIFO����ģʽ */
	CAN_InitStructure.CAN_TXFP = DISABLE;			/* ��ֹ����FIFO���ȼ� */
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;	/* ����CANΪ��������ģʽ */

	/* 
		CAN ������ = RCC_APB1Periph_CAN1 /{(SJW+1)*(Prescaler+1)*[1 + (BS1+1) + (BS2+1)]};
		
		SJW = synchronisation_jump_width 
		BS = bit_segment
		
		�����У�����CAN������Ϊ1 Mbps		
		CAN ������ = 42M /(1 + 14 + 6) / = 2000 Kbps	 
	*/
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
	CAN_InitStructure.CAN_BS1 = CAN_BS1_14tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_6tq;
	CAN_InitStructure.CAN_Prescaler = 2000/_uscbaudrata;
	CAN_Init(_CANx, &CAN_InitStructure);
	
	/* ����CAN�˲���14  CAN2�˲������ 14--27����CAN1����0--13*/
	CAN_FilterInitStructure.CAN_FilterNumber = 14;		
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;		/* �˲���ģʽ������ID����ģʽ */
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;	/* 32λ�˲� */
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;					/* �����ID�ĸ�16bit */
	CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;					/* �����ID�ĵ�16bit */
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;				/* ID����ֵ��16bit */
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;				/* ID����ֵ��16bit */
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO0;		/* �˲�����FIFO 0 */
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;				/* ʹ���˲��� */
	CAN_FilterInit(&CAN_FilterInitStructure);

	/* CAN FIFO0 ��Ϣ�����ж�ʹ�� */ 
	CAN_ITConfig(_CANx, CAN_IT_FMP0, ENABLE);
	return 0;
}

int bsp_CanReceive(CAN_TypeDef* _CANx, CanRxMsg* RxMessage)
{
	if(CAN2 == _CANx)
	{
		if(g_structCanRe.ucCanRxDatacount == 0) //����Ч����
		{
			return -1;
		}

    //���ʽ��ջ�����ʱ����ֹ�����ж�
    CAN_ITConfig(_CANx, CAN_IT_FMP0, DISABLE);

		RxMessage->StdId = g_structCanRe.sCanReceiveBuf[g_structCanRe.ucRxdataRead].StdId;
		RxMessage->ExtId = g_structCanRe.sCanReceiveBuf[g_structCanRe.ucRxdataRead].ExtId;
		RxMessage->IDE = g_structCanRe.sCanReceiveBuf[g_structCanRe.ucRxdataRead].IDE;
		RxMessage->RTR = g_structCanRe.sCanReceiveBuf[g_structCanRe.ucRxdataRead].RTR;
		RxMessage->DLC = g_structCanRe.sCanReceiveBuf[g_structCanRe.ucRxdataRead].DLC;
		RxMessage->Data[0] = g_structCanRe.sCanReceiveBuf[g_structCanRe.ucRxdataRead].Data[0];
		RxMessage->Data[1] = g_structCanRe.sCanReceiveBuf[g_structCanRe.ucRxdataRead].Data[1];
		RxMessage->Data[2] = g_structCanRe.sCanReceiveBuf[g_structCanRe.ucRxdataRead].Data[2];
		RxMessage->Data[3] = g_structCanRe.sCanReceiveBuf[g_structCanRe.ucRxdataRead].Data[3];
		RxMessage->Data[4] = g_structCanRe.sCanReceiveBuf[g_structCanRe.ucRxdataRead].Data[4];
		RxMessage->Data[5] = g_structCanRe.sCanReceiveBuf[g_structCanRe.ucRxdataRead].Data[5];
		RxMessage->Data[6] = g_structCanRe.sCanReceiveBuf[g_structCanRe.ucRxdataRead].Data[6];
		RxMessage->Data[7] = g_structCanRe.sCanReceiveBuf[g_structCanRe.ucRxdataRead].Data[7];

		g_structCanRe.ucRxdataRead++;
    g_structCanRe.ucRxdataRead &= (CAN_BUFF_SIZE-1);

		g_structCanRe.ucCanRxDatacount--;

    CAN_ITConfig(_CANx, CAN_IT_FMP0, ENABLE);

		return 0;
	}
	else
	{
		return -1;
	}
}

#define CAN_WAIT_TXOK_TIMEOUT   50
int bsp_CanSend(CAN_TypeDef* _CANx, CanTxMsg* _pRxMessag)
{
	u32 i = 0;
	if(_pRxMessag == 0)//��Ч��ַ
		return -1;

	gs_CanSendState.ucCanSendMbox = CAN_Transmit(_CANx, _pRxMessag);
	gs_CanSendState.ucCanSendState = 0;

  //�ȴ�һ��ʱ��
	while((CAN_TransmitStatus(_CANx,gs_CanSendState.ucCanSendMbox) != CANTXOK))
	{
    if(i++*5 > CAN_WAIT_TXOK_TIMEOUT)
    {
      gs_CanSendState.ucCanSendState = -1;
      break;
    }

    OSTimeDlyHMSM(0,0,0,5);
	}

	return gs_CanSendState.ucCanSendState;
}

void CAN2_RX0_IRQHandler(void)
{
	OSIntEnter(); 
	Can2_RX_ISR();
	OSIntExit(); 
}

void CAN1_RX0_IRQHandler(void)
{
	OSIntEnter(); 
	Can1_RX_ISR();
	OSIntExit(); 
}


/*
*********************************************************************************************************
*	�� �� ��: can_ISR
*	����˵��: CAN�жϷ������. ��������� stm32f4xx_it.c�б�����
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/ 
static void Can2_RX_ISR(void)
{
	CAN_Receive(CAN2, CAN_FIFO0, &g_structCanRe.sCanReceiveBuf[g_structCanRe.ucRxdataWrite++]);
  g_structCanRe.ucRxdataWrite &= (CAN_BUFF_SIZE-1);

	if(g_structCanRe.ucCanRxDatacount == CAN_BUFF_SIZE)
  {
    APP_TRACE("error: can buffer overflow!!!\r\n");

    //buffer��ʱ�����ɵ�
    g_structCanRe.ucRxdataRead++;
    g_structCanRe.ucRxdataRead &= (CAN_BUFF_SIZE-1);
  }
  else
		g_structCanRe.ucCanRxDatacount++;
}

/*
*********************************************************************************************************
*	�� �� ��: can_ISR
*	����˵��: CAN�жϷ������. ��������� stm32f4xx_it.c�б�����
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/ 
static void Can1_RX_ISR(void)
{
}
