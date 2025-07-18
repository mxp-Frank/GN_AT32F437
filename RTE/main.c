/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  main.c
 * @Brief :      
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0.6
 * @Creat Date : 2022-11-29
 * ****************************************************************************
 */

#include "Common.h"
#include "IF_SL_Cfg.h"
#include "IF_HAL_Cfg.h"
#include "IF_PT_SWC.h"

/* CONST & MACROS */
 
#define QUEUE_LENGTH         	8        //队列长度
/* DATA STRUCTURES */

/* LOCAL VARIABLES */
static uint32_t SysWakeTimer = 0;
static SemaphoreHandle_t mainSemaphore = NULL;

static TaskHandle_t SensorTask_Handler;
static TaskHandle_t	MainTask_Handler;
static TaskHandle_t InterfaceTask_Handler;
static TaskHandle_t PortTask_Handler;
static TaskHandle_t HwTestTask_Handler;
static TaskHandle_t UserHMITask_Handler;
static TaskHandle_t DebugHMITask_Handler;
static TaskHandle_t FpgaUpdateTask_Handler;
static TaskHandle_t ModbusRecvTask_Handler;
static TaskHandle_t ModbusSendTask_Handler;
/* GLOBAL VARIABLES */

/* FUNCTION PROTOTYPES */
static void Pt_Sensor_task_function(void *pvParameters); 
static void Pt_Main_task_function(void *pvParameters);
static void Pt_Interface_task_function(void *pvParameters);
static void PT_Port_task_function(void *pvParameters);
static void UserHMI_task_function(void *pvParameters);
static void DebugHMI_task_function(void *pvParameters);
static void FpgaHMI_task_function(void *pvParameters);
static void ModbusRecv_task_function(void *pvParameters);
static void ModbusSend_task_function(void *pvParameters);
static void HwTest_task_function(void *pvParameters);
/************************************************************************/
/* Global Functions Definitions                                          */
/************************************************************************/

