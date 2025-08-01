#if !defined(UTILITY_H_INCLUDED)
#define UTILITY_H_INCLUDED

#pragma once

#include <string>
#include <vector>
using namespace std;


namespace SN
{
	class SN_Expression;
	typedef vector<SN_Expression> SN_ExpressionList;

	class SN_Value;
	typedef vector<SN_Value> SN_ValueList;

	class SN_Parameter;
	typedef vector<SN_Parameter> SN_ParameterList;
}

namespace SNI
{
	class SNI_Expression;
	class SNI_DisplayOptions;

	class SNI_TaggedValue;
	typedef vector<SNI_TaggedValue> SNI_TaggedValueList;

	class SNI_Variable;
	typedef vector<const SNI_Variable *> SNI_VariableConstPointerList;

	class SNI_World;
	class SNI_WorldSet;

	string CurrentWorkingDirectory();
	string ExtractDirPath(const string& str);
	string GetFormattedTime(void);
	void OpenURLInBrowser(const string& str);

	void Split(const string& a_buffer,
		const string& a_delimeter,
		vector<string>& a_result);

	string ReplaceAll(const string &source, const string &target, const string &replace);
	string EscapeStringToCPP(const std::string &p_Unescaped);
	string UnescapeStringToCPP(const std::string &p_Escaped);
	string EscapeStringToCPPUsingQuoted(const std::string &p_Unescaped);
	string UnescapeStringToCPPUsingQuoted(const string &p_Escaped);
	string EscapeStringToJSON(const string &p_Unescaped);
	string UnescapeStringToJSON(const string &p_Escaped);
	string EscapeStringToHTML(const string &p_Unescaped);

	string Pad(const string &p_Text, size_t p_Width);
	string Trim(const string &p_Text, size_t p_Width);
	string Details(const string &p_Text, size_t p_Width);
	string DetailsFS(const string &p_Text, const string &p_TextHTML, size_t p_Width);

	string DisplaySnExpressionList(SN::SN_ExpressionList * p_ParameterList);
	string DisplaySnValueList(const SN::SN_ValueList &p_ParameterList);
	string DisplaySnParameterList(SN::SN_ParameterList * p_ParameterList);
	string DisplaySnTaggedValueList(const SNI_TaggedValueList &p_TaggedValueList, SNI_DisplayOptions & p_DisplayOptions);
	string DisplaySnTaggedExpressionList(const SNI_TaggedValueList &p_TaggedValueList, SNI_DisplayOptions & p_DisplayOptions);
	string DisplaySnExpression(SNI_Expression *p_Expression);
	string DisplaySnExpression(const SN::SN_Expression &p_Expression);
	string DisplayWorlds(long p_NumWorlds, SNI_World ** p_World);
	string DisplayWorldsD(SNI_DisplayOptions & p_DisplayOptions, long p_NumWorlds, SNI_World ** p_World);
	string DisplayValues(long p_NumWorlds, SN::SN_Expression * p_ParamList, SNI_World ** p_World = NULL);
	string DisplayValuesD(SNI_DisplayOptions &p_DisplayOptions, long p_NumWorlds, SN::SN_Expression * p_ParamList, SNI_World ** p_World = NULL);
	string DisplayWorldSet(SNI_WorldSet *p_WorldSet);

	string MakeBreakPoint(const string &p_DebugId, long p_Index);
	string MakeBreakPointJS(const string &p_DebugId, long p_Index);
}

#endif // UTILITY_H_INCLUDED
