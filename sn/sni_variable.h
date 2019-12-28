#if !defined(SNI_VARIABLE_H_INCLUDED)
#define SNI_VARIABLE_H_INCLUDED

#pragma once

#include "sni_namable.h"
#include "sni_vardef.h"

#include "ref.h"

#include <string>
using namespace std;

namespace SNI
{
	class SNI_Value;
	class SNI_DelayedCall;
	class SNI_Frame;

	class SNI_Variable : virtual public SNI_VarDef, virtual public SNI_Namable
	{
		PGC_CLASS(SNI_Variable);
	public:
		SNI_Variable();
		SNI_Variable(const string &p_Name);
		SNI_Variable(const SN::SN_Expression &p_other);
		virtual ~SNI_Variable();

		virtual string GetTypeName() const;
		virtual string GetReferredName() const;
		virtual string DisplayCpp() const;
		virtual string DisplaySN(long p_Priority, SNI_DisplayOptions &p_DisplayOptions) const;
		virtual string DisplayValueSN(long priority, SNI_DisplayOptions & p_DisplayOptions) const;
		virtual string DisplayCall(long p_Priority, SNI_DisplayOptions & p_DisplayOptions, SN::SN_ExpressionList * p_ParameterList, const SNI_Expression *p_DebugSource) const;
		virtual long GetPriority() const;

		virtual bool GetBool() const;
		virtual string GetString() const;
		virtual size_t Count() const;
		virtual size_t Length() const;
		SNI_Frame * GetFrame() const;
		void SetFrame(SNI_Frame *p_Frame);
		string FrameName() const;

		virtual bool IsComplete() const;
		virtual void Complete();

		virtual bool IsRequested() const;
		virtual bool IsNullValue() const;
		virtual bool IsKnownValue() const;
		virtual bool IsKnownTypeValue() const;
		virtual bool IsLambdaValue() const;
		virtual bool IsVariable() const;
		virtual bool IsReferableValue() const;
		virtual void Request();
		virtual SNI_Expression * GetValue(bool p_Request = true) const;
		SNI_Expression * GetSafeValue() const;
		virtual void Simplify();
		virtual SN::SN_Expression SimplifyValue();
		virtual SN::SN_Expression GetVariableValue(bool p_IfComplete);
		virtual void Fix(SN::SN_Expression p_Value);
		virtual void Fix();
		virtual SN::SN_Error AddValue(SN::SN_Expression p_Value, long p_NumWorlds, SNI_World ** p_WorldList, SNI_WorldSet *p_WorldSet);

		virtual SNI_Expression * Clone(SNI_Frame *p_Frame, bool &p_Changed);
		virtual bool Equivalent(SNI_Object * p_Other) const;
		virtual size_t Hash() const;
		virtual SNI_WorldSet * GetWorldSet();

		virtual size_t Cardinality(size_t p_MaxCardinality = CARDINALITY_MAX) const;
		virtual SN::SN_Error ForEach(std::function<SN::SN_Error(const SN::SN_Expression &p_Param, SNI_World*p_World)> p_Action);
		virtual SN::SN_Error ForEachCart(long p_Depth, SNI_Cart * p_Cart);
		virtual void ForEachSplit(SNI_Splitter * p_Splitter);

		virtual SN::SN_Expression Evaluate(long p_MetaLevel = 0) const;
		virtual SN::SN_Expression PartialEvaluate(long p_MetaLevel = 0) const;
		virtual SN::SN_Error SelfAssert();
		virtual SN::SN_Error AssertValue(const SN::SN_Expression &p_Value);
		virtual SN::SN_Error PartialAssertValue(const SN::SN_Expression &p_Expression, bool p_Define = false);

		virtual SN::SN_Expression Call(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel = 0) const;
		virtual SN::SN_Expression PartialCall(SN::SN_ExpressionList * p_ParameterList, long p_MetaLevel = 0) const;
		virtual SN::SN_Expression Unify(SN::SN_ExpressionList * p_ParameterList);
		virtual SN::SN_Error PartialUnify(SN::SN_ParameterList * p_ParameterList, SN::SN_Expression p_Expression, bool p_Define = false);

		void AttachDelayedCall(SNI_DelayedCall *p_Call);
		void SetValue(const SN::SN_Expression & p_Value);
	protected:
		virtual void PromoteMembers();

	private:
		SNI_Expression * m_Value;
		SNI_Frame      * m_Frame;
		bool           m_Requested;
	};

	typedef vector<SNI_Variable> SNI_VariableList;
	typedef vector<const SNI_Variable *> SNI_VariableConstPointerList;
}

#endif // !defined(SNI_VARIABLE_H_INCLUDED)
