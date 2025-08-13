#include "sni_world.h"
#include "sni_worldset.h"

#include "sn_pch.h"

#include "log.h"
#include "logcontext.h"

namespace SN
{
	thread_local SN::LogContext *t_BaseContext[MAX_LOG_INDEX] = { NULL, NULL };

	long m_LineCount = 0;

	const long prespacer = 0;
	const long horizontal_line_interval = 10;
	const long vertical_line_interval = 5;

	std::string SN::LogContext::GetContextDescription(long p_LogIndex)
	{
		if (t_BaseContext[p_LogIndex])
		{
			return t_BaseContext[p_LogIndex]->GetDescription();
		}
		return "";
	}

	LogContext::LogContext(const std::string &p_Description, long p_LogIndex)
		: m_Description(p_Description)
		, m_Depth(0)
	{
		m_LogIndex = p_LogIndex;
		m_ParentContext = t_BaseContext[m_LogIndex];
		t_BaseContext[m_LogIndex] = this;
		if (m_ParentContext)
		{
			m_Depth = m_ParentContext->Depth() + 1;
		}
		Log::GetLog(m_LogIndex)->WriteLine(std::string("\nCall      ") + Pad("") + m_Description);
	}

	LogContext::~LogContext()
	{
		t_BaseContext[m_LogIndex] = m_ParentContext;
		m_ParentContext = NULL;
	}

	std::string LogContext::GetDescription() const
	{
		std::string line = "\nin " + m_Description;
		if (m_ParentContext)
		{
			line += m_ParentContext->GetDescription();
		}
		return line;
	}

	std::string LogContext::GetSimpleDescription() const
	{
		return m_Description;
	}

	long LogContext::Depth()
	{
		return m_Depth;
	}

	std::string LogContext::Space(long p_Spacer)
	{
		++m_LineCount;
		char ch;
		if (0 == m_LineCount % vertical_line_interval)
		{
			ch = '.';
		}
		else
		{
			ch = ' ';
		}
		std::string pattern = std::string(horizontal_line_interval - 1, ch) + "|";;
		if (p_Spacer < prespacer)
		{
			return std::string(p_Spacer, ch);
		}
		long count = (p_Spacer - prespacer) / horizontal_line_interval;
		std::string patternText = std::string(prespacer, ch);
		for (long j = 0; j < count; j++)
		{
			patternText += pattern;
		}
		return patternText + pattern.substr(0, p_Spacer - prespacer - count * horizontal_line_interval);
	}

	std::string LogContext::Pad(const std::string &p_Text)
	{
		size_t spacer = 10 + Depth();
		size_t resultLength = p_Text.length();
		if (spacer < resultLength)
		{
			resultLength = spacer;
		}
		return p_Text + Space((long) spacer).substr(resultLength) + " ";
	}

	std::string LogContext::PadWidth(const std::string &p_Text, size_t p_width, char p_Char)
	{
		size_t resultLength = p_Text.length();
		if (p_width < resultLength)
		{
			resultLength = p_width;
		}
		std::string temp = p_Text.substr(0, resultLength) + std::string(p_width - resultLength, p_Char) + "|";
		return temp;
	}

	void LogContext::LogExpression(std::string name, SN::SN_Expression p_expression, SNI::SNI_World *p_world)
	{
		std::string worldtext;
		if (p_world)
		{
			SNI_DisplayOptions displayOptions(doTextOnly);
			worldtext = " :: " + p_world->DisplaySN(displayOptions);
		}
		Log::GetLog(m_LogIndex)->WriteLine("\nValue     " + Pad(name) + p_expression.DisplayValueSN());
	}

	void LogContext::LogText(std::string p_tag, std::string p_text)
	{
		Log::GetLog(m_LogIndex)->WriteLine("\n          " + Pad(p_tag) + p_text);
	}
}
