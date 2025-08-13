#if !defined(SNI_FRAME_INCLUDED)
#define SNI_FRAME_INCLUDED

#pragma once

#include "sni_object.h"
#include "sni_expression.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

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
		static size_t GetFrameStackDepth();
		static void DisplayFrameStack(size_t p_Depth);
		static void DisplayFrameStack(long p_ThreadNum, long p_Depth);
		static void DisplayName(long p_ThreadNum, const std::string &p_Name);
		static SNI_Variable *LookupVariable(long p_ThreadNum, const std::string & p_Name);
		SNI_Variable * LookupVariableInFrame(const std::string & p_Name);

		SNI_Frame();
		SNI_Frame(SN::SN_Expression p_Function);
		virtual ~SNI_Frame();

		SNI_Expression *ReplaceVariable(SNI_Variable *p_Variable, bool & p_Changed);
		SNI_Expression *CloneReplace(bool & p_Changed, SNI_Variable * p_From, SNI_Variable * p_To, SNI_Expression *p_Expression);
		SNI_Variable *GetVariable(size_t p_VariableNum);
		SNI_Variable *GetResult();
		std::string NameSuffix();
		std::string GetLogDescription(SNI_Manager *p_Manager);
		std::string GetLogShortDescription(SNI_Manager *p_Manager);
		std::string GetBreakPoint();
		std::string GetBreakPointJS();
		void SetBreakPoint(const std::string & p_BreakPoint, const std::string & p_BreakPointJS);
		bool HasCode() const;
		void SetCloneFunction(const SNI_Expression *p_Function);

		SNI_Variable *CreateTemporary();
		SNI_Variable *CreateSplitVariable();
		SNI_Variable *CreateVariable(const std::string & p_Name, const std::string & p_DomainName, enum skynet::DefineType p_DefineType = skynet::DefineType::Inname);
		SNI_Variable *CreateParameter(size_t p_ParamNum, SN::SN_Expression p_Value);
		SNI_Variable *CreateParameterByName(const std::string &p_ParamName, SN::SN_Expression p_Value);

		void WriteWebFrame(ostream &p_Stream, size_t p_FrameStackPos, size_t p_DebugFieldWidth, SNI::SNI_DisplayOptions &p_DisplayOptions);
		void WriteJSON(ostream & p_Stream, size_t p_FrameStackPos, size_t p_DebugFieldWidth, SNI::SNI_DisplayOptions &p_DisplayOptions);
		void WriteCallJS(boost::property_tree::ptree &callStackNode, size_t p_DebugFieldWidth, SNI::SNI_DisplayOptions & p_DisplayOptions);
		void WriteStackJS(ostream & p_Stream, std::string &p_Delimeter, size_t p_DebugFieldWidth, SNI::SNI_DisplayOptions & p_DisplayOptions);
		void WriteCardinalityJSON(ostream& p_Stream, SN::SN_Expression& p_Value, long p_ParamNum, const std::string& p_Prefix) const;
		void WriteVariable(ostream& p_Stream, SN::SN_Expression& p_Variable, SN::SN_Expression& p_Value, long j, const std::string& p_Prefix, size_t p_DebugFieldWidth, SNI::SNI_DisplayOptions& p_DisplayOptions);

		void RegisterCardinality(size_t p_Cardinality);
		void PromoteExternals(PGC::PGC_Transaction * p_Transaction);

		void AttachParameter(SN::SN_Expression p_Param);

		void AddVariables(SNI_VariablePointerMap& p_Map);

		void RecordSavePoint(vector<bool> p_SavePoint);
		void RestoreSavePoint(vector<bool> p_SavePoint);
	private:
		virtual void PromoteMembers();

		SN::SN_Expression              m_Function;
		size_t                         m_ThreadNum;
		long                           m_FrameNum;
		size_t                         m_StepCount;
		SNI_ReplacementList            m_ReplacementList;
		SNI_VariablePointerList        m_VariableList;
		SN::SN_Expression              m_CloneFunction;
		std::string						   m_DebugId;
		std::string						   m_BreakPoint;
		std::string						   m_BreakPointJS;
		size_t						   m_Cardinality;
		size_t						   m_StackUsage;
		SNI_World *                    m_ContextWorld;
		size_t						   m_ResultIndex;
	};
}

#endif // !defined(SNI_FRAME_INCLUDED)
