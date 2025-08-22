#if !defined(SNI_MAPPING_H_INCLUDED)
#define SNI_MAPPING_H_INCLUDED

#pragma once

#include <unordered_map>

#include "sni_value.h"

namespace SN
{
	class SN_Expression;
}

namespace SNI
{
	class SNI_Class;
	class SNI_Expression;

	struct KeyHasher
	{
		std::size_t operator()(const SN::SN_Expression &p_Key) const
		{
			return p_Key.GetSNI_Expression()->Hash();
		}
	};

	struct KeyEqual
	{
		bool operator()(const SN::SN_Expression &p_Left, const SN::SN_Expression &p_Right) const
		{
			return p_Left.GetSNI_Expression()->Equivalent(p_Right.GetSNI_Expression());
		}
	};

	class SNI_Mapping : public SNI_Value
	{
		PGC_CLASS(SNI_Mapping);
	public:
		
		static SNI_Class *Class();
		virtual SNI_Class *VClass();

		SNI_Mapping();
		virtual ~SNI_Mapping();

		virtual SNI_Expression * Copy() const;

		virtual std::string GetTypeName() const;
		virtual std::string DisplayCpp() const;
		virtual long GetPriority() const;
		virtual std::string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual bool Equivalent(SNI_Object * p_Other) const;
		virtual size_t Hash() const;

		virtual bool IsFixed() const;

		virtual SN::SN_Error AssertSubscriptValue(const SNI_Value *p_Index, SN::SN_Expression p_Result);
		virtual SN::SN_Value DoSubscriptCall(const SN::SN_Value &p_Index) const;
		virtual SN::SN_Value DoReverseSubscript(const SN::SN_Value & p_Result) const;

		virtual SN::SN_Value DoCountIf(SN::SN_Expression p_Value) const;
		virtual SN::SN_Value DoCountAll() const;

		virtual SN::SN_Value DoSum() const;

		virtual void Fix(SN::SN_Expression p_Value);

	protected:
		virtual void PromoteMembers();

	private:
		std::unordered_map<SN::SN_Expression, SN::SN_Expression, KeyHasher, KeyEqual> m_Map;
		bool m_Fixed;
		SNI_Expression *m_DefaultValue;
	};
}

#endif // !defined(SNI_MAPPING_H_INCLUDED)
