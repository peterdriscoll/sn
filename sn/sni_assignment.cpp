#include "sni_assignment.h"

#include "sn_pch.h"

namespace SNI
{
	SNI_Assignment::SNI_Assignment(const SNI_Assignment &p_Other)
		: m_LeftHandSide(p_Other.m_LeftHandSide)
		, m_RightHandSide(p_Other.m_RightHandSide)
	{
		PromoteMembers();
	}

	SNI_Assignment::SNI_Assignment(SNI_Expression *p_LeftHandSide, SNI_Expression *p_RightHandSide)
		: m_LeftHandSide(p_LeftHandSide)
		, m_RightHandSide(p_RightHandSide)
	{
		PromoteMembers();
	}

	SNI_Assignment::~SNI_Assignment()
	{

	}

	void SNI_Assignment::PromoteMembers()
	{
		REQUESTPROMOTION(m_LeftHandSide);
		REQUESTPROMOTION(m_RightHandSide);
	}

	string SNI_Assignment::GetTypeName() const
	{
		return "Lambda";
	}

	string SNI_Assignment::GetReferredName() const
	{
		return GetTypeName();
	}

	bool SNI_Assignment::IsValue() const
	{
		return false;
	}

	string SNI_Assignment::DisplayCpp() const
	{
		return m_LeftHandSide->DisplayCpp() + " = " + m_RightHandSide->DisplayCpp();
	}

	string SNI_Assignment::DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const
	{
		string text = m_LeftHandSide->DisplaySN(GetPriority(), p_DisplayOptions) + SetBreakPoint(":=", p_DisplayOptions, this, SN::LeftId) + m_RightHandSide->DisplaySN(GetPriority(), p_DisplayOptions);
		return Bracket(priority, text, p_DisplayOptions, this);
	}

	long SNI_Assignment::GetPriority() const
	{
		return 1000;
	}

	string SNI_Assignment::GetOperator() const
	{
		return ":=";
	}

	SNI_Expression * SNI_Assignment::LeftHandSide()
	{
		return m_LeftHandSide;
	}

	SNI_Expression * SNI_Assignment::RightHandSide()
	{
		return m_RightHandSide;
	}

	SNI_Expression * SNI_Assignment::Clone(long p_MetaLevel, SNI_Frame *p_Frame, bool &p_Changed)
	{
		bool changed = false;
		SNI_Expression * leftHandSide = m_LeftHandSide->Clone(p_MetaLevel, p_Frame, changed);
		SNI_Expression * rightHandSide = m_RightHandSide->Clone(p_MetaLevel, p_Frame, changed);
		if (changed)
		{
			p_Changed = true;
			return dynamic_cast<SNI_Expression *>(new SNI_Assignment(leftHandSide, rightHandSide));
		}
		return this;
	}

	bool SNI_Assignment::Equivalent(SNI_Object * p_Other) const
	{
		SNI_Assignment * assignment = dynamic_cast<SNI_Assignment *>(p_Other);
		if (assignment)
		{
			return m_LeftHandSide->Equivalent(dynamic_cast<SNI_Object *>(assignment->m_LeftHandSide))
				&& m_RightHandSide->Equivalent(dynamic_cast<SNI_Object *>(assignment->m_RightHandSide));
		}
		return false;
	}

	size_t SNI_Assignment::Cardinality(size_t) const
	{
		return 1;
	}

	SN::SN_Expression SNI_Assignment::Call(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		SNI_Expression *p_DebugSource = NULL;
		Breakpoint(SN::DebugStop, SN::LeftId, GetTypeName(), "Call", NULL, SN::CallPoint);

		ASSERTM(p_ParameterList->size() > 0, "Cannot call an assignment without a state.");
		SN::SN_Expression state = p_ParameterList->back().GetSNI_Expression();
		p_ParameterList->pop_back();

		SN::SN_Error e = skynet::OK;
		return this;
	}

	SN::SN_Expression SNI_Assignment::PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel /* = 0 */) const
	{
		return this;
	}

	SN::SN_Expression SNI_Assignment::DoEvaluate(long p_MetaLevel /* = 0 */) const
	{
		return this;
	}

	SN::SN_Expression SNI_Assignment::DoPartialEvaluate(long p_MetaLevel /* = 0 */) const
	{
		return this;
	}

	SN::SN_Expression SNI_Assignment::Unify(SN::SN_ExpressionList * p_ParameterList)
	{
		return skynet::OK;
	}

	SN::SN_Error SNI_Assignment::PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Result, bool p_Define)
	{
		return skynet::OK;
	}
}