#include    <stdio.h>
#include    <ucos_ii.h>
#include    "bsp_uart.h"

#define UART1_TX_BUF_SIZE	1*512
#define UART1_RX_BUF_SIZE	1*512

#define UART2_TX_BUF_SIZE	1*512
#define UART2_RX_BUF_SIZE	1*512


UART_T g_tUart1;
uint8_t g_TxBuf1[UART1_TX_BUF_SIZE]; 	/* ���ͻ����� */
uint8_t g_RxBuf1[UART1_RX_BUF_SIZE]; 	/* ���ջ����� */

UART_T g_tUart2;
uint8_t g_TxBuf2[UART2_TX_BUF_SIZE]; 	/* ���ͻ����� */
uint8_t g_RxBuf2[UART2_RX_BUF_SIZE]; 	/* ���ջ����� */

#define	UART1_TX_GPIO		GPIOB
#define	UART1_TX_PIN		GPIO_Pin_6
#define	UART1_TX_PINSOURCE	GPIO_PinSource6

#define	UART1_RX_GPIO		GPIOB
#define	UART1_RX_PIN		GPIO_Pin_7
#define	UART1_RX_PINSOURCE	GPIO_PinSource7

#define	UART1_RCC			RCC_APB2Periph_USART1
#define	UART1_PORT_RCC		RCC_AHB1Periph_GPIOB


#define	UART2_TX_GPIO		GPIOD
#define	UART2_TX_PIN		GPIO_Pin_5
#define	UART2_TX_PINSOURCE	GPIO_PinSource5

#define	UART2_RX_GPIO		GPIOD
#define	UART2_RX_PIN		GPIO_Pin_6
#define	UART2_RX_PINSOURCE	GPIO_PinSource6

#define	UART2_RCC			RCC_APB1Periph_USART2
#define	UART2_PORT_RCC		RCC_AHB1Periph_GPIOD


//static int UartSend(UART_T *_pUart, const u8 *_ucaBuf, u16 _usLen);
static void UartIRQ(UART_T *_pUart);
static void UartVarInit(void);
UART_T *ComToUart(USART_TypeDef* _USARTx);



/****************************************************************************
u16 _stopbit:  USART_StopBits_1
input:
_USARTx: USART1/ USART2
baudrate: u32;
stopbit: USART_StopBits_1/ USART_StopBits_0_5/ USART_StopBits_2/ USART_StopBits_1_5
parity: USART_Parity_No/ USART_Parity_Even/ USART_Parity_Odd
******************************************************************************/

int bsp_InitUart(USART_TypeDef* _USARTx, u32 _baudrata, u16 _stopbit, u16 _parity)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	NVIC_InitTypeDef NVIC_InitStructure;

	USART_StructInit(&USART_InitStructure);
	GPIO_StructInit(&GPIO_InitStructure);
	UartVarInit();
	/***************************************************************
	PB6: USART1_TX; PB7:USART1_RX
	PD6: WIFI_USART2_RX; PD6: WIFI_USART2_TX
	***************************************************************/	
	if(USART1 == _USARTx)
	{
		RCC_AHB1PeriphClockCmd(UART1_PORT_RCC, ENABLE);
		RCC_APB2PeriphClockCmd(UART1_RCC, ENABLE);
		GPIO_PinAFConfig(UART1_TX_GPIO, UART1_TX_PINSOURCE, GPIO_AF_USART1);
		GPIO_PinAFConfig(UART1_RX_GPIO, UART1_RX_PINSOURCE, GPIO_AF_USART1);
		/* ���� USART Tx Ϊ���ù��� */
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	/* �������Ϊ���� */
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* �ڲ���������ʹ�� */
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	/* ����ģʽ */

		GPIO_InitStructure.GPIO_Pin = UART1_TX_PIN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(UART1_TX_GPIO, &GPIO_InitStructure);

		/* ���� USART Rx Ϊ���ù��� */
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_Pin = UART1_RX_PIN;
		GPIO_Init(UART1_RX_GPIO, &GPIO_InitStructure);

		
			/* Enable the USARTx Interrupt */
		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  //�����ȼ�Ϊ0(��ֵԽС,���ȼ�Խ��,0��������ȼ�)
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
	}
	else if(USART2 == _USARTx)
	{
		RCC_AHB1PeriphClockCmd(UART2_PORT_RCC, ENABLE);
		RCC_APB1PeriphClockCmd(UART2_RCC, ENABLE);
		GPIO_PinAFConfig(UART2_TX_GPIO, UART2_TX_PINSOURCE, GPIO_AF_USART2);
		GPIO_PinAFConfig(UART2_RX_GPIO, UART2_RX_PINSOURCE, GPIO_AF_USART2);
		/* ���� USART Tx Ϊ���ù��� */
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	/* �������Ϊ���� */
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* �ڲ���������ʹ�� */
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	/* ����ģʽ */

		GPIO_InitStructure.GPIO_Pin = UART2_TX_PIN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(UART2_TX_GPIO, &GPIO_InitStructure);

		/* ���� USART Rx Ϊ���ù��� */
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_Pin = UART2_RX_PIN;
		GPIO_Init(UART2_RX_GPIO, &GPIO_InitStructure);

		
			/* Enable the USARTx Interrupt */
		NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		  //�����ȼ�Ϊ1(��ֵԽС,���ȼ�Խ��,0��������ȼ�)
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
	}
	else
	{
		return -1;
	}
	USART_InitStructure.USART_BaudRate = _baudrata;	/* ������ */
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = _stopbit;
	USART_InitStructure.USART_Parity = _parity ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(_USARTx, &USART_InitStructure);

	USART_ITConfig(_USARTx,USART_IT_RXNE,ENABLE); 

	USART_Cmd(_USARTx, ENABLE);		/* ʹ�ܴ��� */
	return 0;
}


