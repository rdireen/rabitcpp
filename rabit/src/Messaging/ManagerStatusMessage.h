/* ****************************************************************
 * Rabit Multi-Threaded Management System
 * Athrs: Randal Direen PhD
 *        Harry Direen PhD
 * www.direentech.com
 * Date: June 2016
 *
 *******************************************************************/

#ifndef MANAGER_STATUS_MESSAGE
#define MANAGER_STATUS_MESSAGE

#include <string>
#include <memory>
#include <sstream>
#include "RabitMessage.h"
#include "ManagerStats.h"

namespace Rabit
{
    //The Message contains various information about a Managers
    //operation including a number of statistics on the operation
    //of the manager.
    class ManagerStatusMessage : public RabitMessage
    {

    public:
        //This is the Manager name for reference purposes.
        std::string ManagerName = "Unknown";

        enum RunningState_e
        {
            MgrState_Startup,
            MgrState_Running,
            MgrState_Shutdown
        };

        RunningState_e RunningState = MgrState_Startup;


        //This flag can be set to true if there is some error in the
        //manager, otherwize it will be false.
        bool ErrorCondition = false;

        //A user definded Code indicating the type of error occurring
        //in the manager.
        int ErrorCode = 0;

        ManagerStats_t ManagerStats;

    private:


    public:
        ManagerStatusMessage(std::string name) : RabitMessage(name)
        {
            ManagerStats.Clear();
        }

        virtual std::unique_ptr<RabitMessage> Clone() final
        {
            auto clone = std::unique_ptr<ManagerStatusMessage>(new ManagerStatusMessage(GetMessageTypeName()));
            clone->CopyBase(this);
            clone->ManagerName = this->ManagerName;
            clone->RunningState = this->RunningState;
            clone->ErrorCondition = this->ErrorCondition;
            clone->ErrorCode = this->ErrorCode;
            clone->ManagerStats = this->ManagerStats;
            return std::move(clone);
        }

        virtual bool CopyMessage(RabitMessage *msg) final
        {
            RabitMessage::CopyMessage(msg);
            if (msg->GetTypeIndex() == std::type_index(typeid(ManagerStatusMessage)))
            {
                ManagerStatusMessage* msMsg = static_cast<ManagerStatusMessage *>(msg);
                this->ManagerName = msMsg->ManagerName;
                this->RunningState = msMsg->RunningState;
                this->ErrorCondition = msMsg->ErrorCondition;
                this->ErrorCode = msMsg->ErrorCode;
                this->ManagerStats = msMsg->ManagerStats;
                return true;
            }
            return false;
        }

        virtual void Clear() final
        {
            //Don't Clear the Manager Name or Running State.
            //this->RunningState = MgrState_Startup;
            this->ErrorCondition = false;
            this->ErrorCode = 0;
            ManagerStats.Clear();
        }

        virtual std::string ToString() const final
        {
            std::ostringstream ss;
            ss << "ManagerName = " << ManagerName
                << "   RunningState = " << RunningState
                << "   ErrorCondition = " << ErrorCondition
                << "   ErrorCode = " << ErrorCode
                << "   ManagerStats = " << ManagerStats.ToString()
               << std::endl;
            return ss.str();
        }
    };
    


}


#endif //MANAGER_STATUS_MESSAGE
