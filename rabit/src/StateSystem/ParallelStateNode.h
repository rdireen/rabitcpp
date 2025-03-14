/* ****************************************************************
 * DTX State System
 * Harry Direen PhD (hdireen@direentech.com)
 * www.direentech.com
 * Date: Nov. 2020
 *
 *******************************************************************/

#ifndef PARALLELSTATENODE_H_
#define PARALLELSTATENODE_H_

#include "StateNode.h"

namespace DTX_StateSystem
{
    //A State that supports runing a set of substates in parallel.
    //The output state of the parallel state will be one of:
    //    Run:  implies at least one substate is in its run state.
    //    Complete:  means that all substates have reached their complete state.
    //    Error:  means one or more substates is in an error state.  The other
    //            substates will continue to run when ExecuteUnitOfWork is called.
	class ParallelStateNode : public StateNode
	{
	public:
	    //Max number of substates if the Parallel's Array of States
	    //is used.  The user may supply an array of states larger than
	    //this value.
	    static const int MaxNumberOfSubStates = 4;
        //Master State provided Array of States.
        StateNode* ArrayOfStates[MaxNumberOfSubStates];

    protected:
		
			int NumberOfStates = 0;
			
			//Pointer to an array of states.  The number of
			//states is given by NumberOfStates.
			//Each state in the array will be run in the array order each
			//time the ExecuteUnitOfWork.

			StateNode** _arrayOfStatesPtr = ArrayOfStates;
			
			//A Monitor function/method can be used to monitor
			//system parameters and indicate which node's state the 
			//node should be in or change to.
			monitorFnPtr_t _MonitorFnPtr = nullptr;
			
			//Keep track of the last desired state, so that we know if and
			//when the input desired state changed.
			uint8_t _lastInputDesiredState = (uint8_t)StdStateName_e::RSSTN_Run;
            uint8_t _lastMontitorTargetState = (uint8_t)StdStateName_e::RSSTN_Run;
            uint8_t _targetState = (uint8_t)StdStateName_e::RSSTN_Run;

		public:

		ParallelStateNode();
		
		//Setup the Control State.
		virtual bool Setup( void *workspacePtr = nullptr,
							StateNode** arrayOfStatesPtr = nullptr, 
							int numOfStates = 0,
							monitorFnPtr_t monFnPtr = nullptr);
		
		//Reset the State to the Starting Init State.
		virtual uint8_t Reset();

        virtual uint8_t GetSubStateState(int substateID = -1);

        //Pass in the desired state of operation.  The basic desiredState
		//are:
		//RSSTN_Reset:  causes all the state node to go to reset, typically used
		//cause all the states to go back to their reset state and start over.
		//
		//RSSTN_Complete: passed to each state to indicate to each state that
		//it should go to the complete state.
		//
		//RSSTN_Run:  this should be the normal input to the parallel state.
		//it is passed to each substate.
		//
		//The Parallel state
		virtual uint8_t ExecuteUnitOfWork(uint8_t desiredState);
		
		//close out the state at shutdown.
		virtual void Shutdown();

    };

}
#endif /* ParallelStateNode_H_ */