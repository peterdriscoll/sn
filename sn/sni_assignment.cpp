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

	SNI_Expression *SNI_Assignment::DoImperativeTransform(bool &p_Changed, bool &p_Imperative, SNI_Expression *p_StateInput,  SNI_Expression *&p_StateOutput, long p_MetaLevel)
	{
		p_Imperative = true;
		SNI_Expression *state = p_StateInput;
		bool changed = false;
		bool leftChanged = false;
		SNI_StateValue *leftHandSide = m_LeftHandSide->DoImperativeTransform(leftChanged, p_Imperative, state, p_StateOutput, p_MetaLevel);
		SNI_Expression *leftVar = NULL;
		if (leftChanged)
		{
			changed = true;
			state = leftHandSide->GetState();
		}

		bool rightChanged = false;
		SNI_StateValue *rightHandSide = m_RightHandSide->DoImperativeTransform(rightChanged, p_Imperative, state, p_StateOutput, p_MetaLevel);
		SNI_Expression *rightValue = m_RightHandSide;
		if (changed)
		{
			changed = true;
			leftVar = state;
			state = rightHandSide->GetState();
		}

		return SN::SN_StateValue(SN::SN_Assignment(leftVar, rightHandSide->GetValue())(state), rightHandSide->GetValue()).GetSNI_StateValue();
	}

	using namespace skynet;

	SN::SN_Expression SNI_Assignment::DoImperativeTransform2(bool & p_InputChanged, bool & p_OutputChanged, long p_MetaLevel)
	{
		p_InputChanged = true;
		p_OutputChanged = true;
		SNI_Expression *state;

		bool leftInputChanged = false;
		bool leftOutputChanged = false;
		SN::SN_Expression leftHandSide = m_LeftHandSide->DoImperativeTransform2(leftInputChanged, leftOutputChanged, p_MetaLevel);

		bool rightInputChanged = false;
		bool rightOutputChanged = false;
		SN::SN_Expression rightHandSide = m_RightHandSide->DoImperativeTransform2(rightInputChanged, rightOutputChanged, p_MetaLevel);

		if (leftInputChanged)
		{
			if (rightInputChanged)
			{
				SN_DECLARE(s);
				SN_DECLARE(s1);
				SN_DECLARE(v1);
				SN_DECLARE(s2);
				SN_DECLARE(v2);
				return Lambda(s, Local(s1, Local(s2, Local(v1, Local(v2,
					Let(leftHandSide(s) == StateValue(s1, v1)
					&&	rightHandSide(s) == StateValue(s2, v2)
					,	StateValue(s2.AssignValue(v1, v2), v2)
				))))));
			}
			else
			{
				SN_DECLARE(s);
				SN_DECLARE(s1);
				SN_DECLARE(v1);
				SN_DECLARE(v2);
				return Lambda(s, Local(s1, Local(v1, Local(v2, 
					Let(leftHandSide(s) == StateValue(s1, v1)
					&&	rightHandSide == v2
					,	StateValue(s1.AssignValue(v1, v2), v2))
				))));
			}
		}
		else
		{
			if (rightInputChanged)
			{
				SN_DECLARE(s);
				SN_DECLARE(v1);
				SN_DECLARE(s2);
				SN_DECLARE(v2);
				return Lambda(s, Local(s2, Local(v1, Local(v2,
					Let(leftHandSide == v1
					&&	rightHandSide(s) == StateValue(s2, v2)
					,	StateValue(s2.AssignValue(v1, v2), v2))
				))));
			}
			else
			{
				SN_DECLARE(s);
				SN_DECLARE(s1);
				SN_DECLARE(v1);
				SN_DECLARE(v2);
				return Lambda(s, Local(s1, Local(v1, Local(v2,
					Let(rightHandSide == v2
					,	StateValue(s.AssignValue(leftHandSide, v2), v2))
				))));
			}
		}
	}
}