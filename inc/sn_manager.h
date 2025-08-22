#if !defined(SN_MANAGER_H_INCLUDED)
#define SN_MANAGER_H_INCLUDED

#pragma once

#include "exp_ctrl_sn.h"

#include <string>
#include <ostream> // Add this include to resolve the 'std::ostream' identifier

namespace SNI
{
	class SNI_Manager;
}

namespace SN
{
	class SN_Error;

	typedef void OnErrorHandler(bool p_Err, const std::string &p_Description);

	enum LoggingLevel
	{
		ErrorLevel, WarningLevel, DebugLevel, InfoLevel, DetailLevel
	};

	enum DebuggingStop
	{
		NonStop, EndStop, TaskStop, ErrorStop, WarningStop, InfoStop, DebugStop, DetailStop
	};

	enum InterruptPoint
	{
		BreakPoint, CallPoint, ParameterPoint, ClonePoint, ErrorPoint, WarningPoint, EndPoint, MirrorPoint, FailPoint, StaticPoint, UserPoint, CodePoint, DelayedPoint
	};

	enum BreakId
	{
		LeftId, RightId, PartialCallId, CallId, ParameterOneId, ParameterTwoId, ParameterThreeId, ParameterFourId, ParameterFiveId, ValueId, ConditionId,  PositiveId, NegativeId, InfixId, EvaluateId, ReturnId, ErrorId, ExitId, CloneId, NoConstraintId, DelayId, StringId, UserId, MultipleOutputsId, DerivedOneId
	};

	std::string GetLoggingLevelCode(LoggingLevel p_LoggingLevel);

	class SN_EXPORT SN_Manager
	{
	public:
		SN_Manager();
		SN_Manager(SNI::SNI_Manager *p_Manager);
		SN_Manager(std::string p_Description, OnErrorHandler p_Handler = NULL, bool p_DelayOnEvaluate = false, size_t p_MaxCardinalityCall = 100, size_t p_MaxCardinalityUnify = 100);

		virtual ~SN_Manager();

		SN_Expression DelayedCalls();

		std::string Description();

		std::string LogFilePath();
		void SetLogFilePath(std::string p_LogFilePath);

		OnErrorHandler *ErrorHandler();
		void SetErrorHandler(OnErrorHandler *p_ErrorHandler);

		bool DelayOnEvaluate();
		void SetDelayOnEvaluate(bool p_DelayOnEvaluate);

		size_t MaxCardinalityCall();
		size_t MaxCardinalityUnify();
		void SetMaxCardinality(size_t p_MaxCardinalityCall, size_t p_MaxCardinalityUnify);

		size_t MaxStackFrames();
		void SetMaxStackFrames(size_t p_MaxStackFrame);

		void StartLogging(SN::LoggingLevel p_LoggingLevel, std::ostream *p_Stream = NULL);
		void StartDebugCommandLineServer(skynet::DebugAction p_DebugAction, int p_kbhit(), int p_GetCh());
		void StartWebServer(skynet::DebugAction p_DebugAction, const std::string& p_Address, const std::string& p_Port, const std::string& p_DocRoot, bool p_DoIt = true);

		bool HasConsole();
		bool KbHit();
		int  GetCh();

		size_t DebugFieldWidth();
		void SetDebugFieldWidth(size_t p_DebugFieldWidth);

		size_t DebugTitleWidth();
		void SetDebugTitleWidth(size_t p_DebugFieldWidth);

		bool AutoDefine();
		void SetAutoDefine(bool p_AutoDefine);

		// Processing options
		skynet::DirectPassType GetDirectPassType();
		void SetDirectPassType(skynet::DirectPassType p_DirectPassType);

		skynet::EvaluationType GetEvaluationType();
		void SetEvaluationType(skynet::EvaluationType p_EvaluationType);

		skynet::LogicType GetLogicType();
		void SetLogicType(skynet::LogicType p_LogicType);
		//	This flag allows
		//		(right unbounded StringRef == std::string) == unknown boolean.
		//	If enabled, SNI_StringRef::DoEquals will implement this, leading 
		//	to allsorts of complications. It creates valuesets for the start/end 
		//	positions, which turns the stringref into a multi value object, 
		//	effectively, a ValueSet. SimplifyValue then explicitly changes it into
		//	a value set.
		//  This may not be necessary. Changes to Collapse may achieve the same result.
		//	I'll try to get it working with the flag defined or not defined.

		bool TailCallOptimization() const;
		void SetTailCallOptimization(bool p_TailCallOptimization);

		PGC::PromotionStrategy MemoryPromotionStrategy() const;
		void SetMemoryPromotionStrategy(PGC::PromotionStrategy p_MemoryPromotionStrategy);

		bool AutoExpandNull() const;
		void SetAutoExpandNull(bool p_AutoExpandNull);

		// Debugging
		void Breakpoint();
		void SetDebugAction(enum skynet::DebugAction p_DebugAction);

		SNI::SNI_Manager *GetSNI_Manager();
	private:
		SNI::SNI_Manager *m_Manager;
		bool m_MyManager;
	};
}

#endif // !defined(SN_MANAGER_H_INCLUDED)