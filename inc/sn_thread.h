#if !defined(SN_THREAD_H_INCLUDED)
#define SN_THREAD_H_INCLUDED

#pragma once

#include "exp_ctrl_sn.h"

#include <string>


namespace SNI
{
	class SNI_Thread;
	class SNI_Manager;
}

namespace SN
{
	class SN_EXPORT SN_Thread
	{
	public:
		SN_Thread();

		virtual ~SN_Thread();

		bool HasTopManager();

		SNI::SNI_Thread *GetSNI_Thread();
	private:
		SNI::SNI_Thread *m_Thread;
	};
}

#endif // !defined(SN_THREAD_H_INCLUDED)