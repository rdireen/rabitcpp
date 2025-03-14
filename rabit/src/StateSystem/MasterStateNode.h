/* ****************************************************************
 * Rabit Multi-Threaded Management System
 * Athrs: Randal Direen PhD
 *        Harry Direen PhD
 * www.direentech.com
 * Date: June 2020
 *
 *******************************************************************/

#ifndef MASTERSTATENODE_H_
#define MASTERSTATENODE_H_

#include "StateNode.h"

namespace Rabit
{
	class MasterStateNode : public StateNode
	{
	public:
        //The first user state provided in the ArrayOfStates corresponds
        //to the reset state.  This number is the offset in states
        //required for returning the next desired state.
        static const int UserStateNumberOffset = (int)StdStateName_e::RSSTN_Reset;

        static uint8_t UserStatesIndexNumberToStateID(int stateIdx)
        {
            stateIdx = stateIdx < 0 ? 0 : stateIdx > 250 ? 250 : stateIdx;
            return (uint8_t)(stateIdx + UserStateNumberOffset);
        }


	    //Max number of substates if the Master's Array of States
	    //is used.  The user may supply an array of states larger than
	    //this value.
	    static const int MaxNumberOfSubStates = 16;
        //Master State provided Array of States.
        StateNode* ArrayOfStates[MaxNumberOfSubStates];

    protected:
		
			int NumberOfStates = 0;
			
			//Pointer to an array of states.  The number of
			//states is given by NumberOfStates.
			//The 1st state in the list must be the Reset State.
			//    The reset state must run to the "Complete" State so the 
			//    Master knows the reset process which can take multiple execute
			//    cycles is complete.
			//The second state in the array must be a default "Run" State
			//the system goes to after reset.  Reset State must return: RSSTN_Complete
			//when the reset has finished its process.
			// If there is no NextState defined in any sub-state that goes to
			// the complete state, the master will go to the Run state.
			// Otherwise the master will go to the state defined in the
			// substate's NextState.
			//The user may have as many states above the Run state as desired.

			StateNode** _arrayOfStatesPtr = ArrayOfStates;
			
			//A Monitor function/method can be used to monitor
			//system parameters and indicate which node's state the 
			//node should be in or change to.  When using the Monitor
			//Function to set the state, set the desiredState=RSSTN_Run.
			//If the _MonitorFnPtr is left as nullptr, the desiredState input
			//will control the node's state.
			monitorFnPtr_t _MonitorFnPtr = nullptr;
			
			//Keep track of the last desired state, so that we know if and
			//when the input desired state changed.
			uint8_t _lastInputDesiredState = (uint8_t)StdStateName_e::RSSTN_Run;
            uint8_t _lastMontitorTargetState = (uint8_t)StdStateName_e::RSSTN_Run;
            uint8_t _targetState = (uint8_t)StdStateName_e::RSSTN_Run;

		public:

		MasterStateNode();
		
		//Setup the Control State.
		virtual bool Setup(RabitManager* rmgrPtr,
							void *workspacePtr = nullptr,
							StateNode** arrayOfStatesPtr = nullptr, 
							int numOfStates = 0,
							monitorFnPtr_t monFnPtr = nullptr);
		
		//Reset the State to the Starting Init State.
		virtual uint8_t Reset();

        virtual uint8_t GetSubStateState(int substateID = -1);


        //Pass in the desired state of operation.  The basic desiredState
		//are:
		//RSSTN_Reset:  causes the state node to go to reset, typically used
		//on the first call to the Master State to cause it to go to the reset
		//state.  RSSTN_Reset input overrides the MonitorFn state.
		//When the Reset State is complete, the MasterState will go to the
		//Run State.  The Run State must be the default state of the set
		//of states.  If desiredState = RSSTN_Reset, then after the MasterState
		//completes the reset process, it will treat desiredState = RSSTN_Reset
		//as: desiredState = RSSTN_Run.  This prevents the Master Node from
		//continually running throught the reset state.
		//
		//RSSTN_Complete: causes the master node to go to the complete state.
		//RSSTN_Complete input overrides the MonitorFn state.
		//
		//RSSTN_Run or any value above this causes the node to go to that state.
		//The desiredState = RSSTN_Run, has special meaning.  This condition
		//informs the MasterState that it should proceed through the various
		//states based upon the MonitorFn input and substate inputs. In otherwords
		//desiredState = RSSTN_Run is not a dictive to go to the run state.
		//If a MonitorFn is used to set the state, then set the desiredState to: RSSTN_Run.
		//Returns the current operational state.
		//
		//If desiredState > RSSTN_Run, this instructs the Master not to got to
		//that state.  Once that state is reached, the Master node will treat the
		//desiredState as if it where desiredState = RSSTN_Run.  This continues
		//until the desiredState changes value.
		virtual uint8_t ExecuteUnitOfWork(uint8_t desiredState);
		
		//close out the state at shutdown.
		virtual void Shutdown();

		//For Master nodes, the user command state is typically corresponds
		//to the first state in the array of states (the reset state).
		//This method simply adds an offset to the user command state
		//to get the state name lined up with a DTX state name.
        static uint8_t UserCommandStateToDtxState(uint8_t userCmdState)
        {
            return userCmdState + (uint8_t)StdStateName_e::RSSTN_Reset;
        }

        static uint8_t DtxStateToUserCommandState(uint8_t dtxState)
        {
            uint8_t state = 0;
            if( dtxState >= (uint8_t)StdStateName_e::RSSTN_Reset )
            {
                state = dtxState - (uint8_t)StdStateName_e::RSSTN_Reset;
            }
            return state;
        }

    };

}
#endif /* MASTERSTATENODE_H_ */