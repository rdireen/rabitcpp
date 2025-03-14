/* ****************************************************************
 * DireenTech Inc.  (www.direentech.com)
 * Athr: Harry Direen PhD
 * Date: Feb. 2018
 *
******************************/

#ifndef DTI_DEV_CRC_CALCULATOR_H
#define DTI_DEV_CRC_CALCULATOR_H

#include <math.h>
#include <string>

namespace Rabit
{

    //CRC-16 Calculation
    //Reference:  http://www.sunshine2k.de/articles/coding/crc/understanding_crc.html
    uint16_t Compute_CRC16(char* byteArray, int noBytes);

    uint16_t Compute_FastDDSCRC16(uint8_t const *buffer, size_t len);

    bool CalculateTable_CRC16();


}


#endif //DTI_DEV_CRC_CALCULATOR_H
