/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
/* ������Ӳ��bspͷ�ļ� */
#include "bsp_led.h"
#include "bsp_usart.h"
#include "bsp_key.h"


/*����*/
void AppTaskCreatTask(void*arg);
TaskHandle_t StartTaskHandle;

void SendTask(void*arg);
TaskHandle_t SendTaskHandle;

void RecTask(void*arg);
TaskHandle_t RecTaskHandle;

StaticTask_t Idle_Task_TCB;
StackType_t Idle_Task_Stack[configMINIMAL_STACK_SIZE];

/*��Ϣ����*/
QueueHandle_t TestQueueHandle;

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

	if(xTaskCreate(RecTask,"RecTask",50,NULL,1,&RecTaskHandle) != errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY)
	printf("RecTask Create successfully!	");

	if(xTaskCreate(SendTask,"SendTask",128,NULL,2,&SendTaskHandle) != errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY)
	printf("SendTask Create successfully!	");

	if((TestQueueHandle = xQueueCreate(4,4)) != NULL)
	printf("Queue Create successfully!\n\n");

	vTaskDelete(NULL);
	taskEXIT_CRITICAL();
	while (1)
	{
		/* code */
	}
	
}

void RecTask(void *arg)
{
	BaseType_t xReturn = pdTRUE;/* ����һ��������Ϣ����ֵ��Ĭ��Ϊ pdTRUE */ 
	char RecDATA[5]; /* ����һ��������Ϣ�ı��� */ 
	while (1)
	 { 
		xReturn = xQueueReceive( TestQueueHandle,RecDATA,portMAX_DELAY);  
		if (pdTRUE == xReturn) 
			printf("RecDATA:%s\n\n",RecDATA); 
		 else 
		 printf("ERR: 0x%lx\n\n",xReturn); 
 } 	
}

void SendTask(void *arg)
{
	char SendData1[5] = "Good"; 
	char SendData2[4] = "BYE";
	while (1)
	{
		if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON)
		{
			printf("Send Message1!	");
			if(xQueueSend(TestQueueHandle,SendData1,portMAX_DELAY) != errQUEUE_FULL)
				printf("Successfully!\n");
			else
				printf("Failed!\n");
		}
		if(Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON)
		{
			printf("Send Message2!	");
			if(xQueueSend(TestQueueHandle,SendData2,portMAX_DELAY) != errQUEUE_FULL)
				printf("Successfully!\n\n");
			else
				printf("Failed!\n\n");
		}
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

