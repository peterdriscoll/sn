
#if !defined(LOGCONTEXT_H)
#define LOGCONTEXT_H

#pragma once

#include "exp_ctrl_sn.h"
#include "log.h"
#include "../sn/sni_result.h"
#include "sn_expression.h"

#include <string>


namespace SNI
{
	class SNI_World;
}

namespace SN
{
	class LogContext
	{
	public:
		static std::string GetContextDescription(long p_LogIndex);

	public:
		LogContext(const std::string &p_Description, long p_LogIndex = LOG_STANDARD_INDEX);
		virtual ~LogContext();

		std::string GetDescription() const;
		std::string GetSimpleDescription() const;
		long Depth();
		std::string Space(long p_Spacer);
		std::string Pad(const std::string &p_Text);
		std::string PadWidth(const std::string & p_Text, size_t p_width, char p_Char);
		void LogExpression(std::string name, SN::SN_Expression p_expression, SNI::SNI_World *p_world = NULL);
		void LogText(std::string p_tag, std::string p_text);
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
		std::string m_Description;
		long m_LogIndex;
		long m_Depth;
	};
}

#endif // !defined(LOGCONTEXT_H)
