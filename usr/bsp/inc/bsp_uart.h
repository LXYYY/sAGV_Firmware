#ifndef	_BSP_UART_H_
#define	_BSP_UART_H_

#include "stm32f4xx.h"
#include "bsp.h"



int bsp_InitUart(USART_TypeDef* _USARTx, u32 _baudrata, u16 _stopbit, u16 _parity);
int bsp_UartSend(USART_TypeDef* _USARTx, const u8 *_ucaBuf, u16 _usLen);
int bsp_UartReceive(USART_TypeDef* _USARTx, u8 *_prbuff, u16 _usrlen);
int bsp_UartRxNum(USART_TypeDef* _USARTx);
void bsp_UartClearRx(USART_TypeDef* _USARTx);

#if 0
void USART1_IRQHandler(void);
void USART2_IRQHandler(void);
#endif


typedef struct
{
	USART_TypeDef *uart;		/* STM32�ڲ������豸ָ�� */
	uint8_t *pTxBuf;			/* ���ͻ����� */
	uint8_t *pRxBuf;			/* ���ջ����� */
	uint16_t usTxBufSize;		/* ���ͻ�������С */
	uint16_t usRxBufSize;		/* ���ջ�������С */

	volatile uint16_t usTxWrite;			/* ���ͻ�����дָ�� */
	volatile uint16_t usTxRead;			/* ���ͻ�������ָ�� */
	volatile uint16_t usTxCount;			/* �ȴ����͵����ݸ��� */

	uint8_t	ucTxState;			/* ����״̬��־��������Ϊ1*/

	volatile uint16_t usRxWrite;			/* ���ջ�����дָ�� */
	volatile uint16_t usRxRead;			/* ���ջ�������ָ�� */
	volatile uint16_t usRxCount;			/* ��δ��ȡ�������ݸ��� */

	void (*SendBefor)(void); 	/* ��ʼ����֮ǰ�Ļص�����ָ�루��Ҫ����RS485�л�������ģʽ�� */
	void (*SendOver)(void); 	/* ������ϵĻص�����ָ�루��Ҫ����RS485������ģʽ�л�Ϊ����ģʽ�� */
	void (*ReciveNew)(uint8_t _byte);	/* �����յ����ݵĻص�����ָ�� */
}UART_T;

#endif

