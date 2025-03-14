/* ****************************************************************
 * Rabit Multi-Threaded Management System
 * Athrs: Randal Direen PhD
 *        Harry Direen PhD
 * www.direentech.com
 * Date: June 2016
 *
 *******************************************************************/

#include "TimerDelayStateNode.h"

namespace Rabit
{

    //Reset the State to the Starting Init State.
    uint8_t TimerDelayStateNode::Reset()
    {
        StateNode::Reset();
		_stopWatch.reset();
        _currentState = (uint8_t)StdStateName_e::RSSTN_Reset;
        if(UseParameterValues &&  IntParams[0] > 0)
        {
            MaxTimeSec = FloatParams[0];
        }
        if(UseParameterValues && IntParams[0] > 0)
        {
            ShowCompleteOnlyOnDesireStateComplete = true;
        }
        if(DestinationStates[0] != (uint8_t)StdStateName_e::RSSTN_NoChange)
        {
            NextState = DestinationStates[0];
        }
        return _currentState;
    }
	
	uint8_t TimerDelayStateNode::ExecuteUnitOfWork(uint8_t desiredState)
	{
		if(desiredState == (uint8_t)StdStateName_e::RSSTN_Reset)
		{
			_currentState = (uint8_t)StdStateName_e::RSSTN_Reset;
		}
		if(_currentState == (uint8_t)StdStateName_e::RSSTN_Reset)
		{
			Reset();
			_stopWatch.start();
			_currentState = (uint8_t)StdStateName_e::RSSTN_Run;
		}
		
		_stopWatch.captureTime();
		if(_stopWatch.getTimeElapsed() >= MaxTimeSec)
		{
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
