#if !defined(SNI_FRAME_INCLUDED)
#define SNI_FRAME_INCLUDED

#pragma once

#include "../sn/sni_object.h"

namespace SNI
{
	class SNI_Frame : public SNI_Object
	{
		PGC_CLASS(SNI_Frame)
	public:
		SNI_Frame();
		virtual ~SNI_Frame();

		SNI_Expression *ReplaceVariable(SNI_Variable *p_Variable, bool & p_Changed);
		SNI_Expression *CloneReplace(bool & p_Changed, SNI_Variable * p_From, SNI_Variable * p_To, SNI_Expression *p_Expression);
		string GenerateName(SNI_Variable * p_Variable);
	private:
		void PromoteMembers();

		long m_ThreadNum;
		long m_FrameNum;
		SNI_ReplacementList m_ReplacementList;
	};
}

#endif // !defined(SNI_FRAME_INCLUDED)
