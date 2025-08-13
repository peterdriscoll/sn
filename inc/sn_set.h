#if !defined(SN_SET_H_INCLUDED)
#define SN_SET_H_INCLUDED

#pragma once

#include <string>
#include "exp_ctrl_sn.h"
#include "sn_base.h"

namespace SNI
{
	class SNI_Set;
}

namespace SN
{
	class SN_EXPORT SN_Set : public SN_Base
	{
	public:
		static SN_Class Class();

		SN_Set();
		SN_Set(const SN_Set &p_other);
		SN_Set(const SN_Value &p_other);
		SN_Set(const std::initializer_list<SN_Value> &values); // Initializer list.
		SN_Set(const SN_ValueSet &p_other);

		virtual ~SN_Set();

		// Comparison
		SN_Expression operator ==(const SN_Expression &p_Other) const;
		SN_Expression operator !=(const SN_Expression &p_Other) const;

		SN_Expression operator +(const SN_Expression &p_Other) const;
		SN_Expression operator -(const SN_Expression &p_Other) const;

		SN_Expression HasMember(const SN_Expression &p_Set) const;

		size_t Count();
		SN_Value operator[](long p_index);

		SN_Expression IsA(const SN_Expression &p_Parent) const;

		SNI::SNI_Set* GetSNI_Set();
		SNI::SNI_Set* GetSNI_Set() const;
	};

	typedef std::vector<SN_Set> SN_SetList;
}

#endif // !defined(SN_SET_H_INCLUDED)
