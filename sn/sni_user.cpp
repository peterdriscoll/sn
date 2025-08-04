#include "sni_user.h"
#include "sni_thread.h"
#include "sni_delayedprocessor.h"

#include "sn_pch.h"

using namespace skynet;

namespace SNI
{
	/*static*/ SNI_User* SNI_User::GetCurrentUser()
	{
		return SNI_Thread::GetThread()->GetUser();
	}
	// Static function to be used as the thread entry point
	void SNI_User::UserThreadFunc(SNI_User* p_User)
	{
		SNI::SNI_Thread::GetThread()->SetUser(p_User);
	}

	SNI_User::SNI_User(OnErrorHandler *p_ErrorHandler)
		: PGC::PGC_User(p_ErrorHandler)
		, m_Transaction(nullptr)
		, m_DelayedProcessor(nullptr)
		, m_MainThread(SNI_Thread::GetThread())
		, m_Operators(nullptr)
	{
		//SNI::SNI_Thread::ClearThread();
		SNI::SNI_Thread::GetThread()->SetUser(this);
		m_Transaction = new SNI_Transaction();
		m_Operators = new SN::SN_OperatorVariables();

		LogicSetup();
		m_Operators->Setup();

		// Start a thread passing m_User
		// std::thread userThread(UserThreadFunc, this);
		// userThread.detach(); // Detach the thread to run independently
	}

	SNI_User::~SNI_User()
	{
		delete m_Operators;
		delete m_Transaction;
		SNI::SNI_Thread::ResetThread();
	}

	SNI_DelayedProcessor* SNI_User::GetDelayedProcessor()
	{
		if (!m_DelayedProcessor)
		{
			m_DelayedProcessor = new SNI_DelayedProcessor;
			m_Transaction->SubmitTask(m_DelayedProcessor);
		}
		return m_DelayedProcessor;
	}

	SN::SN_OperatorVariables& SNI_User::GetOperators()
	{
		ASSERTM(m_Operators != nullptr, "SNI_User::GetOperators: Null &m_Operators");
		return *m_Operators;
	}

	void SNI_User::LogicSetup()
	{
		skynet::RerunRequest.GetSNI_Error()->MakeRerunRequest();

		// Expression
		Value::Class().GetSNI_Class()->AssertIsAValue(Expression::Class().GetSNI_Class(), skynet::True);
		Define::Class().GetSNI_Class()->AssertIsAValue(Expression::Class().GetSNI_Class(), skynet::True);
		Function::Class().GetSNI_Class()->AssertIsAValue(Expression::Class().GetSNI_Class(), skynet::True);
		Let::Class().GetSNI_Class()->AssertIsAValue(Expression::Class().GetSNI_Class(), skynet::True);
		Local::Class().GetSNI_Class()->AssertIsAValue(Expression::Class().GetSNI_Class(), skynet::True);
		Variable::Class().GetSNI_Class()->AssertIsAValue(Expression::Class().GetSNI_Class(), skynet::True);

		// Value
		Bool::Class().GetSNI_Class()->AssertIsAValue(Value::Class().GetSNI_Class(), skynet::True);
		Char::Class().GetSNI_Class()->AssertIsAValue(Value::Class().GetSNI_Class(), skynet::True);
		String::Class().GetSNI_Class()->AssertIsAValue(Value::Class().GetSNI_Class(), skynet::True);
		StringRef::Class().GetSNI_Class()->AssertIsAValue(Value::Class().GetSNI_Class(), skynet::True);
		Short::Class().GetSNI_Class()->AssertIsAValue(Value::Class().GetSNI_Class(), skynet::True);
		Long::Class().GetSNI_Class()->AssertIsAValue(Value::Class().GetSNI_Class(), skynet::True);
		LongLong::Class().GetSNI_Class()->AssertIsAValue(Value::Class().GetSNI_Class(), skynet::True);
		Float::Class().GetSNI_Class()->AssertIsAValue(Value::Class().GetSNI_Class(), skynet::True);
		Double::Class().GetSNI_Class()->AssertIsAValue(Value::Class().GetSNI_Class(), skynet::True);
		LongDouble::Class().GetSNI_Class()->AssertIsAValue(Value::Class().GetSNI_Class(), skynet::True);
		StateValue::Class().GetSNI_Class()->AssertIsAValue(Value::Class().GetSNI_Class(), skynet::True);
		Vector::Class().GetSNI_Class()->AssertIsAValue(Value::Class().GetSNI_Class(), skynet::True);
		Mapping::Class().GetSNI_Class()->AssertIsAValue(Value::Class().GetSNI_Class(), skynet::True);
		Domain::Class().GetSNI_Class()->AssertIsAValue(Value::Class().GetSNI_Class(), skynet::True);
		Lambda::Class().GetSNI_Class()->AssertIsAValue(Value::Class().GetSNI_Class(), skynet::True);
		Meta::Class().GetSNI_Class()->AssertIsAValue(Value::Class().GetSNI_Class(), skynet::True);
		//FunctionCall::Class().GetSNI_Class()->AssertIsAValue(Value::Class().GetSNI_Class(), skynet::True);

		Char::Class().GetSNI_Class()->AssertIsAValue(String::Class().GetSNI_Class(), skynet::True);
		StringRef::Class().GetSNI_Class()->AssertIsAValue(String::Class().GetSNI_Class(), skynet::True);

		Short::Class().GetSNI_Class()->AssertIsAValue(Long::Class().GetSNI_Class(), skynet::True);
		Long::Class().GetSNI_Class()->AssertIsAValue(LongLong::Class().GetSNI_Class(), skynet::True);

		Float::Class().GetSNI_Class()->AssertIsAValue(Double::Class().GetSNI_Class(), skynet::True);
		Double::Class().GetSNI_Class()->AssertIsAValue(LongDouble::Class().GetSNI_Class(), skynet::True);

		Short::Class().GetSNI_Class()->AssertIsAValue(Float::Class().GetSNI_Class(), skynet::True);
		Long::Class().GetSNI_Class()->AssertIsAValue(Double::Class().GetSNI_Class(), skynet::True);
		LongLong::Class().GetSNI_Class()->AssertIsAValue(LongDouble::Class().GetSNI_Class(), skynet::True);

		Short::Class().GetSNI_Class()->Fix();
		Long::Class().GetSNI_Class()->Fix();
		LongLong::Class().GetSNI_Class()->Fix();
		Float::Class().GetSNI_Class()->Fix();
		Double::Class().GetSNI_Class()->Fix();
		LongDouble::Class().GetSNI_Class()->Fix();
	}

}
