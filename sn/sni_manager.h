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

		void ConsoleFunctions(bool p_Kbhit(), int p_GetCh());

		bool HasConsole();
		bool KbHit();
		int  GetCh();

		void DebugCommand(string p_text);

	private:
		static SNI_Manager *m_TopManager;
		SNI_Manager *m_LastManager;

		string m_LogFilePath;

		OnErrorHandler *m_ErrorHandler;

		size_t m_MaxCardinalityCall;
		size_t m_MaxCardinalityUnify;

		bool m_DelayOnEvaluate;

		bool m_HasConsole;
		bool (*m_KbHit)();
		int (*m_GetCh)();
	};
}

#endif // !defined(SNI_MANAGER_H_INCLUDED)
