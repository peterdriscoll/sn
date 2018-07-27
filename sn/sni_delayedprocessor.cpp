//  SNI_DelayedProcessor class
//
//  Description:
//      Some function calls do not have sufficient information yet to calculate the unknown variables.
//      The processor manages a list of delayed calls and performs calls with the greatest determinancy first
//      

#include "sni_delayedprocessor.h"
#include "sni_delayedcall.h"
#include "sni_world.h"

#include "sn_pch.h"

namespace SNI
{
	/*static*/ SNI_DelayedProcessor *SNI_DelayedProcessor::GetProcessor()
	{
		SNI_DelayedProcessor* task = dynamic_cast<SNI_DelayedProcessor *>(SN::SN_Transaction::TopTransaction()->TopTask());
		if (!task)
		{
			task = new SNI_DelayedProcessor(SNI_Thread::TopManager());
			SN::SN_Transaction::TopTransaction()->SubmitTask(task);
		}
		return task;
	}

	SNI_DelayedProcessor::SNI_DelayedProcessor(SNI_Manager *p_Manager)
		: m_Processing(false)
		, m_Manager(p_Manager)
	{
	}

	SNI_DelayedProcessor::~SNI_DelayedProcessor()
	{
		delete m_Manager;
	}

	// Create a delayed call and link it in as the value of the variables..
	void SNI_DelayedProcessor::Delay(SN::SN_FunctionDef p_Function, SN::SN_Expression *p_ParameterList, SNI_World *p_World)
	{
		SNI_DelayedCall *call = new SNI_DelayedCall(p_Function, p_ParameterList, SNI_Frame::Top(), p_World);
		call->LinkToVariables();
		m_DelayedCallList.push_back(call);
		if (call->IsCallRequested())
		{
			Request(call);
		}
	}

	void SNI_DelayedProcessor::DelayCall(SNI_DelayedCall * p_Call, SNI_World * p_World)
	{
		if (p_Call)
		{
			p_Call->SetWorld(p_World);
			p_Call->LinkToVariables();
		}
	}

	//  Post the delayed call to the list for processing.
	void SNI_DelayedProcessor::Request(SNI_DelayedCall *p_Call)
	{
		if (!p_Call->IsScheduled())
		{
			if (p_Call->IsCallRequested())
			{
				p_Call->MarkScheduled();
				m_DelayedCallList.push_front(p_Call);
				Display();
			}
		}
		if (!m_Processing)
		{
			Process();
		}
	}

	void SNI_DelayedProcessor::Run()
	{
		Process();
	}

	bool SNI_DelayedProcessor::Complete()
	{
		Process();
		return m_DelayedCallList.size() == 0;
	}

	void SNI_DelayedProcessor::Promote(PGC::PGC_Transaction * p_Transaction)
	{
	}

	unordered_map<string, SN::SN_String>& SNI_DelayedProcessor::GetPreventReread()
	{
		return m_PreventReread;
	}

	void SNI_DelayedProcessor::Display()
	{
		LOG(WriteLine(SN::DebugLevel, "Delayed Call List"));
		for (SNI_DelayedCall *d : m_DelayedCallList)
		{
			d->Display();
		}
	}

	// Process those calls in the list that can be processed, starting with the least cardinality.
	// Here, cardinality refers to the number of values. Nothing to do with multi threading.
	bool SNI_DelayedProcessor::Process()
	{
		m_Processing = true;
		bool found = true;
		bool success = false;
		do
		{
			SNI_DelayedCall *call = NULL;
			size_t indet = CARDINALITY_MAX;
			SNI_DelayedCallList::iterator it;
			m_SearchLock.lock();
			for (SNI_DelayedCallList::iterator loopIt = m_DelayedCallList.begin(); loopIt != m_DelayedCallList.end(); loopIt++)
			{
				SNI_DelayedCall *loopCall = *loopIt;
				if (!loopCall->IsLocked())
				{
					size_t loopIndet = loopCall->CallCardinality();
					if (loopIndet < indet)
					{
						call = loopCall;
						indet = loopIndet;
						it = loopIt;
					}
				}
			}
			if (indet < CARDINALITY_MAX)
			{
				call->Lock();
			}

			m_SearchLock.unlock();
			if (indet < CARDINALITY_MAX)
			{
				if (!call->Run())
				{
					m_FailedList.push_back(call);
				}
				it = m_DelayedCallList.erase(it);
				call->Unlock();
				success = true;
			}
			else
			{
				found = false;
			}
		} while (found);
		m_Processing = false;
		return success;
	}
}