/*
*********************************************************************************************************
*	�� �� ��: UartIRQ
*	����˵��: ���жϷ��������ã�ͨ�ô����жϴ�����
*	��    ��: _pUart : �����豸
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void UartIRQ(UART_T *_pUart)
{
	/* ��������ж�  */
	if (USART_GetITStatus(_pUart->uart, USART_IT_RXNE) != RESET)
	{
		/* �Ӵ��ڽ������ݼĴ�����ȡ���ݴ�ŵ�����FIFO */
		uint8_t ch;

		ch = USART_ReceiveData(_pUart->uart);
		_pUart->pRxBuf[_pUart->usRxWrite] = ch;
		if (++_pUart->usRxWrite >= _pUart->usRxBufSize)
		{
			_pUart->usRxWrite = 0;
	    }
		
		if (_pUart->usRxCount < _pUart->usRxBufSize)
		{
			_pUart->usRxCount++;
		}		
		else
		{//����������
			_pUart->usRxRead++;
		}
		
		if (_pUart->usRxRead >= _pUart->usRxBufSize)
		{
			_pUart->usRxRead = 0;
		}

		/* �ص�����,֪ͨӦ�ó����յ�������,һ���Ƿ���1����Ϣ��������һ����� */
		//if (_pUart->usRxWrite == _pUart->usRxRead)
		//if (_pUart->usRxCount == 1)
		//{
		//	if (_pUart->ReciveNew)
		//	{
		//		_pUart->ReciveNew(ch);
		//	}
		//}
	}

	/* �����ͻ��������ж� */
	if (USART_GetITStatus(_pUart->uart, USART_IT_TXE) != RESET)
	{
		//if (_pUart->usTxRead == _pUart->usTxWrite)
		if (_pUart->usTxCount == 0)
		{
			/* ���ͻ�������������ȡ��ʱ�� ��ֹ���ͻ��������ж� ��ע�⣺��ʱ���1�����ݻ�δ����������ϣ�*/
			USART_ITConfig(_pUart->uart, USART_IT_TXE, DISABLE);

			/* ʹ�����ݷ�������ж� */
			USART_ITConfig(_pUart->uart, USART_IT_TC, ENABLE);
			/* ��״̬λ*/
			_pUart->ucTxState = 1;
		}
		else
		{
			/* �ӷ���FIFOȡ1���ֽ�д�봮�ڷ������ݼĴ��� */
			USART_SendData(_pUart->uart, _pUart->pTxBuf[_pUart->usTxRead]);
			if (++_pUart->usTxRead >= _pUart->usTxBufSize)
			{
				_pUart->usTxRead = 0;
			}
			_pUart->usTxCount--;
		}

	}
	/* ����bitλȫ��������ϵ��ж� */
	else if (USART_GetITStatus(_pUart->uart, USART_IT_TC) != RESET)
	{
		//if (_pUart->usTxRead == _pUart->usTxWrite)
		if (_pUart->usTxCount == 0)
		{
			/* �������FIFO������ȫ��������ϣ���ֹ���ݷ�������ж� */
			USART_ITConfig(_pUart->uart, USART_IT_TC, DISABLE);

			/* �ص�����, һ����������RS485ͨ�ţ���RS485оƬ����Ϊ����ģʽ��������ռ���� */
			if (_pUart->SendOver)
			{
				_pUart->SendOver();
			}
		}
		else
		{
			/* ��������£��������˷�֧ */

			/* �������FIFO�����ݻ�δ��ϣ���ӷ���FIFOȡ1������д�뷢�����ݼĴ��� */
			USART_SendData(_pUart->uart, _pUart->pTxBuf[_pUart->usTxRead]);
			if (++_pUart->usTxRead >= _pUart->usTxBufSize)
			{
				_pUart->usTxRead = 0;
			}
			_pUart->usTxCount--;
		}
	}
}


