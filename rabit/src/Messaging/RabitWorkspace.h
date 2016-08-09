/* ****************************************************************
 * Rabit Multi-Threaded Management System
 * Athrs: Randal Direen PhD
 *        Harry Direen PhD
 * www.direentech.com
 * Date: June 2016
 *
 *******************************************************************/

#ifndef RABIT_WORKSPACE
#define RABIT_WORKSPACE

#include <memory>
#include <iostream>
#include <unordered_map>
#include <string>
#include <boost/any.hpp>
#include "RabitMessageQueue.h"
#include "RabitNonBlockingSPSCQueue.h"
#include "PublishSubscribeMessage.h"
#include "MessageNotRegisteredException.h"

namespace Rabit
{

    class RabitWorkspace
    {

    private:
        enum MsgQueueType_e
        {
            MQT_MPMC,       //Multiple Provider, Multiple Consumer, Mutex Locked
            MQT_SPSC,       //Single Provider, Single Consumer, Non-Locked.
        };

        struct MsgQueueStruct
        {
            MsgQueueType_e MsgQueueType;
            boost::any QueuePtr;
        };

        static RabitWorkspace *_rabitWorkspace;

        struct PSMsgContainer
        {
            std::shared_ptr<RabitMessage> MgrMsgRef;
            std::shared_ptr<PublishSubscribeMessage> PSMsg;
        };

        std::unordered_map<std::string, MsgQueueStruct> _messageQueueDict;
        std::unordered_map<std::string, std::unique_ptr<PSMsgContainer> > _publishSubscribeMsgDict;
        std::shared_ptr<RabitMessage> _tmp;

    private:
        RabitWorkspace()
        {}

        RabitWorkspace(const RabitWorkspace &rws);

        RabitWorkspace &operator=(const RabitWorkspace &rws);

    public:

        static RabitWorkspace *GetWorkspace()
        {
            if (!_rabitWorkspace)
                _rabitWorkspace = new RabitWorkspace();
            return _rabitWorkspace;
        }

        void LeavingReactor()
        {
            _messageQueueDict.clear();
            _publishSubscribeMsgDict.clear();
        }

        template<typename T>
        void AddManagerMessageQueue(std::string name, std::shared_ptr<RabitMessageQueue<T>> queue)
        {
            MsgQueueStruct mqs;
            mqs.MsgQueueType = MQT_MPMC;
            mqs.QueuePtr = queue;
            _messageQueueDict[name] = mqs;
        }

        template<typename T>
        void AddManagerMessageQueue(std::string name, std::shared_ptr<RabitNonBlockingSPSCQueue<T>> queue)
        {
            MsgQueueStruct mqs;
            mqs.MsgQueueType = MQT_SPSC;
            mqs.QueuePtr = queue;
            _messageQueueDict[name] = mqs;
        }

        template<typename T>
        T GetManagerMessageQueue(std::string name)
        {
            auto search = _messageQueueDict.find(name);
            if (search == _messageQueueDict.end())
            {
                throw MessageNotRegisteredException(name);
            }
            MsgQueueStruct mqs = _messageQueueDict[name];
            return boost::any_cast<T>(mqs.QueuePtr);
            /************************************
            if (mqs.MsgQueueType == MQT_MPMC)
            {
                return boost::any_cast<std::shared_ptr<RabitMessageQueue<T>>>(mqs.QueuePtr);
            }
            else
            {
                return boost::any_cast<std::shared_ptr<RabitNonBlockingSPSCQueue<T>>>(mqs.QueuePtr);
            }
             ***********************/
        }

        //Add a message to a Standard Message Queue.
        template<typename T>
        bool AddMessageToQueue(std::string name, T msg)
        {
            bool msgAddedToQueue = false;
            auto search = _messageQueueDict.find(name);
            if (search != _messageQueueDict.end())
            {
                MsgQueueStruct mqs = _messageQueueDict[name];
                if (mqs.MsgQueueType == MQT_MPMC)
                {
                    msgAddedToQueue = boost::any_cast<std::shared_ptr<RabitMessageQueue<T>>>
                                                              (mqs.QueuePtr)->AddMessage(msg);
                }
                else
                {
                    msgAddedToQueue = boost::any_cast<std::shared_ptr<RabitNonBlockingSPSCQueue<T>>>
                                                              (mqs.QueuePtr)->AddMessage(msg);
                }
            }
            return msgAddedToQueue;
        }


        //Register an event when a message is added to the Queue.
        template<typename T>
        bool Register_EnqueuedEvent(std::string name, const boost::function<void()> &handler)
        {
            auto search = _messageQueueDict.find(name);
            if (search == _messageQueueDict.end())
            {
                throw MessageNotRegisteredException(name);
            }
            MsgQueueStruct mqs = _messageQueueDict[name];
            if (mqs.MsgQueueType == MQT_MPMC)
            {
                boost::any_cast<std::shared_ptr<RabitMessageQueue<T>>>
                                        (mqs.QueuePtr)->Register_SomethingEnqueued(handler);
            }
            else
            {
                boost::any_cast<std::shared_ptr<RabitNonBlockingSPSCQueue<T>>>
                                        (mqs.QueuePtr)->Register_SomethingEnqueued(handler);

            }
        }

        //Register an event when a message is pulled from the Queue.
        template<typename T>
        bool Register_DequeuedEvent(std::string name, const boost::function<void()> &handler)
        {
            auto search = _messageQueueDict.find(name);
            if (search == _messageQueueDict.end())
            {
                throw MessageNotRegisteredException(name);
            }
            MsgQueueStruct mqs = _messageQueueDict[name];
            if (mqs.MsgQueueType == MQT_MPMC)
            {
                boost::any_cast<std::shared_ptr<RabitMessageQueue<T>>>
                                        (mqs.QueuePtr)->Register_SomethingDequeued(handler);
            }
            else
            {
                boost::any_cast<std::shared_ptr<RabitNonBlockingSPSCQueue<T>>>
                                        (mqs.QueuePtr)->Register_SomethingDequeued(handler);
            }
        }

        bool AddPublishSubscribeMessage(std::string name, std::shared_ptr<RabitMessage> msg)
        {
            bool error = false;
            auto search = _publishSubscribeMsgDict.find(name);
            if (search == _publishSubscribeMsgDict.end())
            {
                auto psMsgContainer = std::unique_ptr<PSMsgContainer>(new PSMsgContainer());
                psMsgContainer->MgrMsgRef = msg;
                psMsgContainer->PSMsg = std::make_shared<PublishSubscribeMessage>(msg->Clone());
                msg->GlobalPublishSubscribeMessageRef(psMsgContainer->PSMsg);
                _publishSubscribeMsgDict[name] = std::move(psMsgContainer);
            }
            else
            {
                if (_publishSubscribeMsgDict[name]->PSMsg->MsgTypeIndex() == msg->GetTypeIndex())
                {
                    msg->GlobalPublishSubscribeMessageRef(_publishSubscribeMsgDict[name]->PSMsg);
                }
                else
                {
                    error = false;
                }
            }
            return error;
        }

        std::unique_ptr<RabitMessage> GetMessage(std::string name)
        {
            auto search = _publishSubscribeMsgDict.find(name);
            std::unique_ptr<RabitMessage> rm = nullptr;
            if (search == _publishSubscribeMsgDict.end())
            {
                rm = _publishSubscribeMsgDict[name]->PSMsg->GetCopyOfMessage();
            }
            return std::move(rm);
        }

    };
}

#endif //RABIT_WORKSPACE