int main(void)
{
	BaseType_t xReturn;
    IF_HAL_CfgInit();
   
    #ifdef BOOT
		IF_SL_Bootloader();
    #endif 
    IF_SL_CfgInit();

    /* disable global interrupt */
    __disable_irq(); 
	
	/* Semaphore initiation */
	mainSemaphore = xSemaphoreCreateBinary();
	FpgaReSemaphore = xSemaphoreCreateBinary();
	FpgaNfSemaphore = xSemaphoreCreateBinary();
	ModbusReSemaphore = xSemaphoreCreateBinary();
	ModbusNfSemaphore = xSemaphoreCreateBinary();
	/* queue initiation */
	
	UserQueue  = xQueueCreate(QUEUE_LENGTH, sizeof(CommMsg_t));   			//User Protocol
	FGIPv2Queue = xQueueCreate(QUEUE_LENGTH, sizeof(CommMsg_t));   			//FGIPv2 Protocol	
	BsipTxQueue  = xQueueCreate(QUEUE_LENGTH, sizeof(CommMsg_t));           //BSIP Protocol	
	ModbusRxQueue  = xQueueCreate(QUEUE_LENGTH, sizeof(CommMsg_t));         //Modbus Protocol		
	ModbusFWQueue  = xQueueCreate(QUEUE_LENGTH, sizeof(BSIPInfo_t));        //Modbus Firmware Protocol	
	FpgaFWQueue = xQueueCreate(QUEUE_LENGTH, sizeof(BSIPInfo_t));   		//Fpga Firmware Protocol	
	CmdQueue = xQueueCreate(QUEUE_LENGTH, sizeof(DeviceCmdMsg_t));		    //Cmd Control
	
	
    /*****Create task function******/
	xReturn = xTaskCreate(Pt_Sensor_task_function,"Sensor_task",512,NULL,5,&SensorTask_Handler);
	if(xReturn != pdPASS)
	{ 
        DEBUG_Print("Sensor_task could not be created as there was insufficient heap memory remaining.\r\n");
	}	
	xReturn = xTaskCreate(Pt_Main_task_function,  "main_task",512,NULL,5,&MainTask_Handler);
	if(xReturn != pdPASS)
	{ 
        DEBUG_Print("Main_task could not be created as there was insufficient heap memory remaining.\r\n");
	}
    xReturn = xTaskCreate(Pt_Interface_task_function,"Interface_task",512,NULL,4,&InterfaceTask_Handler);
	if(xReturn != pdPASS)
	{ 
        DEBUG_Print("Interface_task could not be created as there was insufficient heap memory remaining.\r\n");
	}
	xReturn = xTaskCreate(PT_Port_task_function,"Port_task",512,NULL,2,&PortTask_Handler);
	if(xReturn != pdPASS)
	{ 
        DEBUG_Print("Port_task could not be created as there was insufficient heap memory remaining.\r\n");
	}
	xReturn = xTaskCreate(UserHMI_task_function,"UserHMI_task",1024,NULL,3,&UserHMITask_Handler);
	if(xReturn != pdPASS)
	{ 
        DEBUG_Print("UserHMI_task could not be created as there was insufficient heap memory remaining.\r\n");
	}
	xReturn = xTaskCreate(DebugHMI_task_function,"DebugHMI_task",1024,NULL,2,&DebugHMITask_Handler);
	if(xReturn != pdPASS)
	{ 
        DEBUG_Print("DebugHMI_task could not be created as there was insufficient heap memory remaining.\r\n");
	}
	xReturn = xTaskCreate(FpgaHMI_task_function,"fpga_task",1024,NULL,2,&FpgaUpdateTask_Handler);
	if(xReturn != pdPASS)
	{ 
        DEBUG_Print("Fpga_task could not be created as there was insufficient heap memory remaining.\r\n");
	}
	
	xReturn = xTaskCreate(ModbusRecv_task_function,"modbus_task",1024,NULL,4,&ModbusRecvTask_Handler);
	if(xReturn != pdPASS)
	{ 
        DEBUG_Print("Modbus_task could not be created as there was insufficient heap memory remaining.\r\n");
	}
	xReturn = xTaskCreate(ModbusSend_task_function,"modbus_task",1024,NULL,4,&ModbusSendTask_Handler);
	if(xReturn != pdPASS)
	{ 
        DEBUG_Print("Modbus_task could not be created as there was insufficient heap memory remaining.\r\n");
	}
	xReturn = xTaskCreate(HwTest_task_function,"HwTest_task",256,NULL,1,&HwTestTask_Handler);
	if(xReturn != pdPASS)
	{ 
        DEBUG_Print("HwTest_task could not be created as there was insufficient heap memory remaining.\r\n");
	}
	
	/* enable global interrupt */
    __enable_irq();
	/* start scheduler */
	vTaskStartScheduler();	
}

/************************************************************************/
/* Local Functions Definitions                                          */
/************************************************************************/
/* Pt Input task function: Interface Input porcess */
static void PT_Port_task_function(void *pvParameters)
{
	uint32_t tick = 0;
	portTickType lastWakeTime = xTaskGetTickCount();
	
    while(1)
    {
		vTaskDelayUntil(&lastWakeTime,MAIN_LOOP_DT); //1ms周期延时	
		if(RATE_DO_EXECUTE(RATE_50_HZ, tick)) /** 50Hz 20ms update **/
		{		
			IF_Module_Output_Task();		//PORT输出接口		
			IF_SL_ExecuteAction();						
		}
		if(RATE_DO_EXECUTE(RATE_20_HZ, tick))  /** 20Hz  50ms update **/
		{
			IF_Module_CmdExecute_Task();		   //数据执行输入接口
		}		
		tick++;
    }
}

