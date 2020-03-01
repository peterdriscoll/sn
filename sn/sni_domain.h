#if !defined(SNI_DOMAIN_H_INCLUDED)
#define SNI_DOMAIN_H_INCLUDED

#pragma once

#include <unordered_map>

#include "sni_value.h"
#include "sni_variable.h"

namespace SN
{
	class SN_Expression;
	class SN_Variable;
}

namespace SNI
{
	class SNI_Class;
	class SNI_Expression;

	class SNI_Domain : public SNI_Value
	{
		PGC_CLASS(SNI_Domain);
	public:
		static SNI_Class * Class();
		virtual SNI_Class * VClass();

		SNI_Domain(const string &p_Name);
		virtual ~SNI_Domain();

		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual long GetPriority() const;
		virtual string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual bool Equivalent(SNI_Object * p_Other) const;
		virtual size_t Hash() const;


		virtual SN::SN_Error AssertSubscriptValue(const SNI_Value *p_Index, SN::SN_Expression p_Result);
		virtual SN::SN_Expression operator[](const string & p_Index);
		virtual SN::SN_Value DoSubscriptCall(const SN::SN_Value &p_Index) const;
		virtual SN::SN_Value DoReverseSubscript(const SN::SN_Value & p_Result) const;

	protected:
		virtual void PromoteMembers();

	private:
		static SNI_Class *m_Class;

		unordered_map<string, SN::SN_Variable> m_Map;
		string m_Name;
	};
}

#endif // !defined(SNI_DOMAIN_H_INCLUDED)
