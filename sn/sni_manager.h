#if !defined(SNI_MANAGER_H_INCLUDED)
#define SNI_MANAGER_H_INCLUDED

#pragma once

#include <string>
#include <vector>
using namespace std;

#include "sni_debugcommand.h"

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
	typedef void OnErrorHandler(SN::SN_Error p_Result);

	class SNI_Frame;
	typedef vector<SNI_Frame *> SNI_FrameList;

	class SNI_Manager
	{
	public:
		static void ThrowErrorHandler(SN::SN_Error p_Result);
		SNI_Manager();
		SNI_Manager(SNI_Manager * p_Manager);
		SNI_Manager(string p_Description, OnErrorHandler *p_ErrorHandler, bool p_DelayOnEvaluate, size_t p_MaxCardinalityCall, size_t p_MaxCardinalityUnify);

		virtual ~SNI_Manager();

		void Init();

		string Description();

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

		void StartDebugCommandLineServer(SN::DebugAction p_DebugAction, int p_KbHit(), int p_GetCh());
		void StartWebServer(SN::DebugAction p_DebugAction, const string& p_Address, const string& p_Port, const string& p_DocRoot);

		bool HasConsole();
		bool KbHit();
		int  GetCh();
		bool HasDebugServer();

		size_t DebugFieldWidth();
		void SetDebugFieldWidth(size_t p_DebugFieldWidth);

		size_t DebugTitleWidth();
		void SetDebugTitleWidth(size_t p_DebugFieldWidth);

		ostream * CreateLogFile(SN::LoggingLevel);

		size_t LogBufferCapacity();
		void SetLogBufferCapacity(size_t p_LogBufferCapacity);

	private:
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

		long m_DebugFieldWidth;
		long m_DebugTitleWidth;

		bool m_HasConsole;

		int (*m_KbHit)();
		int (*m_GetCh)();
		mutex m_Mutex;
		bool m_WebServerThreadUsed;
		bool m_CommandServerThreadUsed;

		SN::SN_Transaction *m_Transaction;
		bool m_HasDebugServer;
		
		size_t m_LogBufferCapacity;

		static thread *m_CommandServerThread;
		static long m_CommandServerThreadUsageCount;

		static thread *m_WebServerThread;
		static HTTP::server::server *m_WebServer;
		static long m_WebServerThreadUsageCount;
	};
}

#endif // !defined(SNI_MANAGER_H_INCLUDED)