/*Pt Interface task function */
static void Pt_Interface_task_function(void *pvParameters)
{
	uint32_t tick = 0;
	portTickType lastWakeTime = xTaskGetTickCount();
    while(1)
    {
		vTaskDelayUntil(&lastWakeTime,MAIN_LOOP_DT); //1ms周期延时
		if(RATE_DO_EXECUTE(RATE_50_HZ, tick))  /** 50Hz  20ms update **/
		{
			IF_SL_InterfaceInput_Task();
		}
		if(RATE_DO_EXECUTE(RATE_10_HZ, tick))  /** 10Hz  100ms update **/
		{
			IF_SL_Sensor_Device_Task();
		}
		tick++;		
    }
}
/* Pt sensor task function: Pt sensor process */
static void Pt_Sensor_task_function(void *pvParameters)
{
    while(1)
    {
		IF_SL_Sensor_ReadFpga_Task();
		xSemaphoreGive(mainSemaphore);
		vTaskDelay(MAIN_TASK_PERIOD);
	}
}
/* Pt main task function: Pt main process */
static void Pt_Main_task_function(void *pvParameters)
{
	IF_Module_SWCInit();
    while(1)
    {
		if(pdTRUE == xSemaphoreTake(mainSemaphore, portMAX_DELAY))
		{
			IF_Module_Input_Task();
			IF_Module_Main_Task();
			IF_SL_Sensor_WriteFgpa_Task();
		}				
	}
}

/* HwTest task function */
static void HwTest_task_function(void *pvParameters)
{
    portTickType	lastWakeTime;
    lastWakeTime = xTaskGetTickCount();	
    while(1)
    { 
		IF_SL_Nvm_WriteSystemResetTimes(); 	
		IF_SL_SetInterfaceOutput(IOSIGN_DEBUGLEDONOFF , ON);		
		vTaskDelayUntil(&lastWakeTime, 500 / MAIN_LOOP_DT);
	#ifndef BOOT
		IF_SL_SetInterfaceOutput(IOSIGN_DEBUGLEDONOFF , OFF);	
		vTaskDelayUntil(&lastWakeTime, 500 / MAIN_LOOP_DT);
	#endif
    }
}

/* FUNCTION *****************************************************
 * Function Name : UserHMI_task_function
 * Description   : 在User接口函数
 * Parameter     : 无
 * Return        : 无
 * END *********************************************************/
static void UserHMI_task_function(void *pvParameters)
{
	
    while(1)
    {
		IF_SL_UartTask1();//blocked task
    }
}

/* FUNCTION *****************************************************
 * Function Name : DebugHMI_task_function
 * Description   : 在DeBug接口函数
 * Parameter     : 无
 * Return        : 无
 * END *********************************************************/
static void DebugHMI_task_function(void *pvParameters)
{
	
    while(1)
    {	
		IF_SL_UartTask2();//blocked task
    }
}
/* FUNCTION *****************************************************
 * Function Name : fpga_task_function
 * Description   : 在Lcd接口函数
 * Parameter     : 无
 * Return        : 无
 * END *********************************************************/
static void FpgaHMI_task_function(void *pvParameters)
{
    while(1)
    {
		IF_SL_UartTask3();//blocked task
    }
}
/* FUNCTION *****************************************************
 * Function Name : fpga_task_function
 * Description   : 在Lcd接口函数
 * Parameter     : 无
 * Return        : 无
 * END *********************************************************/
static void ModbusRecv_task_function(void *pvParameters)
{
    while(1)
    {
		IF_SL_UartReciveTask4();//blocked task
    }
}
static void ModbusSend_task_function(void *pvParameters)
{
    while(1)
    {
		IF_SL_UartSendTask4();//blocked task
    }
}

/* FUNCTION *****************************************************
 * Function Name : vApplicationIdleHook
 * Description   : 在空闲函数实现看门狗(实现FreeRTOS中的configUSE_IDLE_HOOK)
 * Parameter     : 无
 * Return        : 无
 * END *********************************************************/
void vApplicationIdleHook(void)
{
	SysWakeTimer++;
	if((SysWakeTimer % 100) == 0)
	{
		IF_SL_WDOG_FEED();	
	}	
}

//*****************************************************************************
//* END
//*****************************************************************************

