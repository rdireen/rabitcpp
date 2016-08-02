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
        //A change in the ResetMgrStatsToggle will cause the manager
        //to reset it Stats.
        bool ResetMgrStatsToggle = false;

        //PulishMgrStatsTime_Sec controls how often the Manager
        //Stats are published.  A value less than zero will disable
        //publishing the manager stats.
        float PulishMgrStatsTime_Sec = 10.0;

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
        ManagerControlMessage(std::string name) : RabitMessage(name)
        {
            _shutdownManager = false;
            ResetMgrStatsToggle = false;
            PulishMgrStatsTime_Sec = 10.0;
        }

        virtual std::unique_ptr<RabitMessage> Clone() final
        {
            auto clone = std::unique_ptr<ManagerControlMessage>(new ManagerControlMessage(GetMessageTypeName()));
            clone->CopyBase(this);
            clone->_shutdownManager = this->_shutdownManager;
            clone->ResetMgrStatsToggle = this->ResetMgrStatsToggle;
            clone->PulishMgrStatsTime_Sec = this->PulishMgrStatsTime_Sec;

            return std::move(clone);
        }

        virtual bool CopyMessage(RabitMessage *msg) final
        {
            RabitMessage::CopyMessage(msg);
            if (msg->GetTypeIndex() == std::type_index(typeid(ManagerControlMessage)))
            {
                ManagerControlMessage* mcMsg = static_cast<ManagerControlMessage *>(msg);
                this->_shutdownManager = mcMsg->_shutdownManager;
                this->ResetMgrStatsToggle = mcMsg->ResetMgrStatsToggle;
                this->PulishMgrStatsTime_Sec = mcMsg->PulishMgrStatsTime_Sec;
                return true;
            }
            return false;
        }

        virtual void Clear() final
        {
            _shutdownManager = false;
            ResetMgrStatsToggle = false;
            PulishMgrStatsTime_Sec = 10.0;
        }

        virtual std::string ToString() const final
        {
            std::ostringstream ss;
            ss << "ShutdownAllManagers = " << _shutdownManager
               << "   RunState = " << ResetMgrStatsToggle
               << "PulishMgrStatsTime = " << PulishMgrStatsTime_Sec;
            return ss.str();
        }
    };

}


#endif //MANAGER_CONTROL_MESSAGE
