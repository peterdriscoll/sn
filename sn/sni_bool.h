#if !defined(SNI_BOOL_H_INCLUDED)
#define SNI_BOOL_H_INCLUDED

#pragma once

#include <string>

#include "sni_value.h"

namespace SNI
{
	class SNI_Instance;

	class SNI_Bool : public SNI_Value
	{
		PGC_CLASS(SNI_Bool);
	public:
		static SNI_Instance *Instance();

		SNI_Bool();
		SNI_Bool(const SNI_Bool &p_Other);
		SNI_Bool(bool p_Other);
		virtual ~SNI_Bool();

		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual long GetPriority() const;

		virtual bool Equivalent(SNI_Object * p_Other) const;
		virtual bool GetBool() const;

		virtual SN::SN_Error AssertIsAValue(const SNI_Value * p_Parent, SN::SN_Expression p_Result);
		virtual SN::SN_Value DoIsA(const SNI_Value * p_Parent) const;
		virtual SN::SN_Value DoHasA(SNI_Value * p_Member, SNI_Value * p_Name) const;

		virtual SN::SN_Value DoAnd(SNI_Expression * p_Other) const;
		virtual SN::SN_Value DoOr(SNI_Expression * p_Other) const;
		virtual SN::SN_Value DoRevAnd(SNI_Expression * p_PositiveCase) const;
		virtual SN::SN_Value DoRevOr(SNI_Expression * p_Condition) const;
		virtual SN::SN_Value DoIf(SNI_Expression * p_PositiveCase, SNI_Expression * p_NegativeCase) const;
		virtual SN::SN_Value DoImplies(SNI_Expression * p_PositiveCase) const;
		virtual SN::SN_Value DoNot() const;

		virtual SN::SN_Value DoCollapse();

		virtual SN::SN_Value DoUnaryAnd() const;
		virtual SN::SN_Value DoUnaryOr() const;

	private:
		static SNI_Instance *m_Instance;
		bool m_Expression;
	};

	typedef vector<SNI_Bool *> SNI_BoolList;
}

#endif // !defined(SNI_BOOL_H_INCLUDED)
