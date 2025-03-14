/* ****************************************************************
 * DireenTech Inc.  (www.direentech.com)
 * Athr: Harry Direen PhD
 * Date: Oct 2016
 *
   *******************************************************************/

#include "ByteArrayReaderWriter.h"
#include <vector>
#include <sstream>

using namespace std;

namespace Rabit
{

    EndianOrder_e GetSystemEndianness()
    {
        int num = 1;
        if(*(char *)&num == 1)
            return Endian_Little;
        else
            return Endian_Big;
    }


    void ByteArrayWriter::Clear()
    {
        Idx = 0;
        for(int i = 0; i < ByteArraySize; i++)
            ByteArray[i] = 0;
    }


    bool ByteArrayWriter::addBytesToArray(uint8_t valArray[], int numBytes)
    {
        if (Idx + numBytes > ByteArraySize)
            return true;    //No room in the Inn.

        if (_endianOrder == _systemEndianness)
        {
            for (int i = 0; i < numBytes; ++i)
                ByteArray[Idx++] = valArray[i];
        }
        else
        {
            for (int i = numBytes - 1; i >= 0; --i)
                ByteArray[Idx++] = valArray[i];
        }
        return false;
    }

    /// <summary>
    /// Write NoBytes from the bArray into the writer, starting
    /// at the bArray offset of startIdx.
    /// </summary>
    /// <param name="bArray">The byte array with values.</param>
    /// <param name="startIdx">The start index into the byte array.</param>
    /// <param name="NoBytes">The no bytes to write.</param>
    /// <returns>The number of bytes read</returns>
    int ByteArrayWriter::WriteBytes(uint8_t bArray[], int bArraySize,
                                    int startIdx, int NoBytes)
    {
        int n1 = ByteArraySize - Idx;
        int n2 = bArraySize - startIdx;
        NoBytes = NoBytes > n1 ? n1 : NoBytes;
        NoBytes = NoBytes > n2 ? n2 : NoBytes;
        for (int i = 0; i < NoBytes; i++)
            ByteArray[Idx++] = bArray[startIdx++];

        return NoBytes;
    }

    bool ByteArrayWriter::writeBool(bool value)
    {
        uint8_t bv = value == true ? 1 : 0;
        uint8_t *bArray = (uint8_t*)&bv;
        return addBytesToArray(bArray, 1);
    }

    bool ByteArrayWriter::writeChar(char value)
    {
        uint8_t *bArray = (uint8_t*)&value;
        return addBytesToArray(bArray, sizeof(int8_t));
    }

    bool ByteArrayWriter::writeInt8(int8_t value)
    {
        uint8_t *bArray = (uint8_t*)&value;
        return addBytesToArray(bArray, sizeof(int8_t));
    }

    bool ByteArrayWriter::writeUInt8(uint8_t value)
    {
        uint8_t *bArray = (uint8_t*)&value;
        return addBytesToArray(bArray, sizeof(int8_t));
    }

    bool ByteArrayWriter::writeInt16(int16_t value)
    {
        uint8_t *bArray = (uint8_t*)&value;
        return addBytesToArray(bArray, sizeof(int16_t));
    }

    bool ByteArrayWriter::writeUInt16(uint16_t value)
    {
        uint8_t *bArray = (uint8_t*)&value;
        return addBytesToArray(bArray, sizeof(uint16_t));
    }

    bool ByteArrayWriter::writeInt32(int32_t value)
    {
        uint8_t *bArray = (uint8_t*)&value;
        return addBytesToArray(bArray, sizeof(int32_t));
    }

    bool ByteArrayWriter::writeUInt32(uint32_t value)
    {
        uint8_t *bArray = (uint8_t*)&value;
        return addBytesToArray(bArray, sizeof(uint32_t));
    }

    bool ByteArrayWriter::writeInt64(int64_t value)
    {
        uint8_t *bArray = (uint8_t*)&value;
        return addBytesToArray(bArray, sizeof(int64_t));
    }

    bool ByteArrayWriter::writeUInt64(uint64_t value)
    {
        uint8_t *bArray = (uint8_t*)&value;
        return addBytesToArray(bArray, sizeof(uint64_t));
    }

    bool ByteArrayWriter::writeFloat(float value)
    {
        uint8_t *bArray = (uint8_t*)&value;
        return addBytesToArray(bArray, sizeof(float));
    }

    bool ByteArrayWriter::writeDouble(double value)
    {
        uint8_t *bArray = (uint8_t*)&value;
        return addBytesToArray(bArray, sizeof(double));
    }

    bool ByteArrayWriter::writeString(const std::string &strVal)
    {
        bool error = true;
        if (strVal.length() < MaxStringSize
            && (Idx + strVal.length() <= ByteArraySize) )
        {
            uint16_t strSize = (uint16_t)strVal.length();
            writeUInt16(strSize);
            for(int i = 0; i < strSize; i++)
                ByteArray[Idx++] = (uint8_t)strVal[i];

            error = false;
        }
        return error;
    }

