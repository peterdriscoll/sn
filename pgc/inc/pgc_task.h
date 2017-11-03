//  pgc_task.h: interface for the PGC_Base class.
//
//  Description:
//      An interface for a task, related to a transaction, that should be performed in the background.
//      The task will only be performed while the transaction exists.
//
//  Usage:
//      Inherit from this class.
//      Construct a class object and submit it.
//      Do not keep any pointers to the task.
//  Example:
//      #include "pgc_task.h"
//      class MyTask: PGC_Task
//      {
//          ...
//      }
//      ...
//         TopTransaction()->SubmitTask(new MyTask);
// 
//////////////////////////////////////////////////////////////////////

#if !defined(PGC_TASK_H_INCLUDED)
#define PGC_TASK_H_INCLUDED

#pragma once

#include "exp_ctrl_pgc.h"
#include "pgc_transaction.h"

namespace PGC
{
	class PGC_EXPORT PGC_Task
	{
	public:
		virtual void Run() = 0; // Perform acivities that can be gainfully done now, and return when complete. The method will be called periodically.
		virtual bool Complete() = 0; // Attempt to complete all outstanding processing. Return false if this cannot be reasonably done.
		virtual void Promote(PGC_Transaction *p_Transaction) = 0;
	};
}

#endif // !defined(PGC_TASK_H_INCLUDED)
