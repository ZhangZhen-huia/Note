/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

/* ������Ӳ��bspͷ�ļ� */
#include "bsp_led.h"
#include "bsp_usart.h"
#include "bsp_key.h"


/*����*/
void AppTaskCreatTask(void*arg);
TaskHandle_t StartTaskHandle;

void KeyTask(void*arg);
TaskHandle_t KeyTaskHandle;

void LedTask(void*arg);
TaskHandle_t LedTaskHandle;

StaticTask_t Idle_Task_TCB;
StackType_t Idle_Task_Stack[configMINIMAL_STACK_SIZE];

/*��Ϣ����*/
EventGroupHandle_t Event_Handle;

static void BSP_Init(void);/* ���ڳ�ʼ�����������Դ */




void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, 
																	 StackType_t **ppxIdleTaskStackBuffer, 
																	 uint32_t *pulIdleTaskStackSize);
#define Key1Event			(0x01<<0)
#define Key2Event			(0x01<<1)


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

	if(xTaskCreate(LedTask,"LedTask",50,NULL,1,&LedTaskHandle) != errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY)
	printf("LedTask Create successfully!	");

	if(xTaskCreate(KeyTask,"KeyTask",128,NULL,2,&KeyTaskHandle) != errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY)
	printf("KeyTask Create successfully!	");

	if((Event_Handle = xEventGroupCreate()) != NULL)
	printf("Event_Handle Create successfully!\r\n\n");

	vTaskDelete(NULL);
	taskEXIT_CRITICAL();
	while (1)
	{
		/* code */
	}
	
}

void LedTask(void *arg)
{
	uint8_t led_flag=0;
	while (1)
	 { 
		if((xEventGroupWaitBits(Event_Handle,Key1Event|Key2Event,pdTRUE,pdTRUE,portMAX_DELAY)	& (Key1Event | Key2Event)) == (Key1Event | Key2Event) )
		{
			printf("Key1��Key2��������!\r\n");
			LED1_TOGGLE;
			led_flag = (led_flag == 0?1:0);
			if(led_flag)
				printf("LED ON\r\n");
			else
				printf("LED OFF\r\n");
		}
		else
		{
			printf("ERR!\r\n");
		}
		vTaskDelay(20);
 } 	
}

void KeyTask(void *arg)
{

	while (1)
	{
		if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON)
			xEventGroupSetBits(Event_Handle,Key1Event),printf("Key1Press!\r\n");
		
		if(Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON)
			xEventGroupSetBits(Event_Handle,Key2Event),printf("Key2Press!\r\n");
		
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

