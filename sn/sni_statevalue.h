#if !defined(SNI_STATEVALUE_H_INCLUDED)
#define SNI_STATEVALUE_H_INCLUDED

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

	class SNI_StateValue : public SNI_Value
	{
		PGC_CLASS(SNI_StateValue);
	public:
		static SNI_Class *m_Class;
		static SNI_Class *Class();
		virtual SNI_Class *VClass();

		SNI_StateValue();
		SNI_StateValue(SNI_Expression *p_State, SNI_Expression *p_Expression);
		virtual ~SNI_StateValue();

		virtual SNI_Expression * Copy() const;

		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual long GetPriority() const;
		virtual string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual bool Equivalent(SNI_Object * p_Other) const;
		virtual size_t Hash() const;


		SNI_Expression *GetState();
		SNI_Expression *GetValue();
	protected:
		virtual void PromoteMembers();

	private:
		SNI_Expression *m_State;
		SNI_Expression *m_Value;
	};
}

#endif // !defined(SNI_STATEVALUE_H_INCLUDED)