void USART1_IRQHandler(void)
{
	OSIntEnter(); 	
	UartIRQ(&g_tUart1);
	OSIntExit(); 
}

void USART2_IRQHandler(void)
{
	OSIntEnter(); 	
	UartIRQ(&g_tUart2);
	OSIntExit(); 
}



/*
*********************************************************************************************************
*	�� �� ��: UartVarInit
*	����˵��: ��ʼ��������صı���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void UartVarInit(void)
{
	g_tUart1.uart = USART1;						/* STM32 �����豸 */
	g_tUart1.pTxBuf = g_TxBuf1;					/* ���ͻ�����ָ�� */
	g_tUart1.pRxBuf = g_RxBuf1;					/* ���ջ�����ָ�� */
	g_tUart1.usTxBufSize = UART1_TX_BUF_SIZE;	/* ���ͻ�������С */
	g_tUart1.usRxBufSize = UART1_RX_BUF_SIZE;	/* ���ջ�������С */
	g_tUart1.usTxWrite = 0;						/* ����FIFOд���� */
	g_tUart1.usTxRead = 0;						/* ����FIFO������ */
	g_tUart1.usRxWrite = 0;						/* ����FIFOд���� */
	g_tUart1.usRxRead = 0;						/* ����FIFO������ */
	g_tUart1.usRxCount = 0;						/* ���յ��������ݸ��� */
	g_tUart1.usTxCount = 0;						/* �����͵����ݸ��� */
	g_tUart1.SendBefor = 0;						/* ��������ǰ�Ļص����� */
	g_tUart1.SendOver = 0;						/* ������Ϻ�Ļص����� */
	g_tUart1.ReciveNew = 0;						/* ���յ������ݺ�Ļص����� */

	g_tUart2.uart = USART2;						/* STM32 �����豸 */
	g_tUart2.pTxBuf = g_TxBuf2;					/* ���ͻ�����ָ�� */
	g_tUart2.pRxBuf = g_RxBuf2;					/* ���ջ�����ָ�� */
	g_tUart2.usTxBufSize = UART2_TX_BUF_SIZE;	/* ���ͻ�������С */
	g_tUart2.usRxBufSize = UART2_RX_BUF_SIZE;	/* ���ջ�������С */
	g_tUart2.usTxWrite = 0;						/* ����FIFOд���� */
	g_tUart2.usTxRead = 0;						/* ����FIFO������ */
	g_tUart2.usRxWrite = 0;						/* ����FIFOд���� */
	g_tUart2.usRxRead = 0;						/* ����FIFO������ */
	g_tUart2.usRxCount = 0;						/* ���յ��������ݸ��� */
	g_tUart2.usTxCount = 0;						/* �����͵����ݸ��� */
	g_tUart2.SendBefor = 0;						/* ��������ǰ�Ļص����� */
	g_tUart2.SendOver = 0;						/* ������Ϻ�Ļص����� */
	g_tUart2.ReciveNew = 0;						/* ���յ������ݺ�Ļص����� */

}



/*
*********************************************************************************************************
*	�� �� ��: ComToUart
*	����˵��: ��COM�˿ں�ת��ΪUARTָ��
*	��    ��: _ucPort: �˿ں�(COM1 - COM6)
*	�� �� ֵ: uartָ��
*********************************************************************************************************
*/
UART_T *ComToUart(USART_TypeDef* _USARTx)
{
	if (USART1 == _USARTx)
	{
 		return &g_tUart1;
	}
	else if (USART2 == _USARTx)
	{
		return &g_tUart2;
	}
	else
	{
		/* �����κδ��� */
		return 0;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_UartSend
*	����˵��: �򴮿ڷ���һ�����ݡ����ݷŵ����ͻ��������������أ����жϷ�������ں�̨��ɷ���
*	��    ��: _ucPort: �˿ں�(COM1 - COM6)
*			  _ucaBuf: �����͵����ݻ�����
*			  _usLen : ���ݳ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
int bsp_UartSend(USART_TypeDef* _USARTx, const u8 *_ucaBuf, u16 _usLen)
{
    //������ѯ����
    int i;
    for(i=0; i<_usLen; i++)
    {
      USART_SendData(_USARTx, (uint8_t)_ucaBuf[i]);
      while (USART_GetFlagStatus(_USARTx, USART_FLAG_TC) == RESET)
      {;}
    }

    return i;
}

#if 0
/*
*********************************************************************************************************
*	�� �� ��: UartSend
*	����˵��: ��д���ݵ�UART���ͻ�����,�����������жϡ��жϴ�����������Ϻ��Զ��رշ����ж�
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static int UartSend(UART_T *_pUart, const u8 *_ucaBuf, u16 _usLen)
{
	u32 tmvalue = 0;//��ʱ�ۼӲ���
	uint16_t i;
	if (_usLen > _pUart->usTxBufSize)
	{
		return -1;
	}

	for (i = 0; i < _usLen; i++)
	{
 		/* �����������뷢�ͻ����� */
		_pUart->pTxBuf[_pUart->usTxWrite] = _ucaBuf[i];

		if (++_pUart->usTxWrite >= _pUart->usTxBufSize)
		{
			_pUart->usTxWrite = 0;
		}
		_pUart->usTxCount++;
	}
	/* ��ʼ��״̬λ*/
	_pUart->ucTxState = 0; 
	USART_ITConfig(_pUart->uart, USART_IT_TXE, ENABLE);
	while(!_pUart->ucTxState)
	{
		tmvalue++;
		if(tmvalue > 2000 * _usLen)
			return -1;
	}
	return 0;
}
#endif


