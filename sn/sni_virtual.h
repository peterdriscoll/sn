#if !defined(SNI_VIRTUAL_H_INCLUDED)
#define SNI_VIRTUAL_H_INCLUDED

#pragma once

#include <vector>
#include <list>

#include "sni_value.h"

namespace SN
{
	class SN_Expression;
}

namespace SNI
{
	class SNI_Class;
	class SNI_Expression;

	class ConstructionTree
	{
	public:
		ConstructionTree(const string &p_ParameterName);
		ConstructionTree(const string &p_ParameterName, SN::SN_Expression p_Parameter);
		ConstructionTree(const string &p_ParameterName, SN::SN_Expression p_Parameter, SN::SN_Expression p_CallExpression);
		~ConstructionTree();

		void AddImplementation(SN::SN_Expression p_Implementation);
		SN::SN_Expression CreateExpression();

	private:
		SN::SN_Expression GetParameter();
		void AddParameterList(size_t p_Base, SN::SN_ExpressionList &p_FormalParameterList, SN::SN_Expression p_ImplementationCall);
		SN::SN_Expression BuildExpression(size_t p_Depth);

		SN::SN_Expression BuildCondition(size_t p_Depth, SN::SN_Expression p_Condition, SN::SN_Variable p_ParameterVariable);

		list<ConstructionTree> m_List;
		SN::SN_Expression m_Parameter;
		SN::SN_Variable m_ParameterVariable;
		SN::SN_Expression m_ImplementationCall;
	};

	enum MatchLevel
	{
		Incompatible,
		Unknown,
		Matched
	};

	class SNI_Virtual : public SNI_Value
	{
		PGC_CLASS(SNI_Virtual);
	public:
		static SNI_Class *m_Class;
		static SNI_Class *Class();
		virtual SNI_Class *VClass();

		SNI_Virtual();
		virtual ~SNI_Virtual();

		virtual SNI_Expression * Copy() const;

		virtual string GetTypeName() const;
		virtual string DisplayCpp() const;
		virtual long GetPriority() const;
		virtual string DisplaySN(long priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual bool Equivalent(SNI_Object * p_Other) const;
		virtual size_t Hash() const;

		virtual bool IsFixed() const;

		virtual void Fix();

		virtual SNI_Expression * Clone(long p_MetaLevel, SNI_Frame * p_Frame, bool & p_Changed);

		virtual SN::SN_Expression Call(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel) const;
		virtual SN::SN_Expression PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel) const;

		virtual SN::SN_Expression Unify(SN::SN_ExpressionList * p_ParameterList);
		virtual SN::SN_Error PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Expression, bool p_Define = false);

	protected:
		virtual void PromoteMembers();

	private:
		ConstructionTree *BuildTree();
		SN::SN_Expression CreateImplementation();
		void BuildImplementation();

		vector<SN::SN_Expression> m_CallList;
		long m_DefineId;

		bool m_Fixed;
		SN::SN_Expression m_CallExpression;
	};
}

#endif // !defined(SNI_VIRTUAL_H_INCLUDED)
