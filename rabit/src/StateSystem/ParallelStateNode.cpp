/* ****************************************************************
 * DTX State System
 * Harry Direen PhD (hdireen@direentech.com)
 * www.direentech.com
 * Date: Nov. 2020
 *
 *******************************************************************/

#include "ParallelStateNode.h"

namespace DTX_StateSystem
{

		ParallelStateNode::ParallelStateNode()
		{
            for(int i = 0; i < MaxNumberOfSubStates; i++)
            {
                ArrayOfStates[i] = nullptr;
            }
            NumberOfStates = 0;
            _arrayOfStatesPtr = ArrayOfStates;
            _MonitorFnPtr = nullptr;
		}
		
		//Setup the Control State.
		//If the arrayOfStatesPtr is null, the user must
		//provide the states by filling in the ArrayOfStates.
		//This must be done before calling this method.
		bool ParallelStateNode::Setup(void *workspacePtr,
								StateNode** arrayOfStatesPtr,
								int numOfStates,
								monitorFnPtr_t monFnPtr )
		{
			bool error = false;
            _workspacePtr = workspacePtr;
			StateNode::Setup(workspacePtr);
			if(arrayOfStatesPtr == nullptr)
            {
                _arrayOfStatesPtr = ArrayOfStates;
                numOfStates = numOfStates > MaxNumberOfSubStates ? MaxNumberOfSubStates : numOfStates;
            }
			else
            {
                _arrayOfStatesPtr = arrayOfStatesPtr;

            }
			//Verify the number of states
            for(int i = 0; i < numOfStates; i++)
            {
                if(_arrayOfStatesPtr[i] == nullptr)
                {
                    numOfStates = i;
                    break;
                }
            }
            NumberOfStates = numOfStates;
			_MonitorFnPtr = monFnPtr;
			//Setup each of the Array of States
			if(_arrayOfStatesPtr != nullptr &&  NumberOfStates > 0)
			{
				for(int i = 0; i < NumberOfStates; i++)
				{
					_arrayOfStatesPtr[i]->SetWorkspace(workspacePtr);
				}
				error = false;
			}
			else
			{
				_currentState = StdStateName_e::RSSTN_Error;
				error = true;
			}
			return error;
		}
		
		//Reset the State and all substates to the Starting Reset State.
		uint8_t ParallelStateNode::Reset()
		{
			StateNode::Reset();
            _lastInputDesiredState = (uint8_t)StdStateName_e::RSSTN_Run;
            _lastMontitorTargetState = (uint8_t)StdStateName_e::RSSTN_Run;
            _targetState = (uint8_t)StdStateName_e::RSSTN_Run;
            _currentState = (uint8_t)StdStateName_e::RSSTN_Reset;
			return _currentState;
		}

        uint8_t ParallelStateNode::GetSubStateState(int substateID)
        {
            uint8_t state = _currentState;
            if(substateID >= 0 && substateID < NumberOfStates)
            {
                state = _arrayOfStatesPtr[substateID]->GetState();
            }
            return state;
        }