    bool ByteArrayWriter::writeString(const char * strVal, int strLenght)
    {
        bool error = true;
        if (strLenght < MaxStringSize
             &&  Idx + strLenght <= ByteArraySize)
        {
            uint16_t strSize = (uint16_t)strLenght;
            writeUInt16(strSize);
            for (int i = 0; i < strLenght; ++i)
                ByteArray[Idx++] = (uint8_t)strVal[i];

            error = false;
        }
        return error;
    }

 /************* Byte Array Reader *****************/

     void ByteArrayReader::Clear()
     {
         Idx = 0;
         for(int i = 0; i < ByteArraySize; i++)
             ByteArray[i] = 0;
     }

    void ByteArrayReader::getBytes(uint8_t *bArray, int numBytes)
    {
        if (Idx + numBytes <= ByteArraySize)
        {
            if (_endianOrder == _systemEndianness)
            {
                for (int i = 0; i < numBytes; ++i)
                    bArray[i] = ByteArray[Idx++];
            }
            else
            {
                for (int i = numBytes - 1; i >= 0; --i)
                    bArray[i] = ByteArray[Idx++];
            }
        }
    }

    bool ByteArrayReader::readBool()
    {
        uint8_t  value = 0;
        getBytes((uint8_t*)&value, 1);
        return value == 0 ? false : true;
    }

    char ByteArrayReader::readChar()
    {
        uint8_t value = 0;
        getBytes((uint8_t*)&value, 1);
        return (char)value;
    }

    int8_t ByteArrayReader::readInt8()
    {
        int8_t  value = 0;
        getBytes((uint8_t*)&value, 1);
        return value;
    }

    uint8_t ByteArrayReader::readUInt8()
    {
        uint8_t  value = 0;
        getBytes((uint8_t*)&value, 1);
        return value;
    }

    int16_t ByteArrayReader::readInt16()
    {
        int16_t value = 0;
        getBytes((uint8_t*)&value, sizeof(int16_t));
        return value;
    }

    uint16_t ByteArrayReader::readUInt16()
    {
        uint16_t value = 0;
        getBytes((uint8_t*)&value, sizeof(uint16_t));
        return value;
    }

    int32_t ByteArrayReader::readInt32()
    {
        int32_t value = 0;
        getBytes((uint8_t*)&value, sizeof(int32_t));
        return value;
    }

    uint32_t ByteArrayReader::readUInt32()
    {
        uint32_t value = 0;
        getBytes((uint8_t*)&value, sizeof(uint32_t));
        return value;
    }

    int64_t ByteArrayReader::readInt64()
    {
        int64_t value = 0;
        getBytes((uint8_t*)&value, sizeof(int64_t));
        return value;
    }

    uint64_t ByteArrayReader::readUInt64()
    {
        uint64_t value = 0;
        getBytes((uint8_t*)&value, sizeof(uint64_t));
        return value;
    }

    float ByteArrayReader::readFloat()
    {
        float value = 0;
        getBytes((uint8_t*)&value, sizeof(float));
        return value;
    }

    double ByteArrayReader::readDouble()
    {
        double value = 0;
        getBytes((uint8_t*)&value, sizeof(double));
        return value;
    }

    std::string ByteArrayReader::readString()
    {
        std::string strVal = "";
        if(Idx + 3 <= ByteArraySize)
        {
            uint16_t strSize = readUInt16();
            for(int i = 0; i < strSize; i++)
                strVal += ByteArray[Idx++];
        }
        return strVal;
    }


    bool ByteArrayReader::readString(std::string *strVal)
    {
        bool error = true;
        strVal->clear();
        if(Idx + 3 <= ByteArraySize)
        {
            uint16_t strSize = readUInt16();
            if(Idx + strSize > ByteArraySize)
                strSize = ByteArraySize - Idx;

            for(int i = 0; i < strSize; i++)
                strVal->push_back(ByteArray[Idx++]);

            error = false;
        }
        return error;
    }

    bool ByteArrayReader::readString(char strBuffer[], int strBufferLenght)
    {
        bool error = true;
        if(Idx + 3 <= ByteArraySize)
        {
            uint16_t strSize = readUInt16();
            if(Idx + strSize > ByteArraySize)
                strSize = ByteArraySize - Idx;

            strSize = strSize + 1 > strBufferLenght ? strBufferLenght - 1 : strSize;
            for(int i = 0; i < strSize; i++)
                strBuffer[i] = ByteArray[Idx++];

            strBuffer[strSize] = 0;
            error = false;
        }
        return error;
    }

}