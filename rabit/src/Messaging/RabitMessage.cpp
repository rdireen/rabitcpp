/* ****************************************************************
 * Rabit Multi-Threaded Management System
 * Athrs: Randal Direen PhD
 *        Harry Direen PhD
 * www.direentech.com
 * Date: June 2016
 *
 *******************************************************************/

#include "RabitMessage.h"
#include "GlobalPublishSubscribeException.h"

namespace Rabit
{

    bool RabitMessage::Register_SomethingPublished(const boost::function<void()> &handler)
    {
        auto gpsm = GetGlobalPublishSubscribeMessageRef();
        if (gpsm != nullptr)
        {
            gpsm->Register_SomethingPublished(handler);
        } else
        {
            throw GlobalPublishSubscribeException();
        }
        return true;
    }

    void RabitMessage::PostMessage()
    {
        auto gpsm = GetGlobalPublishSubscribeMessageRef();
        if (gpsm != nullptr)
        {
            gpsm->PostMessage(this);
        } else
        {
            throw GlobalPublishSubscribeException();
        }
    }

    bool RabitMessage::FetchMessage()
    {
        bool messChanged = false;
        auto gpsm = GetGlobalPublishSubscribeMessageRef();
        if (gpsm != nullptr)
        {
            messChanged = gpsm->FetchMessage(this);
        } else
        {
            throw GlobalPublishSubscribeException();
        }
        return messChanged;
    }

    int RabitMessage::SerializeMsgHeader(Rabit::ByteArrayWriter &bw )
    {
        int startIdx = bw.Idx;
        bw.writeInt32(MessageType);
        bw.writeInt32(MessageSource);
        bw.writeInt32(MessageDestination);
        bw.writeDouble(_timeStamp);
        return bw.Idx - startIdx;
    }


    int RabitMessage::DeSerializeMsgHeader(Rabit::ByteArrayReader &br )
    {
        int startIdx = br.Idx;
        MessageType = br.readInt32();
        MessageSource = br.readInt32();
        MessageDestination = br.readInt32();
        _timeStamp = br.readDouble();
        return br.Idx - startIdx;
    }
}
