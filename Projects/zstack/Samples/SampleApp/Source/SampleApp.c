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
#include "hal_adc.h"
#include "MT_UART.h"
#include "DHT11.h"


void SampleApp_HandleKeys( uint8 shift, uint8 keys );
void sendTemp();
/******************************************
*              
*     �������ƣ�SampleApp_Init
*     �������ܣ�Ӧ�ò��ʼ��
*
*******************************************/
void SampleApp_Init( uint8 task_id )
{ 
    /*  ���ڲ��� */
    MT_UartInit(); //���ô���
    MT_UartRegisterTaskID(task_id); //�򿪴���
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
              sendTemp();
      }
}


void sendTemp()
{
    uint8 strData[20];
    uint8 temp[3]; 
    uint8 humidity[3];   

    
    DHT11();             //��ȡ��ʪ��

    //����ʪ�ȵ�ת�����ַ���
    temp[0]=wendu_shi+0x30;
    temp[1]=wendu_ge+0x30;
    humidity[0]=shidu_shi+0x30;
    humidity[1]=shidu_ge+0x30;
    
    osal_memcpy(strData,"TEMP:",5);
    osal_memcpy(&strData[5],temp,2);
    osal_memcpy(&strData[7],"   ",3);
    osal_memcpy(&strData[10],"Hum:",4);
    osal_memcpy(&strData[14],humidity,2);
    strData[16] = (uint8)'\n';
    HalUARTWrite(0,strData, 16);
    HalUARTWrite(0,"\n", 1); 
}
