#if !defined(SNI_SET_H_INCLUDED)
#define SNI_SET_H_INCLUDED

#pragma once

#include "sni_value.h"
//#include "sni_valueset.h"
#include "sn_value.h"

#include <string>

namespace SNI
{

	class SNI_Set : public SNI_Value
	{
		PGC_CLASS(SNI_Set);
	public:
		static SNI_Class* Class();
		virtual SNI_Class *VClass();

		SNI_Set();
		SNI_Set(const SNI_Set &p_other);
		SNI_Set(const SN::SN_Value &p_other);
		SNI_Set(const std::initializer_list<SN::SN_Value>& values);

		virtual ~SNI_Set();

		virtual SNI_Expression *Copy() const;

		std::string GetTypeName() const;
		std::string DisplayCpp() const;
		std::string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		long GetPriority() const;
		virtual SN::SN_Expression DoEvaluate(long p_MetaLevel = 0) const;
		virtual SN::SN_Expression DoPartialEvaluate(long p_MetaLevel = 0) const;

		virtual size_t Length() const;
		SN::SN_Value operator[](long p_index);
		SN::SN_Value At(const long &p_Value);

		virtual bool Equivalent(SNI_Object * p_Other) const;

		bool HasMember(const SN::SN_Expression &p_Value) const;

		virtual SN::SN_Value DoHasMember(SNI_Value *p_Value) const;
		virtual bool DoIsEmpty() const;

		void AddToSet(const SN::SN_Value &p_Value);
		SN::SN_Set Remove(const SN::SN_Value &p_Other);

	protected:
		virtual void PromoteMembers();

	private:
		SN::SN_ValueList m_SetList;
	};
}

#endif // !defined(SNI_SET_H_INCLUDED)
