#if !defined(SNI_MANAGER_H_INCLUDED)
#define SNI_MANAGER_H_INCLUDED

#pragma once

#include <string>
using namespace std;

namespace SN
{
	class SN_Error;
}

namespace SNI
{
	typedef void OnErrorHandler(SN::SN_Error p_Result);

	class SNI_Manager
	{
	public:
		static SNI_Manager *GetTopManager();

		SNI_Manager();
		SNI_Manager(OnErrorHandler *p_ErrorHandler, bool p_DelayOnEvaluate, size_t p_MaxCardinalityCall, size_t p_MaxCardinalityUnify);

		virtual ~SNI_Manager();

		string LogFilePath();
		void SetLogFilePath(string p_LogFilePath);

		OnErrorHandler *ErrorHandler();
		void SetErrorHandler(OnErrorHandler * p_ErrorHandler);

		size_t MaxCardinalityCall();
		size_t MaxCardinalityUnify();
		void SetMaxCardinality(size_t p_MaxCardinalityCall, size_t p_MaxCardinalityUnify);

		bool DelayOnEvaluate();
		void SetDelayOnEvaluate(bool p_DelayOnEvaluate);

		void StartLogging(SN::LoggingLevel p_LoggingLevel, ostream *p_Stream);
		void StartDebug(SN::DebugAction p_DebugAction, int p_Kbhit(), int p_GetCh());

		bool HasConsole();
		bool KbHit();
		int  GetCh();

		bool IsBreakPoint(SN::InterruptPoint p_InterruptPoint, long p_ThreadNum, long p_FrameStackDepth);
		void DebugCommand(SN::InterruptPoint p_InterruptPoint, const string & p_Text);

		size_t DebugFieldWidth();
		void SetDebugFieldWidth(size_t p_DebugFieldWidth);

		size_t DebugTitleWidth();
		void SetDebugTitleWidth(size_t p_DebugFieldWidth);

		ostream * CreateLogFile(SN::LoggingLevel);

	private:
		static SNI_Manager *m_TopManager;
		SNI_Manager *m_LastManager;

		string m_LogFilePath;

		OnErrorHandler *m_ErrorHandler;

		void ProcessCommand(const string & p_Text, long l_ThreadNum, long l_FrameStackDepth);

		size_t GetThreadNum();
		void WriteStepCounts(long l_ThreadNum);
		size_t m_MaxCardinalityCall;
		size_t m_MaxCardinalityUnify;

		bool m_DelayOnEvaluate;

		long m_DebugFieldWidth;
		long m_DebugTitleWidth;

		bool m_HasConsole;
		int (*m_KbHit)();
		int (*m_GetCh)();
		static long m_Depth;
		static enum SN::DebugAction m_DebugAction;
		static long m_ThreadNum;
		static long m_FrameStackDepth;
		static vector<long> m_ThreadStepCountList;
		static long m_GotoThreadNum;
		static long m_StepCount;
	};
}

#endif // !defined(SNI_MANAGER_H_INCLUDED)
