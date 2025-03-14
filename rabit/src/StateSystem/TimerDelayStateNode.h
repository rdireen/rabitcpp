/* ****************************************************************
 * Rabit Multi-Threaded Management System
 * Athrs: Randal Direen PhD
 *        Harry Direen PhD
 * www.direentech.com
 * Date: June 2020
 *
 *******************************************************************/

#ifndef TIMERDELAYSTATENODE_H_
#define TIMERDELAYSTATENODE_H_

#include "StateNode.h"
#include "../Utilities/RabitStopWatch.h"

namespace Rabit
{
	class TimerDelayStateNode : public StateNode
	{
		private:
		RabitStopWatch _stopWatch;
		
		public:
		double MaxTimeSec = 0.0;
		
		bool ShowCompleteOnlyOnDesireStateComplete = false;
		
		TimerDelayStateNode()
		{
			Reset();
		}
		
		
		//Reset the State to the Starting Init State.
		virtual uint8_t Reset() final;

		virtual uint8_t ExecuteUnitOfWork(uint8_t desiredState) final;

	};

}
#endif /* TIMERDELAYSTATENODE_H_ */