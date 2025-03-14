
#include "ManagerStatusMessage.h"
#include "../Utilities/ByteArrayReaderWriter.h"

namespace Rabit{


    int ManagerStatusMessage::Serialize(uint8_t *buf, int maxBufSize, int stype)
    {
        Rabit::ByteArrayWriter bw(buf, maxBufSize, Rabit::EndianOrder_e::Endian_Big);
        //There is nothing special about the ordering of message variables...
        //It is just the the deserialization must use the same ordering.
        SerializeMsgHeader(bw);
        bw.writeString(ManagerName);
        bw.writeByte((uint8_t)RunningState);
        bw.writeBool(ErrorCondition);
        bw.writeInt32(ErrorCode);

        //Manager Stats
        bw.writeInt32(ManagerStats.TotalNumberOfExecuteCycles);
        bw.writeInt32(ManagerStats.NumberOfExecuteCycles);
        bw.writeDouble(ManagerStats.TimeSinceLastStatsReset_Sec);
        bw.writeDouble(ManagerStats.MinExecuteUnitOfWorkTime_Sec);
        bw.writeDouble(ManagerStats.MaxExecuteUnitOfWorkTime_Sec);
        bw.writeDouble(ManagerStats.AveExecuteUnitOfWorkTime_Sec);
        bw.writeDouble(ManagerStats.TotalExecuteUnitOfWorkTime_Sec);
        bw.writeDouble(ManagerStats.MinSleepTime_Sec);
        bw.writeDouble(ManagerStats.MaxSleepTime_Sec);
        bw.writeDouble(ManagerStats.AveSleepTime_Sec);
        bw.writeDouble(ManagerStats.TotalSleepTime_Sec);
        bw.writeInt32(ManagerStats.NumberOfWakeUpCallsWhileAsleep);
        bw.writeInt32(ManagerStats.NumberOfWakeUpCallsWhileAwake);
        return bw.Idx;
    }

    int ManagerStatusMessage::DeSerialize(uint8_t *buf, int len, int stype)
    {
        Rabit::ByteArrayReader br(buf, len, Rabit::EndianOrder_e::Endian_Big);
        DeSerializeMsgHeader(br);
        br.readString(&ManagerName);
        RunningState = (RunningState_e)br.readByte();
        ErrorCondition = br.readBool();
        ErrorCode = br.readInt32();

        ManagerStats.TotalNumberOfExecuteCycles = br.readInt32();
        ManagerStats.NumberOfExecuteCycles = br.readInt32();
        ManagerStats.TimeSinceLastStatsReset_Sec = br.readDouble();
        ManagerStats.MinExecuteUnitOfWorkTime_Sec = br.readDouble();
        ManagerStats.MaxExecuteUnitOfWorkTime_Sec = br.readDouble();
        ManagerStats.AveExecuteUnitOfWorkTime_Sec = br.readDouble();
        ManagerStats.TotalExecuteUnitOfWorkTime_Sec = br.readDouble();
        ManagerStats.MinSleepTime_Sec = br.readDouble();
        ManagerStats.MaxSleepTime_Sec = br.readDouble();
        ManagerStats.AveSleepTime_Sec = br.readDouble();
        ManagerStats.TotalSleepTime_Sec = br.readDouble();
        ManagerStats.NumberOfWakeUpCallsWhileAsleep = br.readInt32();
        ManagerStats.NumberOfWakeUpCallsWhileAwake = br.readInt32();

        return br.Idx;
    }



}