    //Returns the current operational state.
		uint8_t ParallelStateNode::ExecuteUnitOfWork(uint8_t desiredState)
		{
			uint8_t fnState;
			bool stateChanged;
			bool allComplete;
            bool anyError;

			if( _arrayOfStatesPtr == nullptr || NumberOfStates < 1)
			{
				_currentState = (uint8_t)StdStateName_e::RSSTN_Error;
				return _currentState;
			}

            uint8_t monTargetState = (uint8_t)StdStateName_e::RSSTN_Run;
			if(_MonitorFnPtr != nullptr)
			{
                monTargetState = _MonitorFnPtr(_rmgrPtr, _workspacePtr, _currentState);
			}

            stateChanged = false;
			if(desiredState != _lastInputDesiredState)
            {
			    //This is higher priority than the monTargetState.
                _targetState = desiredState;
                _lastInputDesiredState = desiredState;
                stateChanged = true;
            }
			else if(monTargetState != _lastMontitorTargetState)
            {
                _targetState = monTargetState;
                _lastMontitorTargetState = monTargetState;
                stateChanged = true;
            }
			if( stateChanged )
            {
			    //Qualify requested state change.
			    if(_targetState == (uint8_t)StdStateName_e::RSSTN_Reset)
                {
			        //Force the substates to their reset state so
			        //they will start over.
                    _currentState = (uint8_t)StdStateName_e::RSSTN_Reset;
                    _targetState = (uint8_t)StdStateName_e::RSSTN_Run;
                }
			    else if(_targetState != (uint8_t)StdStateName_e::RSSTN_Complete )
                {
                    _targetState = (uint8_t)StdStateName_e::RSSTN_Run;
                }
            }

			do
			{
				stateChanged = false;
				switch(_currentState)
				{
                    case (uint8_t)StdStateName_e::RSSTN_NoChange:
                        //Go to the Run State;
                        _currentState = (uint8_t)StdStateName_e::RSSTN_Run;
                        _targetState = _currentState;
                        stateChanged = true;
                        break;

					case (uint8_t)StdStateName_e::RSSTN_Error:
						if(_targetState == (uint8_t)StdStateName_e::RSSTN_Reset)
                        {
                            _currentState = _targetState;
                            stateChanged = true;
                        }
						else
                        {
						    //Change to the run state and let all the sub-states
						    //run even if they are in their error state.
						    //We will end up back here and they might have some
						    //means to clear their error.
                            _currentState = (uint8_t)StdStateName_e::RSSTN_Run;
                            stateChanged = true;
                        }
					    break;

					case (uint8_t)StdStateName_e::RSSTN_Complete:
                        if(_targetState == (uint8_t)StdStateName_e::RSSTN_Reset)
                        {
                            _currentState = _targetState;
                            stateChanged = true;
                        }
					    break;


                    case (uint8_t)StdStateName_e::RSSTN_Reset:
                    {
                        //Force all substates to their reset state.
                        for (int i = 0; i < NumberOfStates; i++)
                        {
                            _arrayOfStatesPtr[i]->Reset();
                        }
                        _currentState = (uint8_t)StdStateName_e::RSSTN_Run;
                        _targetState = _currentState;
                        stateChanged = true;
                        break;
                    }
                    case (uint8_t)StdStateName_e::RSSTN_Run:
						//Fall through to default.

					default:
                    {
                        //Run each state.
                        for (int i = 0; i < NumberOfStates; i++)
                        {
                            _arrayOfStatesPtr[i]->ExecuteUnitOfWork(_targetState);
                        }

                        //Check output state condition.
                        allComplete = true;
                        anyError = false;
                        for (int i = 0; i < NumberOfStates; i++)
                        {
                            fnState = _arrayOfStatesPtr[i]->GetState();
                            allComplete &= fnState == (uint8_t) StdStateName_e::RSSTN_Complete;
                            anyError |= fnState == (uint8_t) StdStateName_e::RSSTN_Error;
                        }
                        if (anyError)
                        {
                            _currentState = (uint8_t) StdStateName_e::RSSTN_Error;
                        }
                        else if (allComplete)
                        {
                            _currentState = (uint8_t) StdStateName_e::RSSTN_Complete;
                        }
                        break;
                    }
				}
			} while (stateChanged);

			return _currentState;
		}
		
		//close out the state at shutdown.
		void ParallelStateNode::Shutdown()
		{
			//Setup each of the Array of States
			if(_arrayOfStatesPtr != nullptr)
			{
				for(int i = 0; i < NumberOfStates; i++)
				{
					if(_arrayOfStatesPtr[i] != nullptr)
					{
						_arrayOfStatesPtr[i]->Shutdown();
					}
				}
			}
			_currentState = StdStateName_e::RSSTN_Complete;
		}


}
