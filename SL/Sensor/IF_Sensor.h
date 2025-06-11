/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  IF_Sensor.h
 * @Brief :      
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-09
 * ****************************************************************************
 */
 
#ifndef _IF_SENSOR_H_
#define _IF_SENSOR_H_

#include "IF_PrjLib.h"
#include "IF_Board.h"
#include "DevSensor.h"
#include "FpgaSensor.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */
/******************************************************************************
* Interface functions                                                         *
******************************************************************************/
extern void IF_SensorInit(void);
extern void IF_Sensor_Fpga_Task(void);
extern void IF_Sensor_Device_Task(void);



extern uint16_t IF_Sensor_GetFanCurrent(void);
//Sensor_1G output var
extern int16_t IF_Sensor_GetPCBTemperature(void);
extern uint16_t IF_Sensor_GetPowerSupply(void);
extern int16_t IF_Sensor_GetRFTemperature(uint8_t Channel);


extern float IF_Fpga_GetMcuAlgSensor(uint8_t ChnNo);
extern float IF_Fpga_GetRegAlgSensor(uint8_t ChnNo);

extern uint8_t IF_Fpga_GetSyncInDutyCircle(void);
extern uint32_t IF_Fpga_GetSyncInFrequency(void);
extern uint8_t IF_Fpga_GetSyncOutMeasureDutyCircle(void);
extern uint32_t IF_Fpga_GetSyncOutMeasureFrequency(void);
extern uint8_t IF_Sensor_GetVersion(uint8_t *pBuf);

extern uint16_t IF_Sensor_GetDCBias(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif

 /*
*******************************************************************************
* END
*******************************************************************************
*/

