/* ****************************************************************
 * DireenTech Inc.  (www.direentech.com)
 * Athr: Harry Direen PhD
 * Date: Oct 2016
 *
 * Desc: ByteArrayReaderWriter
 * Two class ByteArrayReader and ByteArrayWriter are definded.
 * These classes are used for packing or unpacking data into a
 * byte array.  The endianness of the packing order can be defined.

  *******************************************************************/

#ifndef VIDERE_DEV_BYTEARRAYREADERWRITER_H
#define VIDERE_DEV_BYTEARRAYREADERWRITER_H

#include <stdlib.h>
#include <stdint.h>
#include <vector>
#include <string>

namespace Rabit
{

    typedef unsigned char byte;

    enum EndianOrder_e
    {
        Endian_Little,
        Endian_Big
    };

    EndianOrder_e GetSystemEndianness();

    class ByteArrayWriter
    {
        EndianOrder_e _endianOrder;
        EndianOrder_e _systemEndianness;

    public:
        uint8_t *ByteArray;
        int ByteArraySize;
        int Idx;       //Current index into the byte array

        //All strings being serialized must be smaller than this value.
        const int MaxStringSize = UINT16_MAX;

    public:
        ByteArrayWriter(uint8_t  *byteArrayPtr, int byteArraySize, EndianOrder_e endianness)
        {
            ByteArray = byteArrayPtr;
            ByteArraySize = byteArraySize;
            _endianOrder = endianness;
            _systemEndianness = GetSystemEndianness();
            Idx = 0;
        }

        void Clear();

        uint8_t *GetByteArrayAddrPtrAtIdx()
        {
            uint8_t *ptr = nullptr;
            if(ByteArray != nullptr)
            {
                ptr = &ByteArray[Idx];
            }
            return ptr;
        }

        int GetNumberBytesAvailable()
        {
            int noBytes = 0;
            if(ByteArray != nullptr)
            {
                noBytes = ByteArraySize - Idx;
            }
            return noBytes;
        }


        /// <summary>
        /// Write NoBytes from the bArray into the writer, starting
        /// at the bArray offset of startIdx.
        /// </summary>
        /// <param name="bArray">The byte array with values.</param>
        /// <param name="startIdx">The start index into the byte array.</param>
        /// <param name="NoBytes">The no bytes to write.</param>
        /// <returns>The number of bytes read</returns>
        int WriteBytes(uint8_t bArray[], int bArraySize,
                       int startIdx, int NoBytes);


        bool writeByte(uint8_t value)
        {
            bool error = true;
            if (Idx < ByteArraySize)
            {
                ByteArray[Idx++] = value;
                error = false;
            }
            return error;
        }

        bool writeBool(bool value);
        bool writeChar(char value);
        bool writeInt8(int8_t value);
        bool writeUInt8(uint8_t value);
        bool writeInt16(int16_t value);
        bool writeUInt16(uint16_t value);
        bool writeInt32(int32_t value);
        bool writeUInt32(uint32_t value);
        bool writeInt64(int64_t value);
        bool writeUInt64(uint64_t value);
        bool writeFloat(float value);
        bool writeDouble(double value);

        //The Length of the String will be written
        //as a uint16_t infront of the string.
        bool writeString(const std::string &strVal);

        //The Length of the String will be written
        //as a uint16_t infront of the string.
        bool writeString(const char * strVal, int strLength);

    protected:

        bool addBytesToArray(uint8_t valArray[], int numBytes);

    };


    class ByteArrayReader
    {
        EndianOrder_e _endianOrder;
        EndianOrder_e _systemEndianness;

    public:
        uint8_t  *ByteArray;
        int ByteArraySize;
        int Idx;       //Current index into the byte array

        //All strings being serialized must be smaller than this value.
        const int MaxStringSize = UINT16_MAX;

    public:
        ByteArrayReader(uint8_t  *byteArrayPtr, int byteArraySize, EndianOrder_e endianness)
        {
            ByteArray = byteArrayPtr;
            ByteArraySize = byteArraySize;
            _endianOrder = endianness;
            _systemEndianness = GetSystemEndianness();
            Idx = 0;
        }

        void Clear();

        uint8_t readByte()
        {
            uint8_t value = 0;
            if (Idx < ByteArraySize)
            {
                value = ByteArray[Idx++];
            }
            return value;
        }

        bool readBool();
        char readChar();
        int8_t readInt8();
        uint8_t readUInt8();
        int16_t readInt16();
        uint16_t readUInt16();
        int32_t readInt32();
        uint32_t readUInt32();
        int64_t readInt64();
        uint64_t readUInt64();
        float readFloat();
        double readDouble();
        std::string readString();

        //The Length of the String will be written
        //as a uint16_t infront of the string.
        bool readString(std::string *strVal);

        //The Length of the String will is written
        //as a uint16_t infront of the string.  The string will be
        //null terminated.
        bool readString(char strBuffer[], int strBufferLength);

    protected:

        void getBytes(uint8_t *bArray, int numBytes);

    };

}
#endif //VIDERE_DEV_BYTEARRAYREADERWRITER_H
