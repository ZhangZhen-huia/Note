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

/*����*/

StaticTask_t Idle_Task_TCB;
StackType_t Idle_Task_Stack[configMINIMAL_STACK_SIZE];

StaticTask_t Timer_Task_TCB;
StackType_t Timer_Task_Stack[configTIMER_TASK_STACK_DEPTH ];

void AppTaskCreatTask(void*arg);
TaskHandle_t StartTaskHandle;

void GiveTask(void*arg);
TaskHandle_t GiveTaskHandle;

void TakeTask(void*arg);
TaskHandle_t TakeTaskHandle;


#define KEY1_EVENT (0x01 << 0) 
#define KEY2_EVENT (0x01 << 1)

static void BSP_Init(void);/* ���ڳ�ʼ�����������Դ */




void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, 
																	 StackType_t **ppxIdleTaskStackBuffer, 
																	 uint32_t *pulIdleTaskStackSize);



int main(void)
{	

  /* ������Ӳ����ʼ�� */
  	BSP_Init();
	if(xTaskCreate(AppTaskCreatTask,"StartTask",512,NULL,1,&StartTaskHandle)!= errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY)
		vTaskStartScheduler();
	else
		return -1;
  while(1);   /* ��������ִ�е����� */    
}


void AppTaskCreatTask(void*arg)
{

	taskENTER_CRITICAL();
	if((xTaskCreate(GiveTask,"GiveTask",128,NULL,2,&GiveTaskHandle)) != errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY)
		printf("GiveTask Create Successfully	");

	if((xTaskCreate(TakeTask,"TakeTask",128,NULL,1,&TakeTaskHandle)) != errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY)
		printf("TakeTask Create Successfully	\r\n\n");


	vTaskDelete(NULL);
	taskEXIT_CRITICAL();
	while (1)
	{
		/* code */
	}
	
}

void TakeTask(void *arg)
{
	uint32_t r_event = 0;
	uint32_t last_event = 0;
	uint32_t xReturn;
	while (1)
	{

			xReturn = xTaskNotifyWait(0,0xFFFFFFFF,&r_event,portMAX_DELAY);	
			if( xReturn == pdTRUE )
				last_event |= r_event; 
			if(last_event == (KEY1_EVENT|KEY2_EVENT))
			{
				last_event = 0;
				printf ( "Key1 �� Key2 ������\n"); 
				LED1_TOGGLE;
			}
			else
			{
				last_event = r_event;
			}
		
			vTaskDelay(20);
	}
	
}


void GiveTask(void *arg)
{

	while (1)
	{
		if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON)
			printf ( "KEY1 ������\n" ),xTaskGenericNotify(TakeTaskHandle,(uint32_t)KEY1_EVENT,eSetBits,NULL);
		
		if(Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON)
			printf ( "KEY2 ������\n" ),xTaskGenericNotify(TakeTaskHandle,(uint32_t)KEY2_EVENT,eSetBits,NULL);
		
		vTaskDelay(20);
	}
	
	
}

static void BSP_Init(void)
{
	/*
	 * STM32�ж����ȼ�����Ϊ4����4bit��������ʾ��ռ���ȼ�����ΧΪ��0~15
	 * ���ȼ�����ֻ��Ҫ����һ�μ��ɣ��Ժ������������������Ҫ�õ��жϣ�
	 * ��ͳһ��������ȼ����飬ǧ��Ҫ�ٷ��飬�мɡ�
	 */
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
	
	/* LED ��ʼ�� */
	LED_GPIO_Config();

	/* ������ʼ�� */
	Key_GPIO_Config();
	
	/* ���ڳ�ʼ��	*/
	USART_Config();
  
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

