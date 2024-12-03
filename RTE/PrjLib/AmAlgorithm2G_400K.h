/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  AmAlgorithm2G.h
 * @Brief :      
 * @Author :     Chenghanbing (iwanthaha@sina.com)
 * @Version :    1.0
 * @Creat Date : 2023-01-03
 * ****************************************************************************
 */
#ifndef _AMALGORITHM2G_400K_H
#define _AMALGORITHM2G_400K_H

#include "stdint.h"


//AmAlgorithmG2
extern void Am_Algorithm_2G_400K_CCsSetting(float CValue, float CsValue);
extern void Am_Algorithm_2G_400K_LSetting(uint16_t L2Max, uint16_t L2Zero, uint16_t L3Max, uint16_t L3Zero, uint16_t LScope);
extern void Am_Algorithm_2G_400K(float sensorReal, float sensorImag, float pwrFreq, float sensorVrms, float sensorIrms, uint16_t currentPosL2, uint16_t currentPosL3, uint16_t *targetPosL2out, uint16_t *targetPosL3out);
extern uint8_t Am_Algorithm_2G_400K_Version(uint8_t* pBuf);
extern void Am_Algorithm_2G_400K_Debug(float *currentL2_Val, float *currentL3_Val, float *realLoad_Val, float *imagLoad_Val, float *targetL2_Val, float *targetL3_Val, float *tmpData1, float *tmpData2);
#endif

