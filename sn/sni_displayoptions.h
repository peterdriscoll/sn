#if !defined(SNI_DISPLAYOPTIONS_H)
#define SNI_DISPLAYOPTIONS_H

#pragma once

#include <map>

namespace SNI
{
	enum DisplayOptionType
	{
		doTextOnly,
		doDebugPointsJS,
		doDebugPointsHTML
	};

	class SNI_DisplayOptions
	{
	public:
		SNI_DisplayOptions(enum DisplayOptionType p_DebugHTML);

		virtual ~SNI_DisplayOptions();

		void SetVarName(const string & p_VarName);

		string CreateName(const string & p_TypeName);

		enum DisplayOptionType GetDebugHTML();

		string GetBreakPointLocation();
		void SetBreakPointLocation(const string &p_BreakPointLocation);

	private:
		map<string, long> m_TypeMap;
		string m_VarName;
		enum DisplayOptionType m_DebugHTML;
		string m_BreakPointLocation;
	};
}

#endif // !defined(SNI_DISPLAYOPTIONS_H)
