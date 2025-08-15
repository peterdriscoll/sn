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

	SNI_Thread* SNI_User::GetThreadByNumber(size_t p_ThreadNum)
	{
		if (p_ThreadNum < m_ThreadList.size())
		{
			return m_ThreadList[p_ThreadNum];
		}
		return NULL;
	}

	size_t SNI_User::GetNumThreads()
	{
		return m_ThreadList.size();
	}

	void SNI_User::AddThread(SNI_Thread* p_Thread)
	{
		if (p_Thread)
		{
			m_ThreadList.push_back(p_Thread);
		}
		else
		{
			ASSERTM(false, "SNI_User::AddThread: Null thread pointer");
		}
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

	std::string SNI_User::StepCountJS()
	{
		stringstream ss;
		cout << "StepPointJS\n";
		WriteStepCountListJS(ss);
		return ss.str();
	}

	void SNI_User::WriteStepCounts(ostream& p_Stream)
	{
		p_Stream << "<div><table class='thread'>\n";
		p_Stream << "<caption>Threads</caption>\n";
		p_Stream << "<tr>\n";
		std::string separator;
		for (size_t k = 0; k < m_ThreadList.size(); k++)
		{
			SNI_Thread* l_thread = m_ThreadList[k];
			if (l_thread)
			{
				l_thread->WriteStepCount(p_Stream);
			}
		}
		p_Stream << "</tr>\n";
		p_Stream << "</table></div>\n";
	}

	void SNI_User::WriteStepCountListJS(ostream& p_Stream)
	{
		p_Stream << "{\"records\":[\n";
		std::string delimeter = " ";
		for (size_t k = 0; k < m_ThreadList.size(); k++)
		{
			SNI_Thread* l_thread = m_ThreadList[k];
			if (l_thread)
			{
				p_Stream << delimeter;

				l_thread->WriteStepCountJS(p_Stream, delimeter);
				delimeter = ",\n";
			}
		}
		p_Stream << "\n]}\n";
	}
}
