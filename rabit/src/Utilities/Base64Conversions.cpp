/* ****************************************************************
 * DireenTech Inc.  (www.direentech.com)
 * Athr: Harry Direen PhD
 * Date: Feb. 2018
 *
  ******************************/


#include "Base64Conversions.h"

namespace Rabit
{

//Copy the string to the command response buffer... insure the command response
//buffer does not over flow.  Null terminate.
//Return the location of the null termination.
    int cpyStringToBuf(char *cmdRespBuf, int bufOffset, int bufSize,
                       const char *strMsg, bool addCmdTermination)
    {
        int n = bufOffset;
        int i = 0;
        for (i = 0; n < bufSize - 3; i++, n++)
        {
            if (strMsg[i] != 0)
            {
                cmdRespBuf[n] = strMsg[i];
            } else
            {
                break;
            }
        }
        if (addCmdTermination)
        {
            cmdRespBuf[n++] = SERIAL_CMD_TERMINATION;
            if (LINEFEED_TERMINATION)
                cmdRespBuf[n++] = LINEFEED_TERMINATION;   //Add Line feed
        }
        cmdRespBuf[n] = 0;  //Null Terminate so that it can be processed like an ASCII String.
        return n;   //Count does not include null termination
    }

    int addCommandTermination(char *cmdRespBuf)
    {
        int n = 0;
        cmdRespBuf[n++] = SERIAL_CMD_TERMINATION;
        if (LINEFEED_TERMINATION)
            cmdRespBuf[n++] = LINEFEED_TERMINATION;   //Add Line feed
        cmdRespBuf[n] = 0;  //Null Terminate so that it can be processed like an ASCII String.
        return n;   //Count does not include null termination
    }


    //Convert a single character hexidecimal value to a int.
//Returns the int value.
//Returns a -1 if the char value is not in:  [0, 1, .. 9] or [A,B,... F] or [a, b,...f]
    int charHexValToInt(char val)
    {
        int8_t num = -1;  //Error Value
        if (val >= '0' && val <= '9')
            num = val - '0';
        else if (val >= 'A' && val <= 'F')
            num = 10 + val - 'A';
        else if (val >= 'a' && val <= 'f')
            num = 10 + val - 'a';

        return num;
    }

//Convert a hex string:  1B to a uint8_t.
    uint8_t hexToUInt8(char *hexStr)
    {
        uint8_t num = 0;
        int8_t nval;
        for (int i = 0; i < 2; i++)
        {
            nval = charHexValToInt(hexStr[i]);
            if (nval >= 0)
            {
                num = num << 4;
                num = num + nval;
            } else
            {
                break;
            }
        }
        return num;
    }


//Convert a hex string:  1B7C to a uint16_t.
    uint16_t hexToUInt(char *hexStr)
    {
        uint16_t num = 0;
        int8_t nval;
        for (int i = 0; i < 4; i++)
        {
            nval = charHexValToInt(hexStr[i]);
            if (nval >= 0)
            {
                num = num << 4;
                num = num + nval;
            } else
            {
                break;
            }
        }
        return num;
    }

//Convert a hex string:  1B7C3E5D to a uint32_t.
    uint32_t hexToUInt32(char *hexStr)
    {
        uint32_t num = 0;
        int8_t nval;
        for (int i = 0; i < 8; i++)
        {
            nval = charHexValToInt(hexStr[i]);
            if (nval >= 0)
            {
                num = num << 4;
                num = num + nval;
            } else
            {
                break;
            }
        }
        return num;
    }

//Convert a number to a Hex string.
    int uint16ToHexStr(char *strBuf, uint16_t val, bool userUpperCase)
    {
        char c;
        int i;
        int n = 3;
        char alphaStart = userUpperCase ? 'A' : 'a';
        for (i = 0; i < 4; i++, n--)
        {
            c = (val >> (4 * n)) & 0xf;
            c = c < 10 ? c + '0' : c - 10 + alphaStart;
            strBuf[i] = c;
        }
        strBuf[i] = 0;
        return i;
    }

//Convert a number to a Hex string.
    int uint32ToHexStr(char *strBuf, uint32_t val, bool userUpperCase)
    {
        char c;
        int i;
        int n = 7;
        char alphaStart = userUpperCase ? 'A' : 'a';
        for (i = 0; i < 8; i++, n--)
        {
            c = (val >> (4 * n)) & 0xf;
            c = c < 10 ? c + '0' : c - 10 + alphaStart;
            strBuf[i] = c;
        }
        strBuf[i] = 0;
        return i;
    }

//Convert a number to a Hex string.
    int uint8ToHexStr(char *strBuf, uint8_t val, bool userUpperCase)
    {
        char c;
        int i;
        int n = 1;
        char alphaStart = userUpperCase ? 'A' : 'a';
        for (i = 0; i < 2; i++, n--)
        {
            c = (val >> (4 * n)) & 0xf;
            c = c < 10 ? c + '0' : c - 10 + alphaStart;
            strBuf[i] = c;
        }
        strBuf[i] = 0;
        return i;
    }

/******** To/From Base 64... table:  https://en.wikipedia.org/wiki/Base64 *************/
//Convert a number to a Base-64 string.

