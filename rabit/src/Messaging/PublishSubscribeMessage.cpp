/* ****************************************************************
 * Rabit Multi-Threaded Management System
 * Athrs: Randal Direen PhD
 *        Harry Direen PhD
 * www.direentech.com
 * Date: June 2016
 *
 *******************************************************************/

#include "PublishSubscribeMessage.h"

namespace Rabit
{

    std::type_index PublishSubscribeMessage::MsgTypeIndex() const
    {
        std::lock_guard<std::mutex> lk(_mutex);
        return _statusMessage->GetTypeIndex();
    }

    void PublishSubscribeMessage::Register_SomethingPublished(const boost::function<void()> &handler)
    {
        _sigPublished.connect(handler);
    }

    std::unique_ptr<RabitMessage> PublishSubscribeMessage::GetCopyOfMessage()
    {
        std::lock_guard<std::mutex> lk(_mutex);
        return std::move(_statusMessage->Clone());
    }

    void PublishSubscribeMessage::PostMessage(RabitMessage *msg_ptr)
    {
        std::lock_guard<std::mutex> lk(_mutex);
        msg_ptr->SetTimeNow();
        //Ensure the Copy process does not loose the mesages'
        //publish subscribe reference. This protects against message
        //copy processes that possibly clobber the GlobalPublishSubscribeMessageRef
        std::shared_ptr<PublishSubscribeMessage> psmr = _statusMessage->GetGlobalPublishSubscribeMessageRef();
        _statusMessage->CopyMessage(msg_ptr);
        _statusMessage->SetGlobalPublishSubscribeMessageRef(psmr);
        _sigPublished();
    }

    bool PublishSubscribeMessage::FetchMessage(RabitMessage *msg_ptr)
    {
        //If the Time stamps are equal.. we assume the two messages are
        //equal...  It is save to compare the two timestamps before
        //geting a mutex... if for any reason they do not compare equal
        //we will lock the mutex and get a copy of the message.  Even
        //though the compare is not atomic... this is ok as even a
        //partial write to a timestamp will cause the compare to fail
        //in which case we will lock the message and get a copy.
        //This approach allows a Fetch to be called and a lock and
        //copy obtained only if the message has changed.
        if (!_statusMessage->CompareTime(msg_ptr))
        {
            std::lock_guard<std::mutex> lk(_mutex);
            //Ensure the Copy process does not loose the mesages'
            //publish subscribe reference. This protects against message
            //copy processes that possibly clobber the GlobalPublishSubscribeMessageRef
            std::shared_ptr<PublishSubscribeMessage> psmr = msg_ptr->GetGlobalPublishSubscribeMessageRef();
            msg_ptr->CopyMessage(_statusMessage.get());
            msg_ptr->SetGlobalPublishSubscribeMessageRef(psmr);
            return true;
        }
        return false;
    }

    void PublishSubscribeMessage::ForceFetchMessage(RabitMessage *msg_ptr)
    {
        std::lock_guard<std::mutex> lk(_mutex);
        //Ensure the Copy process does not loose the mesages'
        //publish subscribe reference. This protects against message
        //copy processes that possibly clobber the GlobalPublishSubscribeMessageRef
        std::shared_ptr<PublishSubscribeMessage> psmr = msg_ptr->GetGlobalPublishSubscribeMessageRef();
        msg_ptr->CopyMessage(_statusMessage.get());
        msg_ptr->SetGlobalPublishSubscribeMessageRef(psmr);
    }

}
