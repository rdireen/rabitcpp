#ifndef RABIT_MESSAGE
#define RABIT_MESSAGE
/* ****************************************************************
 * Rabit Multi-Threaded Management System
 * Athrs: Randal Direen PhD
 *        Harry Direen PhD
 * www.direentech.com
 * Date: June 2016
 *
 *******************************************************************/

#include <chrono>
#include <atomic>
#include <string>
#include <memory>
#include <typeinfo>
#include <typeindex>
#include "PublishSubscribeMessage.h"

namespace Rabit
{

    class PublishSubscribeMessage;

    class RabitMessage
    {
    protected:
        typedef std::chrono::high_resolution_clock::time_point TimeStamp;
        TimeStamp _timeStamp;

        //An optional reference to the global publish/subsribe message.
        std::shared_ptr<PublishSubscribeMessage> _globalPublishSubscribeMessageRef = nullptr;

    public:
        std::shared_ptr<PublishSubscribeMessage> GetGlobalPublishSubscribeMessageRef()
        {
            return _globalPublishSubscribeMessageRef;
        }

        void GlobalPublishSubscribeMessageRef(std::shared_ptr<PublishSubscribeMessage> val)
        {
            _globalPublishSubscribeMessageRef = val;
        }

    public:
        RabitMessage()
        {
            _timeStamp = std::chrono::high_resolution_clock::now();
        }


        //Clone is required to support the Publish/subscribe Message Process.
        //The standard Clone format is:
        //  virtual std::unique_ptr<DerivedRabitMsg> Clone() const
        //     {
        //         std::unique_ptr<DerivedRabitMsg> cloneMsg = std::unique_ptr<DerivedRabitMsg>(new DerivedRabitMsg(*this));
        //         return std::move(cloneMsg);
        //     }
        virtual std::unique_ptr<RabitMessage> Clone() const = 0;


        //Copy message specifically does not copy the
        //_globalPublishSubscribeMessageRef nor the message Type Name
        //the messages must be of the same type.
        virtual bool CopyMessage(RabitMessage *msg)
        {
            _timeStamp = msg->_timeStamp;
            //_messageTypeName = msg->GetMessageTypeName();
            return true;
        }

        virtual void Clear() = 0;

        virtual std::string ToString() const
        {}

        void CopyBase(const RabitMessage *msg)
        {
            _timeStamp = msg->_timeStamp;
            _globalPublishSubscribeMessageRef = msg->_globalPublishSubscribeMessageRef;
        }

        //Get a Unique ID for the Derived Rabit Message type.
        //This can be used to determine the type of Rabit Message
        //for message handling routines.
        std::type_index GetTypeIndex()
        {
            return typeid(*this);
        }

        void SetTimeNow()
        {
            _timeStamp = std::chrono::high_resolution_clock::now();
        }

        const TimeStamp &GetTimeStamp() const
        {
            return _timeStamp;
        }

        double TimeStampSeconds() const
        {
            using std::chrono::duration_cast;
            using std::chrono::duration;
            return (duration_cast<duration<double>>(_timeStamp.time_since_epoch())).count();
        }

        bool Register_SomethingPublished(const boost::function<void()> &handler);

        /**
         * @brief PostMessage
         * @throws GlobalPublishSubscribeException
         */
        void PostMessage();

        /**
         * @brief FetchMessage
         * @throws GlobalPublishSubscribeException
         * @return true if the message has been updated
         */
        bool FetchMessage();

        /*
         * If equal in time, this function returns true;
         */
        bool CompareTime(RabitMessage *rabitMessage)
        {
            if (this->GetTimeStamp() == rabitMessage->GetTimeStamp())
                return true;
            else
                return false;
        }

        virtual ~RabitMessage()
        {

        }
    };
}


#endif //RABIT_MESSAGE