    static char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                    'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                    'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                    '4', '5', '6', '7', '8', '9', '+', '/'};

    char byteToB64(uint8_t val)
    {
        char c;
        val = val & 0x3F;	//Force to be 6-bits
        if(val < 26)
            c = 'A' + val;
        else if(val < 52)
            c = 'a' + (val - 26);
        else if(val < 62)
            c = '0' + (val - 52);
        else if(val == 62)
            c = '+';
        else
            c = '/';

        //return encoding_table[val];
        return c;
    }

    uint8_t B64ToByte(char c)
    {
        uint8_t val = 0;
        if (c >= 'A' && c <= 'Z')
            val = (uint8_t) (c - 'A');
        else if (c >= 'a' && c <= 'z')
            val = (uint8_t) (c - 'a') + 26;
        else if (c >= '0' && c <= '9')
            val = (uint8_t) (c - '0') + 52;
        else if (c == '+')
            val = 62;
        else if (c == '/')
            val = 63;
        else
            val = 0xff;    //Invalid base-64 value.

        return val;
    }

    typedef union
    {
        uint32_t uival;
        float fval;
    } UintFloat_t;

    typedef union
    {
        uint64_t uival;
        double dval;
    }Uint64Double_t;


//Assumes val i < 64
    int uint8ToB64Str(char *strBuf, uint8_t val)
    {
        int n = 0;
        val = val & 0x3F;
        strBuf[n++] = byteToB64(val);
        strBuf[n] = 0;
        return n;
    }

    int uint16ToB64Str(char *strBuf, uint16_t val)
    {
        uint8_t c;
        int i;
        for(i = 0; i < 2; i++)
        {
            c = (val >> (16 - 6 * (i + 1))) & 0x3f;
            strBuf[i] = byteToB64(c);
        }
        c = (val & 0x0F) << 2;
        strBuf[i++] = byteToB64(c);
        strBuf[i] = 0;
        return i;
    }

    int uint32ToB64Str(char *strBuf, uint32_t val)
    {
        uint8_t c;
        int i;
        for(i = 0; i < 5; i++)
        {
            c = (val >> (32 - 6 * (i + 1))) & 0x3f;
            strBuf[i] = byteToB64(c);
        }
        c = (val & 0x03) << 4;
        strBuf[i++] = byteToB64(c);
        strBuf[i] = 0;
        return i;
    }

    int floatToB64Str(char *strBuf, float val)
    {
        UintFloat_t cval;
        cval.fval = val;
        return uint32ToB64Str(strBuf, cval.uival);
    }

    //8-B64 characters
    int uint64To48BitB64Str(char* strBuf, uint64_t val)
    {
        uint8_t c;
        int i;

        for(i = 0; i < 8; i++)
        {
            c = (val >> (48 - 6 * (i + 1))) & 0x3f;
            strBuf[i] = byteToB64(c);
        }
        strBuf[i] = 0;
        return i;
    }

    int uint64ToB64Str(char* strBuf, uint64_t val)
    {
        uint8_t c;
        int i;
        for(i = 0; i < 10; i++)
        {
            c = (val >> (64 - 6 * (i + 1))) & 0x3f;
            strBuf[i] = byteToB64(c);
        }
        c = (val & 0x0F) << 2;
        strBuf[i++] = byteToB64(c);
        strBuf[i] = 0;
        return i;
    }

    int doubleToB64Str(char* strBuf, double val)
    {
        Uint64Double_t cval;
        cval.dval = val;
        return uint64ToB64Str(strBuf, cval.uival);
    }


