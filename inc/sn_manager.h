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

	enum LoggingLevel
	{
		ErrorLevel, WarningLevel, DebugLevel, InfoLevel, DetailLevel
	};

	enum DebugAction
	{
		None, Run, RunToEnd, Debug, CodeBreak, StepOver, StepInto, StepParameter, StepOut, GotoStepCount, Quit, Abort
	};

	enum InterruptPoint
	{
		BreakPoint, CallPoint, ParameterPoint, ClonePoint, ErrorPoint, WarningPoint, EndPoint, MirrorPoint, FailPoint, StaticPoint
	};

	enum BreakId
	{
		LeftId, RightId, PartialCallId, CallId, ParameterOneId, ParameterTwoId, ParameterThreeId, ParameterFourId, ParameterFiveId, ValueId, ConditionId,  PositiveId, NegativeId, InfixId, EvaluateId, ReturnId, ErrorId, ExitId, CloneId, NoConstraintId, DelayId, StringId
	};

	string GetLoggingLevelCode(LoggingLevel p_LoggingLevel);

	class SN_EXPORT SN_Manager
	{
	public:
		static void ThrowErrorHandler(SN::SN_Error p_Result);

		static void LogicSetup();

		SN_Manager();
		SN_Manager(SNI::SNI_Manager *p_Manager);
		SN_Manager(string p_Description, OnErrorHandler p_Handler = NULL, bool p_DelayOnEvaluate = false, size_t p_MaxCardinalityCall = 100, size_t p_MaxCardinalityUnify = 100);

		virtual ~SN_Manager();

		string Description();

		string LogFilePath();
		void SetLogFilePath(string p_LogFilePath);

		OnErrorHandler *ErrorHandler();
		void SetErrorHandler(OnErrorHandler *p_ErrorHandler);

		bool DelayOnEvaluate();
		void SetDelayOnEvaluate(bool p_DelayOnEvaluate);

		size_t MaxCardinalityCall();
		size_t MaxCardinalityUnify();
		void SetMaxCardinality(size_t p_MaxCardinalityCall, size_t p_MaxCardinalityUnify);

		size_t MaxStackFrames();
		void SetMaxStackFrames(size_t p_MaxStackFrame);

		void StartLogging(SN::LoggingLevel p_LoggingLevel, ostream *p_Stream = NULL);
		void StartDebugCommandLineServer(DebugAction p_DebugAction, int p_kbhit(), int p_GetCh());
		void StartWebServer(SN::DebugAction p_DebugAction, const string& p_Address, const string& p_Port, const string& p_DocRoot, bool p_DoIt = true);

		bool HasConsole();
		bool KbHit();
		int  GetCh();

		size_t DebugFieldWidth();
		void SetDebugFieldWidth(size_t p_DebugFieldWidth);

		size_t DebugTitleWidth();
		void SetDebugTitleWidth(size_t p_DebugFieldWidth);

		SNI::SNI_Manager *GetSNI_Manager();
	private:
		SNI::SNI_Manager *m_Manager;
		bool m_MyManager;
	};
}

#endif // !defined(SN_MANAGER_H_INCLUDED)