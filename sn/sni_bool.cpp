#include "sni_bool.h"

#include "sn_bool.h"
#include "sn_error.h"

#include "sni_valueset.h"
#include "sni_helpers.h"
#include "sni_class.h"

#include "sn_pch.h"

namespace SNI
{
	/*static*/ SNI_Class* SNI_Bool::Class()
	{
		return SNI_User::GetCurrentUser()->GetOrCreatePointer<SNI_Bool, SNI_Class>("Bool");
	}

	SNI_Class *SNI_Bool::VClass()
	{
		return Class();
	}

	SNI_Bool::SNI_Bool()
		: m_Expression(false)
	{

	}

	SNI_Bool::~SNI_Bool()
	{

	}

	SNI_Bool::SNI_Bool(const SNI_Bool &p_Other)
		: m_Expression(p_Other.m_Expression)
	{

	}

	SNI_Bool::SNI_Bool(bool p_Other)
		: m_Expression(p_Other)
	{

	}

	SNI_Expression * SNI_Bool::Copy() const
	{
		return new SNI_Bool(*this);
	}

	std::string SNI_Bool::GetTypeName() const
	{
		return "Bool";
	}

	bool SNI_Bool::AllValuesBoolean() const
	{
		return true;
	}

	std::string SNI_Bool::DisplayCpp() const
	{
		if (m_Expression)
		{
			return "true";
		}
		return "false";
	}

	std::string SNI_Bool::DisplaySN(long /*priority*/, SNI_DisplayOptions & /*p_DisplayOptions*/) const
	{
		if (m_Expression)
		{
			return "true";
		}
		return "false";
	}

	long SNI_Bool::GetPriority() const
	{
		return 100;
	}

	bool SNI_Bool::Equivalent(SNI_Object * p_Other) const
	{
		if (dynamic_cast<SNI_Bool *>(p_Other))
		{
			SNI_Bool *l_bool = dynamic_cast<SNI_Bool*>(p_Other);
			return m_Expression == l_bool->m_Expression;
		}
		return false;
	}

	size_t SNI_Bool::Hash() const
	{
		return _Hash_representation(m_Expression);
	}

	SN::SN_Error SNI_Bool::DoAssert()
	{
		return SN::SN_Error(m_Expression, false);
	}


	SN::SN_Value SNI_Bool::DoHasA(SNI_Value * p_Member, SNI_Value * p_Name) const
	{
		return SN::SN_Error(false, false, GetTypeName() + " HasA function not implemented.");
	}

	bool SNI_Bool::GetBool() const
	{
		return m_Expression;
	}

	SN::SN_Value SNI_Bool::DoAnd(SNI_Expression * p_Other) const
	{
		if (!m_Expression)
		{
			return SN::SN_Bool(false);
		}
		else if (dynamic_cast<SNI_Bool *>(p_Other))
		{
			SNI_Bool *l_bool = dynamic_cast<SNI_Bool*>(p_Other);
			return SN::SN_Bool(m_Expression && l_bool->m_Expression);
		}
		else if (dynamic_cast<SNI_ValueSet *>(p_Other))
		{
			return p_Other->DoAnd(const_cast<SNI_Bool *>(this));
		}
		return SN::SN_Error(false, false, GetTypeName() + " && operator not implemented.");
	}

	SN::SN_Value SNI_Bool::DoOr(SNI_Expression * p_Other) const
	{
		if (m_Expression)
		{
			return SN::SN_Bool(true);
		}
		if (dynamic_cast<SNI_Bool *>(p_Other))
		{
			SNI_Bool *l_bool = dynamic_cast<SNI_Bool*>(p_Other);
			return SN::SN_Bool(m_Expression || l_bool->m_Expression);
		}
		if (dynamic_cast<SNI_ValueSet *>(p_Other))
		{
			return SNI_ValueSet::GeneralDoOr(this, p_Other);
		}
		return SN::SN_Error(false, false, GetTypeName() + " && operator not implemented.");
	}

	SN::SN_Value SNI_Bool::DoIf(SNI_Expression * p_PositiveCase, SNI_Expression * p_NegativeCase) const
	{
		if (m_Expression)
		{
			return p_PositiveCase->DoEvaluate();
		}
		else
		{
			return p_NegativeCase->DoEvaluate();
		}
	}

	SN::SN_Value SNI_Bool::DoImplies(SNI_Expression * p_PositiveCase) const
	{
		if (m_Expression)
		{
			return p_PositiveCase;
		}
		return skynet::True;
	}

	SN::SN_Value SNI_Bool::DoNot() const
	{
		return SN::SN_Bool(!m_Expression);
	}

	SN::SN_Value SNI_Bool::DoCollapse()
	{
		return this;
	}

	SN::SN_Value SNI_Bool::DoRevAnd(SNI_Expression * p_PositiveCase) const
	{
		if (GetBool())
		{
			return skynet::True;
		}
		if (!p_PositiveCase->GetBool())
		{
			return skynet::Null;
		}
		return skynet::False;
	}

	SN::SN_Value SNI_Bool::DoRevOr(SNI_Expression * p_Condition) const
	{
		if (!GetBool())
		{
			return skynet::False;
		}
		if (p_Condition->GetBool())
		{
			//return skynet::Null;
			return SN::SN_ValueSet(skynet::True, skynet::False);
		}
		return skynet::True;
	}

	SN::SN_Value SNI_Bool::DoUnaryAnd() const
	{
		if (!GetBool())
		{
			return skynet::False;
		}
		return SN::SN_ValueSet(skynet::True, skynet::False);
	}

	SN::SN_Value SNI_Bool::DoUnaryOr() const
	{
		if (GetBool())
		{
			return skynet::True;
		}
		return SN::SN_ValueSet(skynet::True, skynet::False);
	}
}
