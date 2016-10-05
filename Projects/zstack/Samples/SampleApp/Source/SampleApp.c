/*****************************************************************************
*  �ļ�����  �� ���ʵ������1
*    ����    �� ֣����
*    ʱ��    �� 2016/8/18
******************************************************************************/
#include "OSAL.h"
#include "ZDApp.h"
#include "SampleApp.h"

/* HAL */
#include "hal_led.h"
#include "hal_key.h"
#include "hal_uart.h"
#include "MT_UART.h"
#include "OSAL_Nv.h"

#define MY_DEFINE_UART_PORT 0  //�Զ��崮�ںţ�0��1��
#define RX_MAX_LENGTH 20       //���ջ��������ֵ��20���ֽ�
uint8 RX_BUFFER[RX_MAX_LENGTH]; //���ջ�����
void UartCallBackFunction(uint8 port, uint8 event); //�ص����������������������
void SampleApp_HandleKeys( uint8 shift, uint8 keys );

/*   ���ô���      */
halUARTCfg_t uartConfig; //���崮�����ýṹ�������
void Uart_Config(void); //����������


/*****************************************************************************
*  ��������  �� Uart_Config
*  ��������  �����ô���
*            ��
*    ����    �� ��
*   ����ֵ   �� ��
******************************************************************************/

void Uart_Config(void) 
{
  uartConfig.configured = TRUE; //��������
  uartConfig.baudRate = HAL_UART_BR_115200; //������
  uartConfig.flowControl = FALSE;
  uartConfig.flowControlThreshold = 64;
  uartConfig.rx.maxBufSize = 128;
  uartConfig.tx.maxBufSize = 218;
  uartConfig.idleTimeout = 6;
  uartConfig.intEnable = TRUE;
  uartConfig.callBackFunc = UartCallBackFunction;
}


/******************************************
*              
*     �������ƣ�SampleApp_Init
*     �������ܣ�Ӧ�ò��ʼ��
*
*******************************************/
void SampleApp_Init( uint8 task_id )
{ 
      
    /*  ���ڲ���  */
    Uart_Config(); //���ô���
    HalUARTOpen(MY_DEFINE_UART_PORT , &uartConfig); //�򿪴���
    RegisterForKeys( task_id ); // �Ǽ����еİ����¼�
}


/******************************************
*              
*     �������ƣ�SampleApp_ProcessEvent
*     �������ܣ�Ӧ�ò���������
*
*******************************************/
uint16 SampleApp_ProcessEvent( uint8 task_id, uint16 events )
{
      afIncomingMSGPacket_t *MSGpkt;
      (void)task_id;  // Intentionally unreferenced parameter

      if ( events & SYS_EVENT_MSG ) //����ϵͳ��Ϣ�ٽ����ж�
      {
            MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( task_id );
            while ( MSGpkt )
            {
              switch ( MSGpkt->hdr.event )
              {        
                   case KEY_CHANGE://�����¼�
                    SampleApp_HandleKeys(((keyChange_t *)MSGpkt)->state, ((keyChange_t *)MSGpkt)->keys );
                   break;
              }

              // Release the memory 
              osal_msg_deallocate( (uint8 *)MSGpkt );

              // Next - if one is available 
              MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( task_id );
            }

            // return unprocessed events 
            return (events ^ SYS_EVENT_MSG);
      }
      
      return 0;
}


/******************************************
*              
*     �������ƣ�SampleApp_HandleKeys
*     �������ܣ������¼�������
*
*******************************************/
void SampleApp_HandleKeys( uint8 shift, uint8 keys ) 
{
      (void)shift;  // Intentionally unreferenced parameter
      
      if ( keys & HAL_KEY_SW_6 ) //S1
      {
              HalLedSet(HAL_LED_1,HAL_LED_MODE_TOGGLE);//��תС��
      }
}

/*****************************************************************************
*  ��������  �� UartCallBackFunction
*  ��������  �� ���ڻص�����
*            ��
*    ����    �� port:���ں�
*            �� event:�¼�
*   ����ֵ   �� ��
******************************************************************************/
static void UartCallBackFunction(uint8 port , uint8 event)
{
  uint8 r_val;
  uint8 w_val = 24;
  uint8 r_s[3];
  uint8 RX_Length = 0; //���յ��ַ�����С
  RX_Length = Hal_UART_RxBufLen(MY_DEFINE_UART_PORT); //��ȡ�����ַ�����С��
  if(RX_Length != 0) //�����ݴ���
  {
            //��ȡ�������ݣ�
        HalUARTRead(MY_DEFINE_UART_PORT , RX_BUFFER , RX_Length); //��ȡ�ַ���
        if(osal_memcmp(RX_BUFFER,"nvinit",6))  //����ַ���ǰ����Ϊ"nvinit"
        {
          osal_nv_item_init(0x1200,1,NULL);
          HalUARTWrite(0,"InitOK\n",7);
        }
        else if(osal_memcmp(RX_BUFFER,"nvread",6))//����ַ���ǰ����Ϊ"nvread"
        {
          osal_nv_read(0x1200,0,1,&r_val);
          r_s[0] = r_val/10 + 0x30;
          r_s[1] = r_val%10 + 0x30;
          r_s[2] = '\n';
          HalUARTWrite(0,r_s,3);
          HalUARTWrite(0,"readOK\n",7);
        }
        else if(osal_memcmp(RX_BUFFER,"nvwrite",6))//����ַ���ǰ����Ϊ"nvwrite"
        {
          if( ('0' <= RX_BUFFER[7]) &&  (RX_BUFFER[7] <= '9') ) //ʹ�ø�ʽΪnvwrite+���֣�����Ϊ����
          {
            w_val = (RX_BUFFER[7]-'0') * 10;
            w_val += RX_BUFFER[8]-'0';
            osal_nv_write(0x1200,0,1,&w_val);
            HalUARTWrite(0,"writeOK\n",8);
          }
          else
          {
            HalUARTWrite(0,"err\n",4);
          }
        }
        
        //���ͻظ�����,ʹ�� hal_uart.h �Ľӿں�����
        HalUARTWrite(MY_DEFINE_UART_PORT ,  RX_BUFFER , RX_Length);//����
        
        HalUARTWrite(MY_DEFINE_UART_PORT ,  "\n" , 1);
  }
}