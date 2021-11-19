#if !defined(SN_VARIABLE_H_INCLUDED)
#define SN_VARIABLE_H_INCLUDED

#pragma once

#include "exp_ctrl_sn.h"
#include "sn_expression.h"
#include <string>
using namespace std;

namespace SN
{
	class SNI::SNI_Variable;
	class SN_Value;
	class SN_String;
	class SN_Char;

	template <typename T>
	class SN_Real;

	SN_APPLY_TYPES(DUMMY, DUMMY, SN_FORWARD)

	class SN_EXPORT SN_Variable : public SN_Expression
	{
	public:

		SN_Variable();
		SN_Variable(const SN_Variable &p_other);
		SN_Variable(SNI::SNI_Variable* p_other);
		SN_Variable(const SN_Expression &p_other, bool);
		SN_Variable(const SN_Expression &p_other);
		SN_Variable(const string &p_Name, const string &p_DomainName = "");
		SN_Variable(const string &p_Name, enum skynet::DefineType p_DefineType, bool p_Local);
		SN_Variable(const string &p_Name, const SN_Expression& p_other, enum skynet::DefineType p_DefineType, bool p_Local);

		SN_APPLY_TYPES(SN_Variable, DUMMY, SN_CONSTRUCTOR)

		virtual ~SN_Variable();

		bool GetBool() const;
		string GetString() const;
		size_t Count() const;
		size_t Length() const;

		string GetName();
		void SetName(const string& p_Name);
		string GetDomainName();
		void SetDomainName(const string &p_Name);
		string FrameName();
		skynet::DefineType GetInline();
		void SetInline(skynet::DefineType p_DefineType);

		SN_Expression GetValue() const;
		void SetValue(SN_Expression p_Value, enum skynet::DefineType p_DefineType = skynet::Inname);

		SN_Expression DoPartialEvaluate(long p_MetaLevel = 0) const;

		void Simplify();

		SNI::SNI_Variable* GetSNI_Variable() const;
	};
}

#endif // !defined(SN_VARIABLE_H_INCLUDED)
