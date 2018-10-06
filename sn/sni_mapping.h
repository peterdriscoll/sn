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
		static SNI_Class * Class();

		SNI_Mapping();
		virtual ~SNI_Mapping();

		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual long GetPriority() const;
		virtual string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual bool Equivalent(SNI_Object * p_Other) const;
		virtual size_t Hash() const;

		virtual SN::SN_Error AssertIsAValue(const SNI_Value * p_Parent, SN::SN_Expression p_Result);
		virtual SN::SN_Value DoIsA(const SNI_Value * p_Parent) const;

		virtual SN::SN_Error AssertSubscriptValue(const SNI_Value *p_Index, SN::SN_Expression p_Result);
		virtual SN::SN_Value DoSubscriptCall(const SN::SN_Value &p_Index) const;
		virtual SN::SN_Value DoReverseSubscript(const SN::SN_Value & p_Result) const;

		virtual void Fix(SN::SN_Expression p_Value);

	protected:
		virtual void PromoteMembers();

	private:
		static SNI_Class *m_Class;

		unordered_map<SN::SN_Expression, SN::SN_Expression, KeyHasher, KeyEqual> m_Map;
		bool m_Fixed;
		SNI_Expression *m_DefaultValue;
	};

	typedef vector<SNI_Mapping> SNI_MappingList;
}

#endif // !defined(SNI_Mapping_H_INCLUDED)
