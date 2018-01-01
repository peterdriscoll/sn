#if !defined(SNI_REPLACEMENT_H_INCLUDED)
#define SNI_REPLACEMENT_H_INCLUDED

#pragma once

#include "sn_variable.h"

#include <vector>
using namespace std;

namespace SNI
{
	class SNI_Replacement
	{
	public:
		SNI_Replacement();
		SNI_Replacement(const SNI_Variable *p_From, SNI_Variable *p_To);
		virtual ~SNI_Replacement();

		SNI_Variable * ReplaceVariable(SNI_Variable * p_Variable);

	protected:
		virtual void PromoteMembers();


	private:
		const SNI_Variable *m_From;
		SNI_Variable *m_To;
	};

	typedef vector<SNI_Replacement> SNI_ReplacementList;
}

#endif // !defined(SNI_REPLACEMENT_H_INCLUDED)
