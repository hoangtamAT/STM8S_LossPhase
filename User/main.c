/**
  ******************************************************************************
  * @file    Project/main.c 
  * @author  MCD Application Team
  * @version V2.2.0
  * @date    30-September-2014
  * @brief   Main program body
   ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 


/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"
#include "string.h"
#include "stdio.h"

/* Private defines -----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

extern char receive_data[100], index, flagRX,start;
char type[4],i,int_stt;

void sendUartString(char *str);
void clearBuffer();
void delay(uint32_t n);
void  stringHandle();

void main(void)
{
  /* Infinite loop */
  UART1_DeInit();
  UART1_Init(9600,UART1_WORDLENGTH_8D,
             UART1_STOPBITS_1,
             UART1_PARITY_NO,
             UART1_SYNCMODE_CLOCK_DISABLE,
             UART1_MODE_TXRX_ENABLE);
  UART1_ITConfig(UART1_IT_RXNE_OR,ENABLE);
  UART1_Cmd(ENABLE);
  
  GPIO_Init(GPIOD,GPIO_PIN_3,GPIO_MODE_IN_PU_IT);
  GPIO_Init(GPIOD,GPIO_PIN_2,GPIO_MODE_OUT_OD_HIZ_SLOW);
  EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOD,EXTI_SENSITIVITY_FALL_ONLY);
  
    FLASH_SetProgrammingTime(FLASH_PROGRAMTIME_STANDARD);
