/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  AmAlgorithm2G.h
 * @Brief :      
 * @Author :     Chenghanbing (iwanthaha@sina.com)
 * @Version :    1.0
 * @Creat Date : 2023-01-03
 * ****************************************************************************
 */
#ifndef _AMALGORITHM2GDF_H
#define _AMALGORITHM2GDF_H

#include "stdint.h"

#define   Algo_MHZ                  1000000                  //算法对应频率400KHz=0.4MHz，单位MHz
#define   Algo_PI			        3.1416                   //参与运算的Pi精度
#define   Algo_UH			        0.000001                 //电感单位uH
#define   Algo_NH					0.000000001				 //电感单位nH
#define   Algo_PF			        1000000000000            //电容点位pF
#define   w(f)				        (2.0*Algo_PI*f*Algo_MHZ) //角度与弧度计算
#define   InductanceUH(x,f)         (Algo_UH*x*w(f))		 //电感uH计算	
#define   InductanceNH(x,f)         (Algo_NH*x*w(f))		 //电感nH计算
#define   CapcitorPF(x,f)           (Algo_PF/(x*w(f)))       //Note: add() for all defined EQU. eg.CapcitorPF(x,f), else error caculate...

//AmAlgorithmG2_LF
//低频端(400KHz)电感可调
extern void Am_Algorithm_LF_2G_CSetting(float CValue, float CsValue);
extern void Am_Algorithm_LF_2G_LSetting(uint16_t L2Max, uint16_t L2Zero, uint16_t L3Max, uint16_t L3Zero, uint16_t LScope);
extern void Am_Algorithm_LF_2G(float sensorReal, float sensorImag, float pwrFreq, float sensorVrms, float sensorIrms, uint16_t currentPosL2, uint16_t currentPosL3, uint16_t *targetPosL2out, uint16_t *targetPosL3out);
extern void Am_Algorithm_LF_2G_Debug(float *currentL2_Val, float *currentL3_Val, float *realLoad_Val, float *imagLoad_Val, float *targetL2_Val, float *targetL3_Val, float *tmpData1, float *tmpData2);

//AmAlgorithmG2_HF
//低频端(13.56MHz)电容可调
extern void Am_Algorithm_HF_2G_LSetting(float L1Value, float L2Value);
extern void Am_Algorithm_HF_2G_CSetting(uint16_t ClMax, uint16_t ClZero,uint16_t CtMax, uint16_t CtZero,uint16_t CScope);
extern void Am_Algorithm_HF_2G(float sensorReal, float sensorImag, float pwrFreq, float sensorVrms, float sensorIrms, uint16_t currentPosCl, uint16_t currentPosCt, uint16_t *targetPosClout, uint16_t *targetPosCtout, uint8_t taskPeriod);
extern void Am_Algorithm_HF_2G_Debug(float *currentCl_Val, float *currentCt_Val, float *realLoad_Val, float *imagLoad_Val, float *targetCl_Val, float *targetCt_Val, float *tmpData1, float *tmpData2);
//算法软件版本
extern uint8_t Am_Algorithm_2G_Version(uint8_t* pBuf);
#endif

