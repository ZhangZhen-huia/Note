/* FreeRTOS头文件 */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
/* 开发板硬件bsp头文件 */
#include "bsp_led.h"
#include "bsp_usart.h"
#include "bsp_key.h"


/*任务*/
void AppTaskCreatTask(void*arg);
TaskHandle_t StartTaskHandle;

void SendTask(void*arg);
TaskHandle_t SendTaskHandle;

void RecTask(void*arg);
TaskHandle_t RecTaskHandle;

StaticTask_t Idle_Task_TCB;
StackType_t Idle_Task_Stack[configMINIMAL_STACK_SIZE];

/*消息队列*/
QueueHandle_t TestQueueHandle;

static void BSP_Init(void);/* 用于初始化板载相关资源 */




void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, 
																	 StackType_t **ppxIdleTaskStackBuffer, 
																	 uint32_t *pulIdleTaskStackSize);



int main(void)
{	

  /* 开发板硬件初始化 */
  	BSP_Init();
	if(xTaskCreate(AppTaskCreatTask,"StartTask",512,NULL,1,&StartTaskHandle)!= errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY)
		vTaskStartScheduler();
	else
		return -1;
  while(1);   /* 正常不会执行到这里 */    
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
	BaseType_t xReturn = pdTRUE;/* 定义一个创建信息返回值，默认为 pdTRUE */ 
	char RecDATA[5]; /* 定义一个接收消息的变量 */ 
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
	 * STM32中断优先级分组为4，即4bit都用来表示抢占优先级，范围为：0~15
	 * 优先级分组只需要分组一次即可，以后如果有其他的任务需要用到中断，
	 * 都统一用这个优先级分组，千万不要再分组，切忌。
	 */
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );
	
	/* LED 初始化 */
	LED_GPIO_Config();

	/* 按键初始化 */
	Key_GPIO_Config();
	
	/* 串口初始化	*/
	USART_Config();
  
}
 

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, 
								   StackType_t **ppxIdleTaskStackBuffer, 
								   uint32_t *pulIdleTaskStackSize)
{
	*ppxIdleTaskTCBBuffer=&Idle_Task_TCB;/* 任务控制块内存 */
	*ppxIdleTaskStackBuffer=Idle_Task_Stack;/* 任务堆栈内存 */
	*pulIdleTaskStackSize=configMINIMAL_STACK_SIZE;/* 任务堆栈大小 */
}

