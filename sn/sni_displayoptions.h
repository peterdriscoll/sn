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

		void SetVarName(const std::string & p_VarName);

		std::string CreateName(const std::string & p_TypeName);

		enum DisplayOptionType GetDebugHTML();

		void IncrementLevel();
		void DecrementLevel();
		long GetLevel();
		bool CheckLevel();
	private:
		std::map<std::string, long> m_TypeMap;
		std::string m_VarName;
		enum DisplayOptionType m_DebugHTML;
		long m_Level;
		long m_MaxLevel;
	};
}

#endif // !defined(SNI_DISPLAYOPTIONS_H)
