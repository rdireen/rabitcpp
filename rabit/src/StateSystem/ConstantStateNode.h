/* ****************************************************************
 * Rabit Multi-Threaded Management System
 * Athrs: Randal Direen PhD
 *        Harry Direen PhD
 * www.direentech.com
 * Date: June 2020
 *
 *******************************************************************/

#ifndef CONSTANTSTATENODE_H_
#define CONSTANTSTATENODE_H_

#include "StateNode.h"

namespace Rabit
{
	//Do-Nothing State
	class ConstantStateNode : public StateNode
	{
	public:
	
		ConstantStateNode() {}

		virtual uint8_t ExecuteUnitOfWork(uint8_t desiredState)
		{
			_currentState = desiredState;
			return _currentState;
		}
	};
	
}
#endif /* CONSTANTSTATENODE_H_ */