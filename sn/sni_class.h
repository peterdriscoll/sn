#if !defined(SNI_CLASS_H_INCLUDED)
#define SNI_CLASS_H_INCLUDED

#pragma once

#include <string>

#include "sni_null.h"

namespace SNI
{
	class SNI_Inherits
	{
	public:
		SNI_Inherits(const SNI_Value *p_Parent, SNI_Value *p_Result);
		virtual ~SNI_Inherits();

		const SNI_Value *GetParent() const;
		const SNI_Value *GetResult() const;

		virtual SN::SN_Error AssertValue(SN::SN_Expression p_Result);

		void PromoteMembersExternal(PGC::PGC_Base * p_Base);

	private:
		const SNI_Value *m_Parent;
		SNI_Value *m_Result;
	};

	typedef vector<SNI_Inherits> SNI_Inherits_List;

	class SNI_Class : public SNI_Null
	{
		PGC_CLASS(SNI_Class);
	public:
		SNI_Class(const string &p_ClassName);
		SNI_Class(const SNI_Class &p_Other);
		virtual ~SNI_Class();

		virtual SNI_Expression * Copy() const;
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

		string m_ClassName;
		bool m_Fixed;
		SNI_Inherits_List m_InheritList;
	};

	typedef vector<SNI_Class *> SNI_ClassList;
}

#endif // !defined(SNI_CLASS_H_INCLUDED)
