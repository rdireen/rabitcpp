/* ****************************************************************
 * Rabit Multi-Threaded Management System
 * Athrs: Randal Direen PhD
 *        Harry Direen PhD
 * www.direentech.com
 * Date: June 2016
 *
 *******************************************************************/

#ifndef RABIT_REACTOR
#define RABIT_REACTOR

#include <vector>
#include <memory>

#include "RabitManager.h"
#include "RabitWorkspace.h"

namespace Rabit
{

    class RabitReactor
    {

    private:
        std::vector<std::unique_ptr<RabitManager>> listOfManagers_;

    public:

        RabitReactor()
        {
            RabitWorkspace::GetWorkspace();
            listOfManagers_ = std::vector<std::unique_ptr<RabitManager>>();
        }

        void AddManager(std::unique_ptr<RabitManager> manager)
        {

            if (manager != nullptr)
            {
                //The order here is important... the move(manager) prevents access to the manager
                //after it is moved.
                RabitWorkspace::GetWorkspace()->AddManagerNameToListOfManagers(manager->GetManagerName());
                listOfManagers_.push_back(std::move(manager));
            }
        }

        void Run()
        {

            for (auto &manager : listOfManagers_)
            {
                manager->Run();
            }

            for (auto &manager : listOfManagers_)
            {
                manager->Join();
            }

            RabitWorkspace::GetWorkspace()->LeavingReactor();
        }

        void ShutdownAllManagers()
        {
            for (auto &manager : listOfManagers_)
            {
                manager->ShutdownManager();
            }
        }
    };
}


#endif //RABIT_REACTOR
