/* ****************************************************************
 * Rabit Multi-Threaded Management System
 * Athrs: Randal Direen PhD
 *        Harry Direen PhD
 * www.direentech.com
 * Date: June 2020
 *
 *******************************************************************/

#ifndef STATENODE_H_
#define STATENODE_H_

#include <stdint.h>
#include <stdbool.h>
#include <memory>
#include "../Manager/RabitManager.h"
#include "../Utilities/SystemTimeClock.h"

#define DTXSTATE_MAXNUMBER_INT_PARAMS 4
#define DTXSTATE_MAXNUMBER_FLOAT_PARAMS 4
#define DTXSTATE_MAXNUMBER_DESTINATION_STATES 3


namespace Rabit
{

	//A State can be composed of any number of substates.
	//This defines the primary states that each state node 
	//should support.
	enum StdStateName_e
	{
		RSSTN_NoChange,
		RSSTN_Error,
		//When requested, each state node should go to the Complete State.
		//When going to the complete state, anything that needs to be cleaned
		//up or shutdown should be accomplished before reaching the Complete State.
		RSSTN_Complete,
		//Each time a State Node is activated or started it should
		//start in the reset state.  In this state anything that needs
		//to be setup should be setup for normal state operation.
		RSSTN_Reset,
		//The Run state is the normal state of operation and is the
		//state most nodes will/should go to after Reset.
		RSSTN_Run,
		RSSTN_FirstUserState		
	};
	
	typedef uint8_t (*monitorFnPtr_t)(RabitManager* mgrPtr, void *workspacePtr, uint8_t currentState);
	
	//An Abstract Control State Node
	class StateNode
	{
		protected:
			uint8_t _currentState = (uint8_t)StdStateName_e::RSSTN_Reset;
			
			//The Rabit Manager Pointer allows a state
			//to gain access to the Manager's resources.
			//The state nodes will have to
			//cast the manager to the to the concrete manager.
			RabitManager* _rmgrPtr = nullptr;
			
			//A workspace object that contains variables and objects
			//required by the state nodes.  The state nodes will have to 
			//cast the workspace to the concrete object. 
			void *_workspacePtr = nullptr;			
		
		public:
        //The Next & Last State is for the user's purpose.
        uint8_t NextState = (uint8_t)StdStateName_e::RSSTN_NoChange;

        //If this state goes to the Error state, it may
        //suggest the next state the parent state should go to.
        uint8_t NextStateOnError = (uint8_t)StdStateName_e::RSSTN_Error;

        //Last state this state was in.
        uint8_t LastState = (uint8_t)StdStateName_e::RSSTN_Reset;

        //A state's parameters may be set by the generic IntParams
        //and FloatParams (UseParameterValues = true), or by their
        //internal or unique values (UseParameterValues = false);
        bool UseParameterValues = false;

        //User Defined Integer Parameters
        int IntParams[DTXSTATE_MAXNUMBER_INT_PARAMS];

        //User Defined Integer Parameters
        double FloatParams[DTXSTATE_MAXNUMBER_FLOAT_PARAMS];

        //When this state reaches the complete state, it then
        //passes a desired NextState chose from one of these states
        //to the parent calling state.
        uint8_t DestinationStates[DTXSTATE_MAXNUMBER_DESTINATION_STATES];

        StateNode()
		{
            Setup();
		};

        uint8_t GetState()
        {
            return _currentState;

        }

        //Some states such as the master and serial states
        //have active substates.  This method allows getting
        //the active substate's state which may be used for
        //monitoring purposes.
        //The substate ID can be used to get a specific state's
        //state.  If < 0, the active sub-state's state will be returned.
        virtual uint8_t GetSubStateState(int substateID = -1)
        {
            //If there is not meaningful substate, simply return
            //a valid default.
            return (uint8_t)StdStateName_e::RSSTN_Run;
        }

        void SetWorkspace(void *workspacePtr)
        {
            _workspacePtr = workspacePtr;
        }

        void SetManagerPtr(RabitManager* mgrPtr)
        {
            _rmgrPtr = mgrPtr;
        }

		//Setup the Control State.  
		virtual bool Setup(RabitManager* rmgrPtr = nullptr, 
							void *workspacePtr = nullptr,
							StateNode** _arrayOfStatesPtr = nullptr, 
							int numOfStates = 0,
							monitorFnPtr_t monFnPtr = nullptr)
		{
			_rmgrPtr = rmgrPtr;
			_workspacePtr = workspacePtr;
			_currentState = StdStateName_e::RSSTN_Reset;
			NextState = StdStateName_e::RSSTN_NoChange;
			LastState = StdStateName_e::RSSTN_Reset;
            NextStateOnError = StdStateName_e::RSSTN_Error;
            for(int i = 0; i < DTXSTATE_MAXNUMBER_INT_PARAMS; i++)
                IntParams[i] = 0;
            for(int i = 0; i < DTXSTATE_MAXNUMBER_FLOAT_PARAMS; i++)
                FloatParams[i] = 0.0f;
            for(int i = 0; i < DTXSTATE_MAXNUMBER_DESTINATION_STATES; i++)
                DestinationStates[i] = (uint8_t)StdStateName_e::RSSTN_NoChange;

			return false;
		}
		
		//Reset the State to the Starting Init State.	
		virtual uint8_t Reset() 
		{
            if(DestinationStates[0] != (uint8_t)StdStateName_e::RSSTN_NoChange)
            {
                NextState = DestinationStates[0];
            }
			_currentState = (uint8_t)StdStateName_e::RSSTN_Reset;
			return _currentState;
		}
		
		//A desired state may be passed in.  The User decides how to 
		//use this state.  Typically it will be RSSTN_Run or RSSTN_Complete.
		//The basic desiredState are:
		//RSSTN_Reset:  causes the state node to go to reset, typically used
		//on the first call to state node to cause it to go through its reset process.
		//RSSTN_Complete: Causes the state to go to the complete state... it could
		//take several calls to ExecuteUnitOfWork before the state node reaches the 
		//reset state.
		//RSSTN_Run: normal input to cause the state node to operate normally.
		//Returns the current operational state.
		virtual uint8_t ExecuteUnitOfWork(uint8_t desiredState) = 0; 
		
		//close out the state at shutdown.
		virtual void Shutdown() {}
		
	};
}

#endif /* STATENODE_H_ */