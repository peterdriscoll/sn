#if !defined(SNI_INSTANCE_H_INCLUDED)
#define SNI_INSTANCE_H_INCLUDED

#pragma once

#include <string>

#include "sni_null.h"

namespace SNI
{
	class SNI_Instance : public SNI_Null
	{
		PGC_CLASS(SNI_Instance);

	public:
		virtual SNI_Class * Class();
		virtual SNI_Class *VClass();

		SNI_Instance();
		SNI_Instance(const SNI_Instance &p_Other);
		virtual ~SNI_Instance();

		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual long GetPriority() const;

		virtual bool IsKnownTypeValue() const;

		// Inheritance
		virtual SN::SN_Error AssertIsAValue(const SNI_Value * p_Parent, SN::SN_Expression p_Result);
		virtual SN::SN_Value DoIsA(const SNI_Value * p_Parent) const;
		virtual SN::SN_Value DoHasA(SNI_Value * p_Member, SNI_Value * p_Name) const;

		void Fix();

	private:
		virtual void PromoteMembers();

		SNI_Class *m_Class;
		bool m_Fixed;
	};

	typedef vector<SNI_Instance *> SNI_InstanceList;
}

#endif // !defined(SNI_BOOL_H_INCLUDED)
