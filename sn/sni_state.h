#if !defined(SNI_STATE_H_INCLUDED)
#define SNI_STATE_H_INCLUDED

#pragma once

#include "sni_value.h"

namespace SN
{
	class SN_Expression;
}

namespace SNI
{
	class SNI_Class;
	class SNI_Expression;
	class SNI_StateBuffer;

	class SNI_State : public SNI_Value
	{
		PGC_CLASS(SNI_State);
	public:
		
		static SNI_Class *Class();
		virtual SNI_Class *VClass();

		SNI_State();
		SNI_State(const SNI_State & p_State);
		SNI_State(unsigned long long p_StateId, SNI_StateBuffer * p_StateBuffer);
		virtual ~SNI_State();

		virtual SNI_Expression * Copy() const;

		virtual std::string GetTypeName() const;
		virtual std::string DisplayCpp() const;
		virtual std::string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual bool Equivalent(SNI_Object * p_Other) const;

		SN::SN_Value DoUpdateState(SNI_Value* p_Left, SNI_Expression* p_Right);

	protected:
		virtual void PromoteMembers();

	private:
		unsigned long long m_StateId;
		SNI_StateBuffer *m_StateBuffer;
	};
}

#endif // !defined(SNI_STATE_H_INCLUDED)
