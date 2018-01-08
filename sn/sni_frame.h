#if !defined(SNI_FRAME_INCLUDED)
#define SNI_FRAME_INCLUDED

#pragma once

#include "../sn/sni_object.h"

namespace SNI
{
	class SNI_Variable;

	class SNI_Frame;
	typedef vector<SNI_Frame *> SNI_FrameList;

	class SNI_Frame : public SNI_Object
	{
		PGC_CLASS(SNI_Frame)
	public:
		static SNI_Frame *Push(const SNI_Variable *p_Variable);
		static void PushFrame(SNI_Frame * p_Frame);
		static void Pop();
		static SNI_Frame *Top();
		static long GetThreadNum();
		static long GetFrameStackDepth();
		static void DisplayFrameStack(long p_Depth);

		SNI_Frame();
		SNI_Frame(SN::SN_Expression p_Function);
		virtual ~SNI_Frame();

		SNI_Expression *ReplaceVariable(SNI_Variable *p_Variable, bool & p_Changed);
		SNI_Expression *CloneReplace(bool & p_Changed, SNI_Variable * p_From, SNI_Variable * p_To, SNI_Expression *p_Expression);
		string NameSuffix();
		string GetLogDescription();
		string GetLogShortDescription();
		SNI_Variable *CreateTemporary();

	private:
		void PromoteMembers();

		SN::SN_Expression       m_Function;
		long                    m_ThreadNum;
		long                    m_FrameNum;
		SNI_ReplacementList     m_ReplacementList;
		SNI_VariablePointerList m_VariableList;
	};
}

#endif // !defined(SNI_FRAME_INCLUDED)
