#if !defined(SNI_FRAME_INCLUDED)
#define SNI_FRAME_INCLUDED

#pragma once

namespace SNI
{
	class SNI_Frame
	{
		PGC_CLASS(SNI_Frame)
	public:
		SNI_Frame();
		virtual ~SNI_Frame();

		SNI_ReplacementList &GetReplacementList();
		SNI_Expression *ReplaceVariable(SNI_Variable *p_Variable, bool & p_Changed);
	private:
		void PromoteMembers();

		long m_ThreadNum;
		long m_FrameNum;
		SNI_ReplacementList m_ReplacementList;
	};
}

#endif // !defined(SNI_FRAME_INCLUDED)
