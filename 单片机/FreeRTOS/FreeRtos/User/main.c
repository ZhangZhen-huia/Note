/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include "timers.h"

/* ������Ӳ��bspͷ�ļ� */
#include "bsp_led.h"
#include "bsp_usart.h"
#include "bsp_key.h"
#include "limits.h"
#include "bsp_spi.h"
#include "ff.h"


TaskHandle_t ReadFlashTaskHandle;
void ReadFlashTask(void* arg);


StaticTask_t Idle_Task_TCB;
StackType_t Idle_Task_Stack[configMINIMAL_STACK_SIZE];

StaticTask_t Timer_Task_TCB;
StackType_t Timer_Task_Stack[configTIMER_TASK_STACK_DEPTH ];

void AppTaskCreatTask(void*arg);
TaskHandle_t StartTaskHandle;




static void BSP_Init(void);


FATFS file;
FIL fp;
FRESULT filefeedback;
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, 
																	 StackType_t **ppxIdleTaskStackBuffer, 
																	 uint32_t *pulIdleTaskStackSize);


int main(void)
{	
	
	
 
  	BSP_Init();

	if(xTaskCreate(AppTaskCreatTask,"StartTask",512,NULL,1,&StartTaskHandle)!= errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY)
		vTaskStartScheduler();
	else
		return -1;
  while(1);   
}


void AppTaskCreatTask(void*arg)
{

	taskENTER_CRITICAL();
	if((xTaskCreate(ReadFlashTask,"ReadFlash",1024,NULL,2,&ReadFlashTaskHandle)) != errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY)



	vTaskDelete(NULL);
	taskEXIT_CRITICAL();
	while (1)
	{
		/* code */
	}
	
}


void ReadFlashTask(void* arg)
{
	UINT bw,br;
	char Writebyte[] = "���ˮˮˮˮ!\n";
	char Readbyte[200] = "";
	f_open(&fp,"1:��bcdeffgggghghgh.txt",FA_READ | FA_OPEN_ALWAYS | FA_WRITE);
	while(1)
	{

		if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON)
		{
			f_write(&fp,Writebyte,sizeof(Writebyte),&bw);
			
		}
		else if(Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON)
		{
			f_lseek(&fp,0);
			f_read(&fp,Readbyte,f_size(&fp),&br);
			f_close(&fp);
			f_mount(NULL,"1:",1);
		}
			
		vTaskDelay(10);
	}
	
}

static void BSP_Init(void)
{


	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
	
	LED_GPIO_Config();

	Key_GPIO_Config();
	
	USART_Config();
	
	SPI_FLASH_Init();
	
	if(f_mount(&file,"1:",1) ==13)
	{
		filefeedback = f_mkfs("1:",0,0);
		
		f_mount(NULL,"1:",1);
		f_mount(&file,"1:",1);
	}




	

  
}
 

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, 
								   StackType_t **ppxIdleTaskStackBuffer, 
								   uint32_t *pulIdleTaskStackSize)
{
	*ppxIdleTaskTCBBuffer=&Idle_Task_TCB;/* ������ƿ��ڴ� */
	*ppxIdleTaskStackBuffer=Idle_Task_Stack;/* �����ջ�ڴ� */
	*pulIdleTaskStackSize=configMINIMAL_STACK_SIZE;/* �����ջ��С */
}

void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize )
{
	*ppxTimerTaskTCBBuffer = &Timer_Task_TCB;
	*ppxTimerTaskStackBuffer = Timer_Task_Stack;
	*pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

