#if !defined(SNI_FRAME_INCLUDED)
#define SNI_FRAME_INCLUDED

#pragma once

#include "../sn/sni_object.h"

namespace SNI
{
	class SNI_Variable;

	class SNI_Frame : public SNI_Object
	{
		PGC_CLASS(SNI_Frame)
	public:
		static size_t StackMemory();
		static SNI_Frame *Push(const SNI_Expression *p_Function, SNI_Expression *p_Result);
		static void PushFrame(SNI_Frame * p_Frame);
		static void Pop();
		static SNI_Frame *Top();
		static long GetFrameStackDepth();
		static void DisplayFrameStack(long p_Depth);
		static void DisplayFrameStack(long p_ThreadNum, long p_Depth);
		static void DisplayName(long p_ThreadNum, const string &p_Name);
		static SNI_Variable *LookupVariable(long p_ThreadNum, const string & p_Name);
		SNI_Variable * LookupVariableInFrame(const string & p_Name);

		SNI_Frame();
		SNI_Frame(SN::SN_Expression p_Function);
		virtual ~SNI_Frame();

		SNI_Expression *ReplaceVariable(SNI_Variable *p_Variable, bool & p_Changed);
		SNI_Expression *CloneReplace(bool & p_Changed, SNI_Variable * p_From, SNI_Variable * p_To, SNI_Expression *p_Expression);
		SNI_Variable *GetVariable(size_t p_VariableNum);
		SNI_Variable *GetResult();
		string NameSuffix();
		string GetLogDescription(SNI_Manager *p_Manager);
		string GetLogShortDescription(SNI_Manager *p_Manager);
		string GetBreakPoint();
		string GetBreakPointJS();
		void SetBreakPoint(const string & p_BreakPoint, const string & p_BreakPointJS);
		bool HasCode() const;
		void SetCloneFunction(const SNI_Expression *p_Function);

		SNI_Variable *CreateTemporary();
		SNI_Variable *CreateSplitVariable();
		SNI_Variable *CreateVariable(const string & p_Name, const string & p_DomainName);
		SNI_Variable *CreateParameter(size_t p_ParamNum, SN::SN_Expression p_Value);
		SNI_Variable *CreateParameterByName(const string &p_ParamName, SN::SN_Expression p_Value);

		void WriteWebFrame(ostream &p_Stream, size_t p_FrameStackPos, size_t p_DebugFieldWidth, SNI::SNI_DisplayOptions &p_DisplayOptions);
		void WriteJS(ostream & p_Stream, size_t p_FrameStackPos, size_t p_DebugFieldWidth, SNI::SNI_DisplayOptions &p_DisplayOptions);
		void WriteStackJS(ostream & p_Stream, string &p_Delimeter, size_t p_DebugFieldWidth, SNI::SNI_DisplayOptions & p_DisplayOptions);
		void WriteVariable(ostream &p_Stream, SN::SN_Expression &p_Variable, SN::SN_Expression &p_Value, long j, const string &p_Prefix, size_t p_DebugFieldWidth, SNI::SNI_DisplayOptions &p_DisplayOptions);

		void RegisterCardinality(size_t p_Cardinality);
		void PromoteExternals(PGC::PGC_Transaction * p_Transaction);

		void AttachParameter(SN::SN_Expression p_Param);

		void RecordSavePoint(vector<bool> p_SavePoint);
		void RestoreSavePoint(vector<bool> p_SavePoint);
	private:
		virtual void PromoteMembers();

		SN::SN_Expression              m_Function;
		long                           m_ThreadNum;
		long                           m_FrameNum;
		long                           m_StepCount;
		SNI_ReplacementList            m_ReplacementList;
		SNI_VariablePointerList        m_VariableList;
		SN::SN_Expression              m_CloneFunction;
		string						   m_DebugId;
		string						   m_BreakPoint;
		string						   m_BreakPointJS;
		size_t						   m_Cardinality;
		size_t						   m_StackUsage;
		SNI_World *                    m_ContextWorld;
		size_t						   m_ResultIndex;
	};
}

#endif // !defined(SNI_FRAME_INCLUDED)