//Convert base-64 string:  number.
//This assumes the byte value is < 64
    uint8_t base64ToUInt8(char *b64Str)   //1 B-64 char
    {
        return B64ToByte(b64Str[0]);
    }

    uint16_t base64ToUInt16(char *b64Str)  //3 B-64 char
    {
        uint16_t num = 0;
        int8_t nval;
        int i;
        for(i = 0; i < 2; i++)
        {
            nval = B64ToByte(b64Str[i]);
            if( nval < 64 )
            {
                num = num << 6;
                num = num + nval;
            }
            else
            {
                return num;
            }
        }
        nval = B64ToByte(b64Str[i]);
        if( nval < 64 )
        {
            num = num << 4;
            num = num + (nval >> 2);
        }
        return num;
    }


    uint32_t base64ToUInt32(char *b64Str)
    {
        uint32_t num = 0;
        int8_t nval;
        int i;
        for( i = 0; i < 5; i++)
        {
            nval = B64ToByte(b64Str[i]);
            if( nval < 64 )
            {
                num = num << 6;
                num = num + nval;
            }
            else
            {
                return num;
            }
        }
        nval = B64ToByte(b64Str[i]);
        if( nval < 64 )
        {
            num = num << 2;
            num = num + (nval >> 4);
        }
        return num;
    }

    float base64ToFloat(char *b64Str)  //6 B-64 char
    {
        UintFloat_t val;
        val.uival = base64ToUInt32(b64Str);
        return val.fval;
    }

    uint64_t base64_48BitToUInt64(char* b64Str)
    {
        uint64_t num = 0;
        int8_t nval;
        int i;
        for(i = 0; i < 8; i++)
        {
            nval = B64ToByte(b64Str[i]);
            if( nval < 64 )
            {
                num = num << 6;
                num = num + nval;
            }
            else
            {
                return num;
            }
        }
        return num;
    }

    //The 48 bit number is expected to represent
    //time in usec... convert to timestame in seconds
    double base64_48BitToTimeStampSec(char* b64Str)
    {
        uint64_t num = base64_48BitToUInt64(b64Str);
        double tsec = 1e-6 * (double)num;
        return tsec;
    }

    uint64_t base64ToUInt64(char* b64Str)
    {
        uint64_t num = 0;
        int8_t nval;
        int i;
        for(i = 0; i < 10; i++)
        {
            nval = B64ToByte(b64Str[i]);
            if( nval < 64 )
            {
                num = num << 6;
                num = num + nval;
            }
            else
            {
                return num;
            }
        }
        nval = B64ToByte(b64Str[i]);
        if( nval < 64 )
        {
            num = num << 2;
            num = num + (nval >> 4);
        }
        return num;
    }

    double base64ToDouble(char* b64Str)  //6 B-64 char
    {
        Uint64Double_t val;
        val.uival = base64ToUInt32(b64Str);
        return val.dval;
    }

}