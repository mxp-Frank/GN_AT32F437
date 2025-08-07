/**
 * ********************** Copyright (c) CHENGYUAN-TECH.COM *******************
 * @File Name :  IF_FpgaUnit.h--->external use
 * @Brief :      
 * @Author :     mengxingping(mengxingping@chengyuan-tech.com)
 * @Version :    1.0
 * @Creat Date : 2022-08-01
 * ****************************************************************************
 */

 #ifndef _IF_FPGAUNIT_H_
 #define _IF_FPGAUNIT_H_
 #include "IF_PrjLib.h"
 
#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */
typedef enum
{
  FPGA_RDDATA0      = 0x00,
  FPGA_RDDATA1      = 0x01,
  FPGA_RDDATA2      = 0x02,
  FPGA_RDDATA3      = 0x03
} FpgaRdData_type;

typedef enum
{
  FPGA_WRDATA0      = 0x00,
  FPGA_WRDATA1      = 0x01,
  FPGA_WRDATA2      = 0x02,
  FPGA_WRDATA3      = 0x03
} FpgaWrData_type;
 /******************************************************************************
 * Interface functions                                                         *
 ******************************************************************************/
extern void IF_FpgaUnitInit(void);
extern void IF_FpgaReg_ReadStart(void);
extern void IF_FpgaReg_ReadEnd(void);
extern uint8_t IF_FpgaReg_Read(uint16_t regAddr);
extern void IF_FpgaReg_Write(uint16_t regAddr, uint8_t regData);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif
 
 /*
*******************************************************************************
* END
*******************************************************************************
*/

