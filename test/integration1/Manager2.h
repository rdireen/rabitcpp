#ifndef MANAGER2
#define MANAGER2

#include <string>
#include "Message1.h"
#include "RabitManager.h"

class Manager2 : public Rabit::RabitManager
{

private:
    typedef Rabit::RabitMessageQueue<std::string> StringMessageQueue;
    std::shared_ptr<StringMessageQueue> _squeue_sptr;

    std::shared_ptr<Message1> _message1_sptr;

public:
    Manager2(std::string name) : RabitManager(name)
    {

        this->SetWakeupTimeDelayMSec(5);
        _squeue_sptr = std::make_shared<StringMessageQueue>(10, "procstrings");
        this->AddManagerMessageQueue(_squeue_sptr->GetMessageQueueName(), _squeue_sptr);
        this->WakeUpManagerOnEnqueue(_squeue_sptr);

        _message1_sptr = std::make_shared<Message1>("Message1");
        this->AddPublishSubscribeMessage(_message1_sptr->GetMessageTypeName(), _message1_sptr);
        this->WakeUpManagerOnMessagePost(_message1_sptr);

    }

    void ExecuteUnitOfWork() final
    {
        std::string message;
        while(_squeue_sptr->GetMessage(message))
        {
            std::ostringstream os;
            os << this->GetManagerName() << ": Queue: " << message;
            this->AddMessageToQueue("printstrings", os.str());
        }

        if (_message1_sptr->FetchMessage())
        {
            if (_message1_sptr->a > 100)
                this->ShutdownAllManagers(true);
            std::ostringstream os;
            os << this->GetManagerName() << ": PSMes: " << _message1_sptr->a;
            this->AddMessageToQueue("printstrings", os.str());
        }

    }


};

#endif //MANAGER2