/* Unlock Data memory */
  FLASH_Unlock(FLASH_MEMTYPE_DATA);
  while (FLASH_GetFlagStatus(FLASH_FLAG_DUL) == RESET);
  
  enableInterrupts();
  
  while(strstr((char const*)receive_data,"OK")==0)
  {
     sendUartString("AT\r\n");
     sendUartString("ATE0\r\n");
     delay(50000);
     if(strstr((char const*)receive_data,"OK")==0) clearBuffer();
  }
  sendUartString("AT+CLIP=1\r\n"); //display infomation call
  delay(10000);
  while(UART1_GetFlagStatus(UART1_FLAG_TXE)==RESET);
  sendUartString("ATE0\r\n"); //disable echo
  delay(10000);
  while(UART1_GetFlagStatus(UART1_FLAG_TXE)==RESET);
  sendUartString("AT+IPR=9600\r\n"); //set baudrate uart
  delay(10000);
  while(UART1_GetFlagStatus(UART1_FLAG_TXE)==RESET);
  sendUartString("AT+CMGF=1\r\n"); // format sms text
  delay(10000);
  while(UART1_GetFlagStatus(UART1_FLAG_TXE)==RESET);
  sendUartString("AT+CNMI=2,2,0,0,0\r\n"); //auto display inbox sms
  delay(10000);
  while(UART1_GetFlagStatus(UART1_FLAG_TXE)==RESET);
  sendUartString("AT+CUSD=1\r\n"); // enable USSD
  delay(10000);
  while(UART1_GetFlagStatus(UART1_FLAG_TXE)==RESET);
  sendUartString("AT+CMGDA=DEL ALL\r\n" ); // dell all msg
  delay(10000);
  while(UART1_GetFlagStatus(UART1_FLAG_TXE)==RESET);
  clearBuffer();

   
  while (1)
  {
    //if(start) stringHandle();
    if(int_stt!=1)
    {
      GPIO_Init(GPIOD,GPIO_PIN_2,GPIO_MODE_OUT_PP_LOW_SLOW);
      GPIO_WriteLow(GPIOD,GPIO_PIN_2);
      delay(50000);
      delay(50000);
      delay(50000);
      delay(50000);
      delay(50000);
      delay(50000);
      delay(50000);
      delay(50000);
      while(strstr(receive_data,"BUSY")==0)
      {
        clearBuffer();
        sendUartString("ATD");
        while(UART1_GetFlagStatus(UART1_FLAG_TXE)==RESET);
        delay(500);
        for(i=0; i<11; i++)
        {
          if(47 < FLASH_ReadByte(0x4000+i) <58){
            UART1_SendData8(FLASH_ReadByte(0x4000+i));
            while(UART1_GetFlagStatus(UART1_FLAG_TXE)==RESET);
          }
        }          
        delay(5000);
        sendUartString(";\r");
        delay(5000);
        while(UART1_GetFlagStatus(UART1_FLAG_TXE)==RESET);
        UART1_SendData8(26);
        while(UART1_GetFlagStatus(UART1_FLAG_TXE)==RESET);
        while(strstr(receive_data,"NO CARRIER")==0 && strstr(receive_data,"NO ANSWER")==0 && strstr(receive_data,"BUSY")==0);
        //clearBuffer();
        break;
      }
      if(strstr(receive_data,"BUSY"))
      {
      GPIO_Init(GPIOD,GPIO_PIN_2,GPIO_MODE_OUT_OD_HIZ_SLOW);
      halt();
      //sendUartString("halt");
      int_stt=1;
      }
    }
    
    
   
  }
}
void  stringHandle()
{
  char i,k;
  char phoneNumber[12]="";
  char qnt,numberSent[12]="";
  char a=0x22,b=0x2c;
  
  qnt=(strcspn(receive_data,&b))-(strcspn(receive_data,&a))-4;
  
//  while(strstr((char const*)receive_data,"CMT")==0)
//    {
//      e++;
//      delay(5000);
//      if(e>20){
//        sendUartString("time out!");
//        clearBuffer();
//        break;
//        }
//    }
  
//  while(strstr((char const*)receive_data,"#0000 sdt")==0)
//    {
//      e++;
//      delay(5000);
//      if(e>20){
//        sendUartString("time out!");
//        clearBuffer();
//        break;
//        }
//    }
  
  if(strstr((char const*)receive_data,"CMT")){
    if(strstr((char const*)receive_data,"#0000 sdt"))
    {
//      while(flagRX==0)
//      {
//        e++;
//        delay(5000);
//        if(e>20){
//          sendUartString("time out!");
//          clearBuffer();
//          break;
//        }
//      }
      if(flagRX)
      {
        for(k=1;k<qnt;k++)
        {
          numberSent[0]=48;
          numberSent[k]=receive_data[strcspn(receive_data,&a)+3+k];
        }
        for(i=0; i<11; i++)
        {
          if(47<receive_data[strcspn(receive_data,"#")+10+i]<58){
            phoneNumber[i]=receive_data[strcspn(receive_data,"#")+10+i];
            FLASH_ProgramByte(0x4000+i,phoneNumber[i]);
          } else phoneNumber[i]=0;
        }
        
        //sendSMS(numberSent,strcat(phoneNumber,"Da them sdt thanh cong"));
        
        sendUartString("AT+CMGS=");
        while(UART1_GetFlagStatus(UART1_FLAG_TXE)==RESET);
        UART1_SendData8(34);
        while(UART1_GetFlagStatus(UART1_FLAG_TXE)==RESET);
        sendUartString(numberSent);
        while(UART1_GetFlagStatus(UART1_FLAG_TXE)==RESET);
        UART1_SendData8(34);
        while(UART1_GetFlagStatus(UART1_FLAG_TXE)==RESET);
        sendUartString("\r");
        while(UART1_GetFlagStatus(UART1_FLAG_TXE)==RESET);
        sendUartString("Da them sdt thanh cong! :");
        while(UART1_GetFlagStatus(UART1_FLAG_TXE)==RESET);
        sendUartString(phoneNumber);
        while(UART1_GetFlagStatus(UART1_FLAG_TXE)==RESET);
        while(UART1_GetFlagStatus(UART1_FLAG_TXE)==RESET);
        UART1_SendData8(26);
        while(UART1_GetFlagStatus(UART1_FLAG_TXE)==RESET);
        clearBuffer();
      }
    }
    //============================
  }
}

void clearBuffer()
{
   uint8_t i;
   for(i=0;i<index;i++)
   {
     receive_data[i]=0;
   }
   index=0;
   start=0;
   int_stt=0;
   type[0]=type[1]=type[2]=type[3]=0;
   flagRX=0;
   
}
void sendUartString(char *str)
{
    while(*str){
        UART1_SendData8(*str);
        while(UART1_GetFlagStatus(UART1_FLAG_TXE)==RESET);
        str++;
      }
  }
void delay(uint32_t n)
{
    while (n>0) n--;
}
#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