/*
*********************************************************************************************************
*	�� �� ��: bsp_UartReceive
*	����˵��: �Ӵ��ڻ�������ȡ1�ֽڣ��������������������ݾ���������
*	��    ��: _ucPort: �˿ں�(COM1 - COM6)
*			  _pByte: ���յ������ݴ���������ַ
*	�� �� ֵ: 0 ��ʾ������, 1 ��ʾ��ȡ����Ч�ֽ�
*********************************************************************************************************
*/
int bsp_UartReceive(USART_TypeDef* _USARTx, u8 *_prbuff, u16 _usrlen)
{
	UART_T *pUart;
	u16 i = 0;
	int   ret;

	pUart = ComToUart(_USARTx);
	if (pUart == 0)
	{//uart�˿ںŴ��󣬷���-1
		return -1;
	}

	DISABLE_INT();

	if(0 == pUart->usRxCount)
	{//û����Ч���ݣ����ش���
		ret = 0;
		goto EXIT;
	}

	if(_usrlen > pUart->usRxCount)//��������ݴ�����Ч���ݸ���, ������Ч���ݸ���
	{
		for(i = 0; i < pUart->usRxCount; i++)
		{
			*_prbuff = pUart->pRxBuf[pUart->usRxRead];
			if (++pUart->usRxRead >= pUart->usRxBufSize)
			{
				pUart->usRxRead = 0;
			}			
			_prbuff++;
		}
		pUart->usRxCount -= i;

		ret = i;

	}
	else 
	{
		for(i = 0; i < _usrlen; i++)
		{
			*_prbuff = pUart->pRxBuf[pUart->usRxRead];
			if (++pUart->usRxRead >= pUart->usRxBufSize)
			{
				pUart->usRxRead = 0;
			}
			pUart->usRxCount--;
			_prbuff++;
		}

		ret = i;
	}

EXIT:
	ENABLE_INT();
	return ret;
}



/*
*********************************************************************************************************
*	�� �� ��: comSendChar
*	����˵��: �򴮿ڷ���1���ֽڡ����ݷŵ����ͻ��������������أ����жϷ�������ں�̨��ɷ���
*	��    ��: _ucPort: �˿ں�(COM1 - COM6)
*			  _ucByte: �����͵�����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void comSendChar(uint8_t _ucByte)
{
	bsp_UartSend(USART1, &_ucByte, 1);
}

int bsp_UartRxNum(USART_TypeDef* _USARTx)
{
    UART_T *puart;

    puart = ComToUart(_USARTx);

    return puart->usRxCount;
}

void bsp_UartClearRx(USART_TypeDef* _USARTx)
{
    UART_T *puart;

    puart = ComToUart(_USARTx);

		DISABLE_INT();
    puart->usRxCount = 0;
    puart->usRxRead = puart->usRxWrite = 0;
    ENABLE_INT();
}

/*
*********************************************************************************************************
*	�� �� ��: fputc
*	����˵��: �ض���putc��������������ʹ��printf�����Ӵ���1��ӡ���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
int fputc(int ch, FILE *f)
{
#if 1	/* ����Ҫprintf���ַ�ͨ�������ж�FIFO���ͳ�ȥ��printf�������������� */
	comSendChar(ch);

	return ch;
#else	/* ����������ʽ����ÿ���ַ�,�ȴ����ݷ������ */
	/* дһ���ֽڵ�USART1 */
	USART_SendData(USART1, (uint8_t) ch);

	/* �ȴ����ͽ��� */
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
	{}

	return ch;
#endif
}


