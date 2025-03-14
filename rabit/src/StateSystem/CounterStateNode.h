/* ****************************************************************
 * Rabit Multi-Threaded Management System
 * Athrs: Randal Direen PhD
 *        Harry Direen PhD
 * www.direentech.com
 * Date: June 2020
 *
 *******************************************************************/

#ifndef COUNTERSTATENODE_H_
#define COUNTERSTATENODE_H_

#include "StateNode.h"

namespace Rabit
{
	class CounterStateNode : public StateNode
	{
	private:
		uint32_t _counter = 0;
	
	public:
		uint32_t MaxCount = 0;
		
		bool ShowCompleteOnlyOnDesireStateComplete = false;
		
		CounterStateNode() 
		{
			Reset();
		}

		//Reset the State to the Starting Init State.
		virtual uint8_t Reset() final;

		virtual uint8_t ExecuteUnitOfWork(uint8_t desiredState) final;

	};

}
#endif /* COUNTERSTATENODE_H_ */