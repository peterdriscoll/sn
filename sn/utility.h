#if !defined(UTILITY_H_INCLUDED)
#define UTILITY_H_INCLUDED

#pragma once

#include <string>
#include <vector>
using namespace std;


namespace SN
{
	class SN_Expression;
	typedef std::vector<SN_Expression> SN_ExpressionList;

	class SN_Value;
	typedef std::vector<SN_Value> SN_ValueList;

	class SN_Parameter;
	typedef std::vector<SN_Parameter> SN_ParameterList;
}

namespace SNI
{
	class SNI_Expression;
	class SNI_DisplayOptions;

	class SNI_TaggedValue;
	typedef std::vector<SNI_TaggedValue> SNI_TaggedValueList;

	class SNI_Variable;
	typedef std::vector<const SNI_Variable *> SNI_VariableConstPointerList;

	class SNI_World;
	class SNI_WorldSet;

	std::string CurrentWorkingDirectory();
	std::string ExtractDirPath(const std::string& str);
	std::string GetFormattedTime(void);
	void OpenURLInBrowser(const std::string& str);

	void Split(const std::string& a_buffer,
		const std::string& a_delimeter,
		std::vector<std::string>& a_result);

	std::string ReplaceAll(const std::string &source, const std::string &target, const std::string &replace);
	std::string EscapeStringToCPP(const std::string &p_Unescaped);
	std::string UnescapeStringToCPP(const std::string &p_Escaped);
	std::string EscapeStringToCPPUsingQuoted(const std::string &p_Unescaped);
	std::string UnescapeStringToCPPUsingQuoted(const std::string &p_Escaped);
	std::string EscapeStringToJSON(const std::string &p_Unescaped);
	std::string UnescapeStringToJSON(const std::string &p_Escaped);
	std::string EscapeStringToHTML(const std::string &p_Unescaped);

	std::string Pad(const std::string &p_Text, size_t p_Width);
	std::string Trim(const std::string &p_Text, size_t p_Width);
	std::string Details(const std::string &p_Text, size_t p_Width);
	std::string DetailsFS(const std::string &p_Text, const std::string &p_TextHTML, size_t p_Width);

	std::string DisplaySnExpressionList(SN::SN_ExpressionList * p_ParameterList);
	std::string DisplaySnValueList(const SN::SN_ValueList &p_ParameterList);
	std::string DisplaySnParameterList(SN::SN_ParameterList * p_ParameterList);
	std::string DisplaySnTaggedValueList(const SNI_TaggedValueList &p_TaggedValueList, SNI_DisplayOptions & p_DisplayOptions);
	std::string DisplaySnTaggedExpressionList(const SNI_TaggedValueList &p_TaggedValueList, SNI_DisplayOptions & p_DisplayOptions);
	std::string DisplaySnExpression(SNI_Expression *p_Expression);
	std::string DisplaySnExpression(const SN::SN_Expression &p_Expression);
	std::string DisplayWorlds(long p_NumWorlds, SNI_World ** p_World);
	std::string DisplayWorldsD(SNI_DisplayOptions & p_DisplayOptions, long p_NumWorlds, SNI_World ** p_World);
	std::string DisplayValues(long p_NumWorlds, SN::SN_Expression * p_ParamList, SNI_World ** p_World = NULL);
	std::string DisplayValuesD(SNI_DisplayOptions &p_DisplayOptions, long p_NumWorlds, SN::SN_Expression * p_ParamList, SNI_World ** p_World = NULL);
	std::string DisplayWorldSet(SNI_WorldSet *p_WorldSet);

	std::string MakeBreakPoint(const std::string &p_DebugId, long p_Index);
	std::string MakeBreakPointJS(const std::string &p_DebugId, long p_Index);
}

#endif // UTILITY_H_INCLUDED
