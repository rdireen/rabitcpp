/* ****************************************************************
 * Rabit Multi-Threaded Management System
 * Athrs: Randal Direen PhD
 *        Harry Direen PhD
 * www.direentech.com
 * Date: June 2016
 *
 *******************************************************************/

#ifndef MANAGER_CONTROL_MESSAGE
#define MANAGER_CONTROL_MESSAGE

#include <string>
#include <memory>
#include <sstream>
#include "RabitMessage.h"

namespace Rabit
{

    class ManagerControlMessage : public RabitMessage
    {

    private:
        bool _shutdownManager = false;

    public:
        std::string ManagerName = "Unknown";

        //A change in the ResetMgrStatsToggle will cause the manager
        //to reset it Stats.
        bool ResetMgrStatsToggle = false;

        //PublishMgrStatsTime_Sec controls how often the Manager
        //Stats are published.  A value of zero or less will disable
        //publishing the manager stats.
        double PublishMgrStatsTime_Sec = 10.0;

    public:
        bool GetShutdownManager()
        {
            return _shutdownManager;
        }

        void ShutdownManager(bool value)
        {
            _shutdownManager = value;
        }

    public:
        ManagerControlMessage() : RabitMessage()
        {
            Clear();
        }


        virtual std::unique_ptr<RabitMessage> Clone() const final
        {
            auto clone = std::unique_ptr<ManagerControlMessage>(new ManagerControlMessage(*this));
            return std::move(clone);
        }

        virtual bool CopyMessage(RabitMessage *msg) final
        {
            if (msg->GetTypeIndex() == std::type_index(typeid(ManagerControlMessage)))
            {
                RabitMessage::CopyMessage(msg);
                ManagerControlMessage* mcMsg = static_cast<ManagerControlMessage *>(msg);
                this->ManagerName = mcMsg->ManagerName;
                this->_shutdownManager = mcMsg->_shutdownManager;
                this->ResetMgrStatsToggle = mcMsg->ResetMgrStatsToggle;
                this->PublishMgrStatsTime_Sec = mcMsg->PublishMgrStatsTime_Sec;
                return true;
            }
            return false;
        }

        virtual void Clear() final
        {
            ManagerName = "Unknown";
            _shutdownManager = false;
            ResetMgrStatsToggle = false;
            PublishMgrStatsTime_Sec = 10.0;
        }

        virtual int Serialize(uint8_t *buf, int maxBufSize, int stype = 0);

        virtual int DeSerialize(uint8_t *buf, int len, int stype = 0);

        virtual std::string ToString() const final
        {
            std::ostringstream ss;
            ss << "ShutdownAllManagers = " << _shutdownManager
               << "   RunState = " << ResetMgrStatsToggle
               << "PulishMgrStatsTime = " << PublishMgrStatsTime_Sec;
            return ss.str();
        }
    };

}


#endif //MANAGER_CONTROL_MESSAGE
