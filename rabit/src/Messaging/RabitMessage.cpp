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
}
