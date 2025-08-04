#if !defined(SNI_MANAGER_H_INCLUDED)
#define SNI_MANAGER_H_INCLUDED

#pragma once

#include <string>
#include <vector>
using namespace std;

#include "sni_debugcommand.h"
#include "ihttp_server.h"

namespace SN
{
	class SN_Error;
}

namespace HTTP
{
	namespace server
	{
		class server;
	}
}

namespace SNI
{
	typedef void OnErrorHandler(bool p_Err, const string& p_Description);

	class SNI_Frame;
	typedef vector<SNI_Frame *> SNI_FrameList;

	class SNI_Manager
	{
	public:
		static void ThrowErrorHandler(bool p_Err, const string& p_Description);
		static void LogicSetup();

		SNI_Manager();
		SNI_Manager(SNI_Manager * p_Manager);
		SNI_Manager(string p_Description, OnErrorHandler *p_ErrorHandler, bool p_DelayOnEvaluate, size_t p_MaxCardinalityCall, size_t p_MaxCardinalityUnify);

		virtual ~SNI_Manager();

		SN::SN_Expression DelayedCalls();

		string Description()  const;

		string LogFilePath()  const;
		void SetLogFilePath(string p_LogFilePath);

		OnErrorHandler *ErrorHandler();
		void SetErrorHandler(OnErrorHandler * p_ErrorHandler);

		size_t MaxCardinalityCall() const;
		size_t MaxCardinalityUnify() const;
		void SetMaxCardinality(size_t p_MaxCardinalityCall, size_t p_MaxCardinalityUnify);

		bool DelayOnEvaluate() const;
		void SetDelayOnEvaluate(bool p_DelayOnEvaluate);

		void StartLogging(SN::LoggingLevel p_LoggingLevel, ostream *p_Stream);

		void StartDebugCommandLineServer(skynet::DebugAction p_DebugAction, int p_KbHit(), int p_GetCh());
		void StartWebServer(skynet::DebugAction p_DebugAction, const string& p_Address, const string& p_Port, const string& p_DocRoot);

		bool HasConsole() const;
		bool KbHit();
		int  GetCh();
		bool HasDebugServer() const;

		size_t DebugFieldWidth() const;
		void SetDebugFieldWidth(size_t p_DebugFieldWidth);

		size_t DebugTitleWidth() const;
		void SetDebugTitleWidth(size_t p_DebugFieldWidth);

		ostream * CreateLogFile(SN::LoggingLevel);

		size_t LogBufferCapacity() const;
		void SetLogBufferCapacity(size_t p_LogBufferCapacity);

		size_t LogExpressionBufferCapacity() const;
		void SetLogExpressionBufferCapacity(size_t p_LogExpressionBufferCapacity);

		size_t MaxStackFrames() const;
		void SetMaxStackFrames(size_t p_MaxStackFrame);

		bool AutoDefine() const;
		void SetAutoDefine(bool p_AutoDefine);

		// Processing options
		skynet::DirectPassType GetDirectPassType() const;
		void SetDirectPassType(skynet::DirectPassType p_DirectPassType);

		skynet::EvaluationType GetEvaluationType() const;
		void SetEvaluationType(skynet::EvaluationType p_EvaluationType);

		skynet::LogicType GetLogicType() const;
		void SetLogicType(skynet::LogicType p_LogicType);

		bool TailCallOptimization() const;
		void SetTailCallOptimization(bool p_TailCallOptimization);

		PGC::PromotionStrategy MemoryPromotionStrategy() const;
		void SetMemoryPromotionStrategy(PGC::PromotionStrategy p_MemoryPromotionStrategy);

		bool AutoExpandNull() const;
		void SetAutoExpandNull(bool p_AutoExpandNull);

		// debugging
		void Breakpoint(string p_Description = "User breakpoint");
		void SetDebugAction(enum skynet::DebugAction p_DebugLevel);

	private:
		void Initialize();

		static void DebugCommandLineServer(SNI_Thread * p_Thread, int p_KbHit(), int p_GetCh());
		static void RunServer(const string & p_Address, const string & p_Port, const string & p_DocRoot);

		void ScheduleWebServerShutdown();
		static void WebServerShutdown();
		
		string m_Description;

		SNI_Manager *m_LastManager;

		OnErrorHandler *m_ErrorHandler;
		string m_LogFilePath;
		size_t m_MaxCardinalityCall;
		size_t m_MaxCardinalityUnify;

		bool m_DelayOnEvaluate;

		size_t m_DebugFieldWidth;
		size_t m_DebugTitleWidth;

		bool m_AutoDefine;

		// Processing options.
		skynet::DirectPassType m_DirectPassType;
		skynet::EvaluationType m_EvaluationType;
		skynet::LogicType m_LogicType;
		bool m_TailCallOptimization;
		PGC::PromotionStrategy m_MemoryPromotionStrategy;

		bool m_AutoExpandNull;
		
		// Console
		bool m_HasConsole;

		int (*m_KbHit)();
		int (*m_GetCh)();
		mutex m_Mutex;
		bool m_WebServerThreadUsed;
		bool m_CommandServerThreadUsed;

		SNI_User* m_User;
		bool m_HasDebugServer;
		
		size_t m_LogBufferCapacity;
		size_t m_LogExpressionBufferCapacity;

		size_t m_MaxStackFrames;

		static thread *m_CommandServerThread;
		static long m_CommandServerThreadUsageCount;

		static thread *m_WebServerThread;
		static IHTTP_Server *m_WebServer;
		static long m_WebServerThreadUsageCount;
		static bool m_LogicSetupDone;
	};
}

#endif // !defined(SNI_MANAGER_H_INCLUDED)
