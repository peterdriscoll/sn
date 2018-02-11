
#if !defined(LOGCONTEXT_H)
#define LOGCONTEXT_H

#pragma once

#include "exp_ctrl_sn.h"
#include "log.h"
#include "../sn/sni_result.h"
#include "sn_expression.h"

#include <string>
using namespace std;

namespace SNI
{
	class SNI_World;
}

#define LOG_RETURN(context, result) context.LogReturn(result)

namespace SN
{
	class LogContext
	{
	public:
		static string GetContextDescription(long p_LogIndex);

	public:
		LogContext(const string &p_Description, long p_LogIndex = LOG_STANDARD_INDEX);
		virtual ~LogContext();

		string GetDescription() const;
		string GetSimpleDescription() const;
		long Depth();
		string Space(long p_Spacer);
		string Pad(const string &p_Text);
		string PadWidth(const string & p_Text, size_t p_width, char p_Char);
		void LogExpression(string name, SN::SN_Expression p_expression, SNI::SNI_World *p_world = NULL);
		void LogText(string p_tag, string p_text);
		template<typename T> T LogReturn(T result)
		{
			Log::GetLog(m_LogIndex)->WriteLine("\nreturn    " + Pad(SN::SN_Expression(result).DisplaySN()) + m_Description);
			return result;
		};
		bool LogReturn(bool result)
		{
			Log::GetLog(m_LogIndex)->WriteLine("\nreturn    " + Pad(result ? "true" : "false") + m_Description);
			return result;
		};
		template<> SNI::SNI_Result LogReturn<SNI::SNI_Result>(SNI::SNI_Result result)
		{
			Log::GetLog(m_LogIndex)->WriteLine("\nreturn    " + Pad(result.GetResult() ? "true" : "false") + m_Description);
			return result;
		};

	private:
		LogContext *m_ParentContext;
		string m_Description;
		long m_LogIndex;
		long m_Depth;
	};
}

#endif // !defined(LOGCONTEXT_H)
