#if !defined(SNI_REPLACEMENT_H_INCLUDED)
#define SNI_REPLACEMENT_H_INCLUDED

#pragma once

#include "sn_variable.h"

#include <vector>

namespace SNI
{
	class SNI_Replacement
	{
	public:
		SNI_Replacement();
		SNI_Replacement(const SNI_Variable *p_From, SNI_Variable *p_To);
		virtual ~SNI_Replacement();

		SNI_Variable * ReplaceVariable(SNI_Variable * p_Variable);

		void PromoteMembersExternal(PGC::PGC_Base * p_Base);

	private:
		const SNI_Variable *m_From;
		SNI_Variable *m_To;
	};

	typedef std::vector<SNI_Replacement> SNI_ReplacementList;
}

#endif // !defined(SNI_REPLACEMENT_H_INCLUDED)
