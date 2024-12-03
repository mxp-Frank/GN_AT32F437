/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  AmAlgorithm1G.h
 * @Brief :      
 * @Author :     Chenghanbing (iwanthaha@sina.com)
 * @Version :    1.0
 * @Creat Date : 2022-12-08
 * ****************************************************************************
 */

#ifndef _AMALGORITHM_H
#define _AMALGORITHM_H

#include "stdint.h"


//Timing define
#define ALGO_REVERSE_TIME 		50        //反转延时50ms
#define ALGO_AMFINE_TIME 		25         //精调时间25ms
#define ALGO_CAPACITOR_NUM 		4

//Control and parmas
typedef struct _AlgoMotorControl_t
{
    //Motor info input
    int16_t CurrentSpeed;         //当前速度  p/s
    uint16_t CapPos;              //电容的当前位置, 范围: 0 ~ CAP_MAX_SCOPE

    //Sensor sampling input
    int8_t   SpeedDir;            // 速度方向, -1 0r 1
    uint16_t SpeedMod;            // 判断速度的模值
    uint16_t MagOrPhaseErr;
    
    uint16_t HighSpeedThr;        //用于调整电机的高速阈值
    uint16_t Deadband;            //用于调整电机的停止死区

    //Motor target Output
    int16_t TargetSpeed;          //目标速度

} AlgoMotorControl_t;

typedef struct _AlgoInternalParam_t
{
    /* 自动匹配算法参数 */
    uint16_t Deadband[ALGO_CAPACITOR_NUM];        /* Deadband for the two capacitors */

    /* 电机速度参数 */
    uint16_t MinSpeed[ALGO_CAPACITOR_NUM];        /* minimum speed of the motor, eg. 400 p/s */
    uint16_t MaxSpeed[ALGO_CAPACITOR_NUM];	     /* maximum speed of the motor, eg. 2000 p/s */
    uint16_t MaxBreakSpeed[ALGO_CAPACITOR_NUM];	 /* maximum break speed of the motor, eg. 2000 p/s */

} AlgoInternalParam_t;

typedef struct _AlgoUserParam_t
{
    /* 电容位置上下限 */
    uint16_t PosLowLimit[ALGO_CAPACITOR_NUM];     /* the low limits of the two capacitors, the range is [0, 10000], 0%~100% */
    uint16_t PosHighLimit[ALGO_CAPACITOR_NUM];    /* the high limits of the two capacitors, the range is [0, 10000], 0~100% */

} AlgoUserParam_t;


//Lib Interface
void Algorithm1G_Process(uint8_t capIndex, \
                                AlgoMotorControl_t *MotorControlA, \
                                AlgoMotorControl_t *MotorControlB, \
                                AlgoUserParam_t *UserParam, \
                                AlgoInternalParam_t *InternalParam, \
                                uint8_t taskPeriod, \
                                int8_t AutoMatchStatus);


#endif

