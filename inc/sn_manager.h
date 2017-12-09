#if !defined(SN_MANAGER_H_INCLUDED)
#define SN_MANAGER_H_INCLUDED

#pragma once

#include "exp_ctrl_sn.h"

#include <string>
using namespace std;

namespace SNI
{
	class SNI_Manager;
}

namespace SN
{
	class SN_Error;

	typedef void OnErrorHandler(SN_Error p_Result);

	enum DebugAction
	{
		None, Run, RunToEnd, StepOver, StepInto, StepParameter, StepOut
	};

	enum InterruptPoint
	{
		BreakPoint, CallPoint, ParameterPoint, ErrorPoint
	};

	class SN_EXPORT SN_Manager
	{
	public:
		static SN_Manager GetTopManager();

		SN_Manager();
		SN_Manager(SNI::SNI_Manager *p_Manager);
		SN_Manager(OnErrorHandler p_Handler, bool p_DelayOnEvaluate = false, size_t p_MaxCardinalityCall = 100, size_t p_MaxCardinalityUnify = 100);

		virtual ~SN_Manager();

		string LogFilePath();
		void SetLogFilePath(string p_LogFilePath);

		OnErrorHandler *ErrorHandler();
		void SetErrorHandler(OnErrorHandler *p_ErrorHandler);

		bool DelayOnEvaluate();
		void SetDelayOnEvaluate(bool p_DelayOnEvaluate);

		size_t MaxCardinalityCall();
		size_t MaxCardinalityUnify();
		void SetMaxCardinality(size_t p_MaxCardinalityCall, size_t p_MaxCardinalityUnify);

		void StartDebug(DebugAction p_DebugAction, int p_kbhit(), int p_GetCh());
		bool HasConsole();
		bool KbHit();
		int  GetCh();
		void DebugCommand(InterruptPoint p_InterruptPoint);

	private:
		SNI::SNI_Manager *m_Manager;
		bool m_MyManager;
	};
}

#endif // !defined(SN_MANAGER_H_INCLUDED)