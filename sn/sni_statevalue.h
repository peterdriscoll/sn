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
		
		static SNI_Class *Class();
		virtual SNI_Class *VClass();

		SNI_StateValue();
		SNI_StateValue(SNI_Expression *p_Expression, SNI_Expression *p_State);
		virtual ~SNI_StateValue();

		virtual SNI_Expression * Copy() const;

		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual long GetPriority() const;
		virtual string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual bool Equivalent(SNI_Object * p_Other) const;
		virtual size_t Hash() const;
		virtual SNI_Expression* Clone(long p_MetaLevel, SNI_Frame* p_Frame, bool& p_Changed);

		virtual SN::SN_Expression DoEvaluate(long) const;
		virtual SN::SN_Expression DoPartialEvaluate(long) const;

		SNI_Expression *GetValue();
		SNI_Expression* GetState();

		virtual SN::SN_Expression DoGetValue() const;
		virtual SN::SN_Expression DoGetState() const;
	protected:
		virtual void PromoteMembers();

	private:
		SNI_Expression *m_Value;
		SNI_Expression* m_State;
	};
}

#endif // !defined(SNI_STATEVALUE_H_INCLUDED)
