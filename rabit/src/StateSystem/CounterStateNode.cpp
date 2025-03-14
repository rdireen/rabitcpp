/* ****************************************************************
 * Rabit Multi-Threaded Management System
 * Athrs: Randal Direen PhD
 *        Harry Direen PhD
 * www.direentech.com
 * Date: June 2020
 *
 *******************************************************************/

#include "CounterStateNode.h"

namespace Rabit
{

    //Reset the State to the Starting Init State.
    uint8_t CounterStateNode::Reset()
    {
        StateNode::Reset();
        _counter = 0;
        _currentState = (uint8_t)StdStateName_e::RSSTN_Reset;
        if(UseParameterValues && IntParams[0] > 0)
        {
            MaxCount = (uint32_t)IntParams[0];
        }
        if(UseParameterValues && IntParams[1] > 0)
        {
            ShowCompleteOnlyOnDesireStateComplete = true;
        }
        if(DestinationStates[0] != (uint8_t)StdStateName_e::RSSTN_NoChange)
        {
            NextState = DestinationStates[0];
        }
        return _currentState;
    }



    uint8_t CounterStateNode::ExecuteUnitOfWork(uint8_t desiredState)
	{
		if(desiredState == (uint8_t)StdStateName_e::RSSTN_Reset)
		{
			_currentState = (uint8_t)StdStateName_e::RSSTN_Reset;
		}
		if(_currentState == (uint8_t)StdStateName_e::RSSTN_Reset)
		{
			Reset();
			_currentState = (uint8_t)StdStateName_e::RSSTN_Run;
		}
		
		if(++_counter >= MaxCount)
		{
			_counter = MaxCount;
			if(ShowCompleteOnlyOnDesireStateComplete )
			{
				_currentState = desiredState == (uint8_t)StdStateName_e::RSSTN_Complete ?
				    (uint8_t)StdStateName_e::RSSTN_Complete : (uint8_t)StdStateName_e::RSSTN_Run;
			}
			else
			{
				_currentState = (uint8_t)StdStateName_e::RSSTN_Complete;
			}
		}
		else
		{
			_currentState = (uint8_t)StdStateName_e::RSSTN_Run;
		}
		return _currentState;
	}
		
}
