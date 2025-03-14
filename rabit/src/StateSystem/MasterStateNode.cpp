/* ****************************************************************
 * Rabit Multi-Threaded Management System
 * Athrs: Randal Direen PhD
 *        Harry Direen PhD
 * www.direentech.com
 * Date: June 2020
 *
 *******************************************************************/

#include "MasterStateNode.h"

namespace Rabit
{

		MasterStateNode::MasterStateNode()
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
		bool MasterStateNode::Setup(RabitManager* rmgrPtr,
								void *workspacePtr,
								StateNode** arrayOfStatesPtr,
								int numOfStates,
								monitorFnPtr_t monFnPtr )
		{
			bool error = false;
            _workspacePtr = workspacePtr;
			StateNode::Setup(rmgrPtr, workspacePtr);
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
			if(_arrayOfStatesPtr != nullptr &&  NumberOfStates >= 2)
			{
				for(int i = 0; i < NumberOfStates; i++)
				{
					if(_arrayOfStatesPtr[i] != nullptr)
					{
                        _arrayOfStatesPtr[i]->SetManagerPtr(rmgrPtr);
                        _arrayOfStatesPtr[i]->SetWorkspace(workspacePtr);
					}
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
		
		//Reset the State to the Starting Reset State.
		uint8_t MasterStateNode::Reset()
		{
			StateNode::Reset();
            _lastInputDesiredState = (uint8_t)StdStateName_e::RSSTN_Run;
            _lastMontitorTargetState = (uint8_t)StdStateName_e::RSSTN_Run;
            _targetState = (uint8_t)StdStateName_e::RSSTN_Run;
			if(_arrayOfStatesPtr != nullptr &&  NumberOfStates >= 2)
			{
				_currentState = (uint8_t)StdStateName_e::RSSTN_Reset;
				if(_arrayOfStatesPtr[0] != nullptr)
				{
					_arrayOfStatesPtr[0]->Reset();
				}
				else
				{
					_currentState = StdStateName_e::RSSTN_Run;
				}
			}
			else
			{
				_currentState = StdStateName_e::RSSTN_Error;
			}
			return _currentState;
		}

        uint8_t MasterStateNode::GetSubStateState(int substateID)
        {
		    uint8_t state = _currentState;
		    if(substateID >= 0 && substateID < NumberOfStates)
            {
                state = _arrayOfStatesPtr[substateID]->GetState();
            }
		    else if( _currentState >= (uint8_t)StdStateName_e::RSSTN_Reset)
            {
                substateID = DtxStateToUserCommandState(_currentState);
                if(substateID < NumberOfStates)
                {
                    state = _arrayOfStatesPtr[substateID]->GetState();
                }
            }
		    return state;
        }
		
		//Returns the current operational state.
		uint8_t MasterStateNode::ExecuteUnitOfWork(uint8_t desiredState)
		{
			uint8_t stateFnIdx = 0;
			uint8_t fnState, nstate, estate;
			uint8_t ctrlDesiredState = (uint8_t)StdStateName_e::RSSTN_Run;

			bool stateChanged = false;
			if( _arrayOfStatesPtr == nullptr || NumberOfStates < 2)
			{
				_currentState = (uint8_t)StdStateName_e::RSSTN_Error;
				return _currentState;
			}

            uint8_t monTargetState = (uint8_t)StdStateName_e::RSSTN_Run;
			if(_MonitorFnPtr != nullptr)
			{
				monTargetState = _MonitorFnPtr(_rmgrPtr, _workspacePtr, _currentState);
			}

			if(desiredState != _lastInputDesiredState)
            {
			    //This is higher priority than the monTargetState.
                _targetState = desiredState;
                _lastInputDesiredState = desiredState;
            }
			else if(monTargetState != _lastMontitorTargetState)
            {
                _targetState = monTargetState;
                _lastMontitorTargetState = monTargetState;
            }

			//A change in state allows a second pass;
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
                            _currentState = (uint8_t)StdStateName_e::RSSTN_Reset;
                            if (_arrayOfStatesPtr[0] != nullptr)
                            {
                                fnState = _arrayOfStatesPtr[0]->Reset();
                            }
                            stateChanged = true;
                        }
					    break;
				
					case (uint8_t)StdStateName_e::RSSTN_Complete:
                        if(_targetState == (uint8_t)StdStateName_e::RSSTN_Reset)
                        {
                            _currentState = (uint8_t)StdStateName_e::RSSTN_Reset;
                            if (_arrayOfStatesPtr[0] != nullptr)
                            {
                                fnState = _arrayOfStatesPtr[0]->Reset();
                            }
                            stateChanged = true;
                        }
					    break;
				

                    case (uint8_t)StdStateName_e::RSSTN_Reset:
                        //Fall through to default.

                    case (uint8_t)StdStateName_e::RSSTN_Run:
						//Fall through to default.
				
					default:
						stateFnIdx = _currentState - (uint8_t)StdStateName_e::RSSTN_Reset;
						if(stateFnIdx < NumberOfStates
						    && _arrayOfStatesPtr[stateFnIdx] != nullptr )
						{
							ctrlDesiredState = (uint8_t)StdStateName_e::RSSTN_Run;
							if( (_currentState != _targetState)
							    && (_arrayOfStatesPtr[stateFnIdx]->GetState() != (uint8_t)StdStateName_e::RSSTN_Reset))
							{
							    //Tell the sub-state to go to the complete state.
								ctrlDesiredState = (uint8_t)StdStateName_e::RSSTN_Complete;
							}
							
							LastState = _currentState;
							fnState = _arrayOfStatesPtr[stateFnIdx]->ExecuteUnitOfWork(ctrlDesiredState);
							if(fnState == (uint8_t)StdStateName_e::RSSTN_Complete)
							{
								LastState = _currentState;
                                nstate = _arrayOfStatesPtr[stateFnIdx]->NextState;
                                if( _currentState != _targetState)
                                {
                                    //The target is the highest priority.
                                    _currentState = _targetState;
                                }
								else if( nstate != (uint32_t)StdStateName_e::RSSTN_NoChange )
                                {
                                    _targetState = nstate;
                                    _currentState = nstate;
                                }
								else
                                {
								    //Default after state completes.
                                    _currentState = (uint32_t)StdStateName_e::RSSTN_Run;
                                    _targetState = _currentState;
                                }
								if(_currentState >= (uint8_t)StdStateName_e::RSSTN_Reset)
                                {
                                    stateFnIdx = _currentState - (uint8_t) StdStateName_e::RSSTN_Reset;
                                    if(stateFnIdx < NumberOfStates)
                                    {
                                        //Get the next state ready for running by first
                                        //calling its reset method.
                                        if (_arrayOfStatesPtr[stateFnIdx] != nullptr)
                                        {
                                            fnState = _arrayOfStatesPtr[stateFnIdx]->Reset();
                                        }
                                    }
                                }
                                stateChanged = true;
							}
							else if(fnState == (uint8_t)StdStateName_e::RSSTN_Error)
							{
                                LastState = _currentState;
                                _currentState = (uint8_t)StdStateName_e::RSSTN_Error;
                                estate = _arrayOfStatesPtr[stateFnIdx]->NextStateOnError;
                                if( estate != (uint32_t)StdStateName_e::RSSTN_NoChange)
                                {
                                    _targetState = estate;
                                    _currentState = estate;
                                }
                                if(_currentState >= (uint8_t)StdStateName_e::RSSTN_Reset)
                                {
                                    stateFnIdx = _currentState - (uint8_t) StdStateName_e::RSSTN_Reset;
                                    if(stateFnIdx < NumberOfStates)
                                    {
                                        //Get the next state ready for running by first
                                        //calling its reset method.
                                        if (_arrayOfStatesPtr[stateFnIdx] != nullptr)
                                        {
                                            fnState = _arrayOfStatesPtr[stateFnIdx]->Reset();
                                        }
                                    }
                                }
                                stateChanged = true;
							}
						}
						else
						{
							//We should never reach this state.
							//Force Reset state
							_currentState = (uint8_t)StdStateName_e::RSSTN_Error;
                            _targetState = _currentState;
						}			
				}
			} while (stateChanged);
			
			return _currentState;
		}
		
		//close out the state at shutdown.
		void MasterStateNode::Shutdown()
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
