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
#include "boost/any.hpp"
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
            //Keep message alive until incase manager shuts down.
            //I'm not sure this is necessary... but it does not hurt.
            //The reference will only keep the first message that was used
            //to create a new publish/subscribe message alive.
            std::shared_ptr<RabitMessage> MgrMsgRef;
            std::shared_ptr<PublishSubscribeMessage> PSMsg;
        };

        std::unordered_map<std::string, MsgQueueStruct> _messageQueueDict;
        std::unordered_map<std::string, std::unique_ptr<PSMsgContainer> > _publishSubscribeMsgDict;
        std::shared_ptr<RabitMessage> _tmp;

        std::vector<std::string> _listOfManagerNames;

        friend class RabitReactor;


    private:
        RabitWorkspace()
        {}

        RabitWorkspace(const RabitWorkspace &rws);

        RabitWorkspace &operator=(const RabitWorkspace &rws);

        void AddManagerNameToListOfManagers(std::string mgrName)
        {
            if(!DoesManagerExist(mgrName))
            {
                _listOfManagerNames.push_back(mgrName);
            }
        }

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


        //Get the Index number for a manager.
        //There is no particuarl order other than the order the mangagers
        //were added to the rabit reactor.  The Index number can be used
        //as a unique ID for the manager
        //If the manager does not exist, a -1 will be returned.
        int GetManagerIndexNumber(std::string mgrName)
        {
            int idxNo = -1;
            for(int i = 0; i < _listOfManagerNames.size(); i++)
            {
                if( _listOfManagerNames[i] == mgrName)
                {
                    idxNo = i;
                    break;
                }
            }
            return idxNo;
        }

        bool DoesManagerExist(std::string mgrName)
        {
            return GetManagerIndexNumber(mgrName) >= 0 ? true : false;
        }

        //Get a copy of the list of Manager Names.
        std::vector<std::string> GetListOfManagerNames()
        {
            return _listOfManagerNames;
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
                msg->SetGlobalPublishSubscribeMessageRef(psMsgContainer->PSMsg);
                _publishSubscribeMsgDict[name] = std::move(psMsgContainer);
            }
            else
            {
                if (_publishSubscribeMsgDict[name]->PSMsg->MsgTypeIndex() == msg->GetTypeIndex())
                {
                    msg->SetGlobalPublishSubscribeMessageRef(_publishSubscribeMsgDict[name]->PSMsg);
                }
                else
                {
                    error = false;
                }
            }
            return error;
        }

        //Get a clone of the message in the publish subscribe list.
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

        //Get a copy of the message in the publish subscribe list.
        //Message copy is only made if the timestamp of the message provided
        //is different than the global message.
        //return true if message found and copied, otherwise return false.
        bool FetchMessage(std::string name, RabitMessage *msgPtr)
        {
            bool msgFound = false;
            auto search = _publishSubscribeMsgDict.find(name);
            if (search == _publishSubscribeMsgDict.end())
            {
                msgFound = _publishSubscribeMsgDict[name]->PSMsg->FetchMessage(msgPtr);
            }
            return msgFound;
        }

        //Get a copy of the message in the publish subscribe list.
        //return true if message found and copied, otherwise return false.
        bool ForceFetchMessage(std::string name, RabitMessage *msgPtr)
        {
            bool msgFound = false;
            auto search = _publishSubscribeMsgDict.find(name);
            if (search == _publishSubscribeMsgDict.end())
            {
                _publishSubscribeMsgDict[name]->PSMsg->ForceFetchMessage(msgPtr);
                msgFound = true;
            }
            return msgFound;
        }


    };
}

#endif //RABIT_WORKSPACE
