#include "sni_state.h"

#ifdef USE_LOGGING
#include "logcontext.h"
#endif

#include "sn_expression.h"
#include "sn_parameter.h"
#include "sn_value.h"
#include "sn_error.h"

#include "utility.h"

#include "sn_pch.h"

namespace SNI
{
	/*static*/ SNI_Class* SNI_State::Class()
	{
		return SNI_User::GetCurrentUser()->GetOrCreatePointer<SNI_State, SNI_Class>("State");
	}

	SNI_Class *SNI_State::VClass()
	{
		return Class();
	}

	SNI_State::SNI_State()
		: m_StateId(0)
		, m_StateBuffer(new SNI_StateBuffer())
	{
	}

	SNI_State::SNI_State(const SNI_State &p_State)
		: m_StateId(p_State.m_StateId)
		, m_StateBuffer(p_State.m_StateBuffer)
	{
	}

	SNI_State::SNI_State(unsigned long long p_StateId, SNI_StateBuffer * p_StateBuffer)
		: m_StateId(p_StateId)
		, m_StateBuffer(p_StateBuffer)
	{
	}

	SNI_State::~SNI_State()
	{
	}

	SNI_Expression * SNI_State::Copy() const
	{
		return new SNI_State(*this);
	}

	void SNI_State::PromoteMembers()
	{
		REQUESTPROMOTION(m_StateBuffer);
	}

	std::string SNI_State::GetTypeName() const
	{
		return "State";
	}

	std::string SNI_State::DisplayCpp() const
	{
		return GetTypeName() + "()";
	}

	std::string SNI_State::DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const
	{
		return GetTypeName() + "_" + std::to_string(m_StateId);
	}

	bool SNI_State::Equivalent(SNI_Object * p_Other) const
	{
		SNI_State *other = dynamic_cast<SNI_State *>(p_Other);
		if (other)
		{
			return m_StateId == other->m_StateId && m_StateBuffer == other->m_StateBuffer;
		}
		return false;
	}

	SN::SN_Value SNI_State::DoUpdateState(SNI_Value *p_Left, SNI_Expression *p_Right)
	{
		return new SNI_State(m_StateBuffer->DoAssign(p_Left, p_Right), m_StateBuffer);
	}
}
