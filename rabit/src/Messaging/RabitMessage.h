#ifndef RABIT_MESSAGE
#define RABIT_MESSAGE
/* ****************************************************************
 * Rabit Multi-Threaded Management System
 * Athrs: Randal Direen PhD
 *        Harry Direen PhD
 * www.direentech.com
 * Date: June 2016
 *
 * Aug. 2020, added virtual serialize and deserialize methods.
 *******************************************************************/

#include <chrono>
#include <atomic>
#include <string>
#include <memory>
#include <typeinfo>
#include <typeindex>
#include "PublishSubscribeMessage.h"
#include "SystemTimeClock.h"
#include "ByteArrayReaderWriter.h"

namespace Rabit
{

    class PublishSubscribeMessage;

    class RabitMessage
    {
    protected:
        //typedef std::chrono::high_resolution_clock::time_point TimeStamp;
        //TimeStamp _timeStamp;
        double _timeStamp;

        //An optional reference to the global publish/subsribe message.
        std::shared_ptr<PublishSubscribeMessage> _globalPublishSubscribeMessageRef = nullptr;

    public:
        //The message type is for generic end-user use.
        //It may be used to help identify the type of message.
        //The user is responsible for setting, clearing and adjusting as needed.
        int MessageType = 0;

        //A generic source information available to the user.
        //It could be used for instance to indicate what message pool the
        //message came from.
        int MessageSource = 0;

        //A generic message destination available to the user.
        int MessageDestination = 0;

        std::shared_ptr<PublishSubscribeMessage> GetGlobalPublishSubscribeMessageRef()
        {
            return _globalPublishSubscribeMessageRef;
        }

        void SetGlobalPublishSubscribeMessageRef(std::shared_ptr<PublishSubscribeMessage> val)
        {
            _globalPublishSubscribeMessageRef = val;
        }


    public:
        RabitMessage()
        {
            //_timeStamp = std::chrono::high_resolution_clock::now();
            Clear();
            SetTimeNow();
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
        //The Concrete Message must be careful not to copy the _globalPublishSubscribeMessageRef
        //via a *this = cpyMessage.
        virtual bool CopyMessage(RabitMessage *msg)
        {
            _timeStamp = msg->_timeStamp;
            MessageType = msg->MessageType;
            MessageSource = msg->MessageSource;
            MessageDestination = msg->MessageDestination;
            return true;
        }

        virtual void Clear()
        {
            MessageType = 0;
            MessageSource = 0;
            MessageDestination = 0;
            _timeStamp = 0;
        }

        virtual std::string ToString() const
        {return nullptr;}

        //Copy Base Items as part of the Clone Message process.
        //Don't use this as part of a CopyMessage as it copies
        //the _globalPublishSubscribeMessageRef.
        void CopyBase(const RabitMessage *msg)
        {
            MessageType = msg->MessageType;
            MessageSource = msg->MessageSource;
            MessageDestination = msg->MessageDestination;
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
            //_timeStamp = std::chrono::high_resolution_clock::now();
            _timeStamp = SystemTimeClock::GetSystemTimeClock()->GetCurrentGpsTimeInSeconds();
        }

        double GetTimeStamp() const
        {
            return _timeStamp;
        }

        void SetTimeStamp(double timeSec)
        {
            _timeStamp = timeSec;
        }

        double TimeStampSeconds() const
        {
            //using std::chrono::duration_cast;
            //using std::chrono::duration;
            //return (duration_cast<duration<double>>(_timeStamp.time_since_epoch())).count();
            return _timeStamp;
        }

        bool Register_SomethingPublished(const boost::function<void()> &handler);

        /**
         * @brief PostMessage
         * @throws GlobalPublishSubscribeException
         */
        void PostMessage();

        //There seems to be an issue with PostMessage() on a Windows Build.  It may be the 
        //code generator for rabit messages.  The code believes it is calling PostMessageA().
        //This is a cheat to get around the issue that works.
        void PostMessageWin()
        {
            PostMessage();
        }

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

        int getSerializedMsgHeaderSize()
        {
            int size = 3 * sizeof(int32_t);
            size += sizeof(double);
            return size;
        }

        //An optional serialization of the message.
        //The "stype" may be used by the end-user to indicate the
        //type of serialization to perform.
        //Pass in a buffer that has enough room to serialize the message.
        //The maxBuffersize indicates the maximum number of bytes available
        //in the buffer.
        //Returns the number of bytes used in the buffer (serialization size).
        virtual int Serialize(uint8_t *buf, int maxBufSize, int stype = 0)
        {
            return 0;
        }

        int SerializeMsgHeader(Rabit::ByteArrayWriter &bw);

        //An optional de-serialization of the message.
        //The "stype" may be used by the end-user to indicate the
        //type of serialization to perform.
        //The buffer contains the serialized message.
        //The len value indicates the length in bytes of the serialized data.
        //Returns a value indicating success.
        virtual int DeSerialize(uint8_t *buf, int len, int stype = 0)
        {
            return 0;
        }

        int DeSerializeMsgHeader(Rabit::ByteArrayReader &br);


        virtual ~RabitMessage()
        {
        }

    };
}


#endif //RABIT_MESSAGE
