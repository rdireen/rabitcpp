
#include "ManagerControlMessage.h"
#include "../Utilities/ByteArrayReaderWriter.h"

namespace Rabit
{

    int ManagerControlMessage::Serialize(uint8_t *buf, int maxBufSize, int stype)
    {
        Rabit::ByteArrayWriter bw(buf, maxBufSize, Rabit::EndianOrder_e::Endian_Big);
        //There is nothing special about the ordering of message variables...
        //It is just the the deserialization must use the same ordering.
        SerializeMsgHeader(bw);
        bw.writeString(ManagerName);
        bw.writeBool(_shutdownManager);
        bw.writeBool(ResetMgrStatsToggle);
        bw.writeDouble(PublishMgrStatsTime_Sec);
        return bw.Idx;
    }

    int ManagerControlMessage::DeSerialize(uint8_t *buf, int len, int stype)
    {
        Rabit::ByteArrayReader br(buf, len, Rabit::EndianOrder_e::Endian_Big);
        DeSerializeMsgHeader(br);
        br.readString(&ManagerName);
        _shutdownManager = br.readBool();
        ResetMgrStatsToggle = br.readBool();
        PublishMgrStatsTime_Sec = br.readDouble();
        return br.Idx;
    }


}