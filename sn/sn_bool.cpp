#include "sn_bool.h"

#include "sn_cartesian.h"
#include "sn.h"
#include "sn_expression.h"
#include "sn_value.h"

#include "sni_bool.h"

#include "sn_pch.h"

namespace SN
{
	/*static*/ SN_Class SN_Bool::Class()
	{
		return SNI_Bool::Class();
	}

	SN_Bool::SN_Bool()
		: SN_Base(NULL)
	{
	}

	SN_Bool::SN_Bool(bool p_Bool)
		: SN_Base(new SNI::SNI_Bool(p_Bool))
	{
	}

	SN_Bool::SN_Bool(const SN_Value &p_Value)
		: SN_Base(p_Value.GetSNI_Bool())
	{
	}

	SN_Bool::SN_Bool(const SN_Expression& p_Other)
		: SN_Base(p_Other.GetSNI_Bool())
	{
	}

	SN_Bool::SN_Bool(const SN_Bool &p_Bool)
		: SN_Base(p_Bool.GetSNI_Bool())
	{
	}

	SN_Bool::~SN_Bool()
	{
	}

	SNI::SNI_Bool* SN_Bool::GetSNI_Bool()
	{
		return static_cast<SNI::SNI_Bool*>(GetSNI_Bool());
	}

	SNI::SNI_Bool* SN_Bool::GetSNI_Bool() const
	{
		return static_cast<SNI::SNI_Bool*>(GetSNI_Base());
	}

	SN_Expression SN_Bool::operator==(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator==(*this, p_Other);
	}

	SN_Expression SN_Bool::operator!=(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator!=(*this, p_Other);
	}

	bool SN_Bool::GetBool() const
	{
		return GetSNI_Bool()->GetBool();
	}

	SN_Expression SN_Bool::Assert() const
	{
		return SN_Operators::Assert(*this);
	}

	SN_Expression SN_Bool::PartialAssert() const
	{
		return SN_Operators::PartialAssert(*this);
	}

	SN_Expression SN_Bool::Evaluate() const
	{
		return SN_Operators::Evaluate(*this);
	}

	SN_Expression SN_Bool::PartialEvaluate() const
	{
		return SN_Operators::PartialEvaluate(*this);
	}

	SN_Expression SN_Bool::operator !() const
	{
		return SN_Operators::operator !(*this);
	}

	SN_Expression SN_Bool::operator &&(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator &&(*this, p_Other);
	}

	SN_Expression SN_Bool::operator &&(const  bool p_Other) const
	{
		return SN_Operators::operator &&(*this, SN_Bool(p_Other));
	}

	SN_Expression SN_Bool::operator ||(const SN_Expression &p_Other) const
	{
		return SN_Operators::operator ||(*this, p_Other);
	}

	SN_Expression SN_Bool::operator ||(const bool p_Other) const
	{
		return SN_Operators::operator ||(*this, SN_Bool(p_Other));
	}

	SN_Expression SN_Bool::If(const SN_Expression &p_PositiveCase, const SN_Expression &p_NegativeCase) const
	{
		return SN_Operators::If(*this, p_PositiveCase, p_NegativeCase);
	}

	SN_Expression SN_Bool::Implies(const SN_Expression & p_PositiveCase) const
	{
		return SN_Operators::Implies(*this, p_PositiveCase);
	}

	SN_Expression SN_Bool::RevAnd(const SN_Expression & p_PositiveCase) const
	{
		return SN_Operators::RevAnd(*this, p_PositiveCase);
	}

	SN_Expression SN_Bool::RevOr(const SN_Expression & p_Condition) const
	{
		return SN_Operators::RevOr(*this, p_Condition);
	}

	SN_Expression SN_Bool::UnaryAnd() const
	{
		return SN_Operators::UnaryAnd(*this);
	}

	SN_Expression SN_Bool::UnaryOr() const
	{
		return SN_Operators::UnaryOr(*this);
	}

	SN_Expression SN_Bool::Collapse() const
	{
		return SN_Operators::Collapse(*this);
	}

	SN_Expression SN_Bool::IsA(const SN_Expression &p_Parent) const
	{
		return SN_Operators::IsA(*this, p_Parent);
	}
}
