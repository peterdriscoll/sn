#include "sni_stateful.h"

#include "sn_pch.h"

namespace SNI
{
	/*static*/ SNI_Class *SNI_Stateful::m_Class = NULL;
	/*static*/ SNI_Class *SNI_Stateful::Class()
	{
		if (!m_Class)
		{
			m_Class = new SNI_Class("Stateful");
		}
		return m_Class;
	}

	SNI_Class *SNI_Stateful::VClass()
	{
		return Class();
	}

	SNI_Stateful::SNI_Stateful(const SNI_Stateful & p_Other)
		: m_Mutability(p_Other.m_Mutability)
		, m_Expression(p_Other.m_Expression)
	{
	}

	SNI_Stateful::SNI_Stateful(enum skynet::Mutability p_Mutability, SNI_Expression * p_Expression)
		: m_Mutability(p_Mutability)
		, m_Expression(p_Expression)
	{
	}

	SNI_Stateful::~SNI_Stateful()
	{
	}

	string SNI_Stateful::GetTypeName() const
	{
		return "Stateful";
	}

	string SNI_Stateful::DisplayCpp() const
	{
		return GetTypeName() + "(" + m_Expression->DisplayCpp() + ")";
	}

	string SNI_Stateful::DisplaySN(long /*priority*/, SNI_DisplayOptions &p_DisplayOptions) const
	{
		if (m_Mutability == skynet::Const)
		{
			return "[[" + m_Expression->DisplaySN(0, p_DisplayOptions) + "]]";
		}
		return "{{" + m_Expression->DisplaySN(0, p_DisplayOptions) + "}}";
	}

	long SNI_Stateful::GetPriority() const
	{
		return 0;
	}

	SNI_Expression * SNI_Stateful::GetExpression()
	{
		return m_Expression;
	}

	bool SNI_Stateful::Equivalent(SNI_Object * p_Other) const
	{
		if (dynamic_cast<SNI_Stateful *>(p_Other))
		{
			SNI_Stateful * other = dynamic_cast<SNI_Stateful *>(p_Other);

			return m_Mutability == other->m_Mutability && m_Expression->Equivalent(dynamic_cast<SNI_Object *>(other->m_Expression));
		}
		return false;
	}

	SNI_Expression *SNI_Stateful::Clone(long p_MetaLevel, SNI_Frame *p_Frame, bool &p_Changed)
	{
		bool changed = false;
		SNI_Expression * expression = m_Expression->Clone(p_MetaLevel, p_Frame, changed);
		if (changed)
		{
			p_Changed = true;
			return new SNI_Stateful(m_Mutability, expression);
		}
		return this;
	}


	SN::SN_Expression SNI_Stateful::DoEvaluate(long p_MetaLevel /* = 0 */) const
	{
		return this;
	}

	SN::SN_Expression SNI_Stateful::DoPartialEvaluate(long p_MetaLevel /* = 0 */) const
	{
		return this;
	}

	SN::SN_Expression SNI_Stateful::Call(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		return m_Expression->Call(p_ParameterList, p_MetaLevel);
	}

	SN::SN_Expression SNI_Stateful::PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		return m_Expression->PartialCall(p_ParameterList, p_MetaLevel);
	}

	SN::SN_Error SNI_Stateful::PartialAssertValue(const SN::SN_Expression &p_Expression, bool /* p_Define = false */)
	{
		return m_Expression->PartialAssertValue(p_Expression, true);
	}

	SN::SN_Expression SNI_Stateful::Unify(SN::SN_ExpressionList * p_ParameterList)
	{
		return m_Expression->Unify(p_ParameterList);
	}

	SN::SN_Error SNI_Stateful::PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Result, bool p_Define)
	{
		return m_Expression->PartialUnify(p_ParameterList, p_Result);
	}

	SNI_Expression *SNI_Stateful::DoImperativeTransform(bool &p_Changed, bool &p_Imperative, SNI_Expression *p_StateInput,  SNI_Expression *&p_StateOutput, long p_MetaLevel)
	{
		if (m_Mutability == skynet::Const)
		{
			p_Changed = true;
			return SN::SN_StateValue(p_StateInput, SN::SN_Expression(m_Expression)(p_StateInput)).GetSNI_StateValue();
		}
		p_Changed = true;
		p_Imperative = true;
		SN::SN_Variable state;
		SN::SN_Variable value;
		SN::SN_StateValue result(state, SN::SN_Let(SN::SN_StateValue(state, value) == SN::SN_Expression(m_Expression)(p_StateInput), value));
		string s = result.DisplaySN();
		return result.GetSNI_StateValue();
	}
}