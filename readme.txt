�⹤��ֻ������Zigbee�ṩ�Ľӿ���ʵ�ְ�������LED�ơ�
������S1����ʱ������תLed1С�ơ�



Ҫ�ڳ�ʼ��ʱע�ᰴ���¼�����������ϵͳ�Ż����а����¼�ʱ֪ͨ���ǡ�
/******************************************
*              
*     �������ƣ�SampleApp_Init
*     �������ܣ�Ӧ�ò��ʼ��
*
*******************************************/
void SampleApp_Init( uint8 task_id )
{ 
    RegisterForKeys( task_id ); // �Ǽ����еİ����¼�
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
              HalLedSet(HAL_LED_1,HAL_LED_MODE_TOGGLE); //��תС��
      }
}


��DHT11.h,DHT11.c���빤��

�ڰ����е�����ʪ�Ⱥ���
void SampleApp_HandleKeys( uint8 shift, uint8 keys ) 
{
      (void)shift;  // Intentionally unreferenced parameter
      
      if ( keys & HAL_KEY_SW_6 ) //S1
      {
              HalLedSet(HAL_LED_1,HAL_LED_MODE_TOGGLE);//��תС��
              sendTemp();
      }
}


�򴮿ڷ��ͺ���
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
