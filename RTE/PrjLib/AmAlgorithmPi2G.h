/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  AmAlgorithmPi2G.h
 * @Brief :      
 * @Author :     Chenghanbing (iwanthaha@sina.com)
 * @Version :    1.0
 * @Creat Date : 2023-01-03
 * ****************************************************************************
 */
#ifndef _AMALGORITHMPI2G_H
#define _AMALGORITHMPI2G_H

#include "stdint.h"


//AmAlgorithmG2
extern void Am_AlgorithmPi_2G_LSetting(float l1Value, float l2Value, float l3Value, float l4Value);
extern void Am_AlgorithmPi_2G_CSetting(uint16_t ClMax, uint16_t ClZero,uint16_t CtMax, uint16_t CtZero,uint16_t CScope);
extern void Am_AlgorithmPi_2G_ZSetting(float targetZ_Real, float targetZ_Image);
extern void Am_AlgorithmPi_2G(float sensorReal, float sensorImag, float pwrFreq, float sensorVrms, float sensorIrms, uint16_t currentPosCl, uint16_t currentPosCt, uint16_t *targetPosClout, uint16_t *targetPosCtout, uint8_t taskPeriod);
extern void Am_AlgorithmPi_2G_End(void);
extern uint8_t Am_AlgorithmPi_2G_Version(uint8_t* pBuf);
extern void Am_AlgorithmPi_2G_Debug(float *currentCl_Val, float *currentCt_Val, float *realLoad_Val, float *imagLoad_Val, float *targetCl_Val, float *targetCt_Val, float *tmpData1, float *tmpData2);
extern void Am_AlgorithmPi_2G_OptimizeDebug(uint8_t *optOnVal, uint8_t *optConVal, uint8_t *bigLoadSTEPVal, uint8_t *bigTuneSTEPVal, uint16_t *currentLoadPos, uint16_t *currentTunePos,  uint16_t *outLoadPos, uint16_t *outTunePos);

#endif

