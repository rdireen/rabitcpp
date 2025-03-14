/* ****************************************************************
 * DireenTech Inc.  (www.direentech.com)
 * Athr: Harry Direen PhD
 * Date: Feb. 2018
 *
  ******************************/

#ifndef VIDERE_DEV_BASE64CONVERSIONS_H
#define VIDERE_DEV_BASE64CONVERSIONS_H

#include <string>
#include <vector>

//The Serial Input Command Terminator... this is
//typically a carrage return (13 or 0x0d);
#define SERIAL_CMD_TERMINATION '\r'

//If LINEFEED_TERMINATION is not equal to 0, then
//send the line feed termination after the
#define LINEFEED_TERMINATION '\n'

namespace Rabit
{
    //Copy the string to the command response buffer... insure the command response
    //buffer does not over flow.  Null terminate.
    //Return the location of the null termination.
    int cpyStringToBuf(char *cmdRespBuf, int bufOffset, int bufSize,
                       const char *strMsg, bool addCmdTermination);

    int addCommandTermination(char *cmdRespBuf);

    //Convert a single character hexidecimal value to a int.
    //Returns the int value.
    //Returns a -1 if the char value is not in:  [0, 1, .. 9] or [A,B,... F] or [a, b,...f]
    int charHexValToInt(char val);

    uint8_t hexToUInt8(char *hexStr);

    //Convert a hex string:  1B7C to a uint16_t.
    uint16_t hexToUInt(char *hexStr);

    uint32_t hexToUInt32(char *hexStr);

    //Convert a number to a Hex string.
    int uint16ToHexStr(char *strBuf, uint16_t val, bool userUpperCase = true);

    int uint32ToHexStr(char *strBuf, uint32_t val, bool userUpperCase = true);

    //Convert a number to a Hex string.
    int uint8ToHexStr(char *strBuf, uint8_t val, bool userUpperCase = true);

    //Convert a number to a Base-64 string.
    char byteToB64(uint8_t val);

    uint8_t B64ToByte(char c);

    int uint8ToB64Str(char *strBuf,  uint8_t val);

    int uint16ToB64Str(char *strBuf, uint16_t val);

    int uint32ToB64Str(char *strBuf, uint32_t val);

    int floatToB64Str(char *strBuf, float val);

    //8-B64 characters
    int uint64To48BitB64Str(char* strBuf, uint64_t val);

    int uint64ToB64Str(char* strBuf, uint64_t val);

    int doubleToB64Str(char* strBuf, double val);


    //Convert base-64 string:  number.
    uint8_t base64ToUInt8(char *b64Str);   //3 B-64 char

    uint16_t base64ToUInt16(char *b64Str);   //3 B-64 char

    uint32_t base64ToUInt32(char *b64Str);  //6 B-64 char

    uint64_t base64_48BitToUInt64(char* b64Str);

    //The 48 bit number is expected to represent
    //time in usec... convert to timestame in seconds
    double base64_48BitToTimeStampSec(char* b64Str);


    uint64_t base64ToUInt64(char* b64Str);

    float base64ToFloat(char *b64Str);  //6 B-64 char

}


#endif //VIDERE_DEV_BASE64CONVERSIONS_H
