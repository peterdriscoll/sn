#pragma once

#include "sni_object.h"
#include <vector>

// SNI_User is a class that represents an individual users attachment via the debugger interface to SN, possibly implemented as a session id.
// It holds a main thread and 0..many worker threads.
// The class also holds the SNI_DelayedProcessor.
namespace SNI
{
	class SNI_DelayedProcessor;
	class SNI_Thread;

	class SNI_User : public SNI_Object
	{
		PGC_CLASS(SNI_User)

	private:
		static void UserThreadFunc(SNI_User* p_User);
	public:
		SNI_User();

	private:
		SNI_DelayedProcessor* m_Processor;
		SNI_Thread* m_MainThread;
		std::vector<SNI_Thread*> m_WorkerList;
	};
}
