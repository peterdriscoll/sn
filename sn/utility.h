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

	class SNI_TaggedValue;
	typedef vector<SNI_TaggedValue> SNI_TaggedValueList;

	class SNI_Variable;
	typedef vector<const SNI_Variable *> SNI_VariablePointerList;

	class SNI_World;
	class SNI_WorldSet;

	string CurrentWorkingDirectory();
	string ExtractDirPath(const string& str);
	string GetFormattedTime(void);

	void Split(const string& a_buffer,
		const string& a_delimeter,
		vector<string>& a_result);

	string ReplaceAll(const string &source, const string &target, const string &replace);

	string Pad(const string &p_Text, size_t p_Width);

	string DisplayPmExpressionList(SN::SN_ExpressionList * p_ParameterList);
	string DisplayPmValueList(const SN::SN_ValueList &p_ParameterList);
	string DisplayPmParameterList(SN::SN_ParameterList * p_ParameterList);
	string DisplayPmTaggedValueList(const SNI_TaggedValueList &p_TaggedValueList);
	string DisplayPmTaggedExpressionList(const SNI_TaggedValueList &p_TaggedValueList, SNI_VariablePointerList & p_DisplayVariableList);
	string DisplayPmExpression(SNI_Expression *p_Expression);
	string DisplayPmExpression(const SN::SN_Expression &p_Expression);
	string DisplayWorlds(long p_NumWorlds, SNI_World ** p_World);
	string DisplayValues(long p_NumWorlds, SN::SN_Expression * p_ParamList, SNI_World ** p_World = NULL);
	string DisplayWorldSet(SNI_WorldSet *p_WorldSet);
}

#endif // UTILITY_H_INCLUDED
